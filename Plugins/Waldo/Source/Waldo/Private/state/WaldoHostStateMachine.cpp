#include "WaldoHostStateMachine.h"

#include "LogWaldo.h"
#include "command/CommandDecoder.h"
#include "command/CommandEncoder.h"

UWaldoHostStateMachine::UWaldoHostStateMachine()
{
	CommandByteStream = CreateDefaultSubobject<UCommandByteStream>(TEXT("CommandByteStream"));
}

void UWaldoHostStateMachine::Reset(USerialPort* inSerialPort)
{
	SerialPort = inSerialPort;
	State = EWaldoHostState::Reset;

	CommandByteStream->Setup(inSerialPort);

	LastCommandTime = GetTime();
}

void UWaldoHostStateMachine::Tick(float DeltaTime)
{
	while (true)
	{
		const float Time = GetTime();
	
		FCommand Command;
		if (!CommandByteStream->Receive(Command))
		{
			const float ElapsedTime = Time - LastCommandTime;

			if (ElapsedTime > MaxTimeBetweenCommands)
			{
				CommandByteStream->Reset();
				
				FCommand Reset;
				FCommandEncoder(Reset).Reset();
				CommandByteStream->Send(Reset);

				LastCommandTime = Time;
			}
			
			return;
		}

		LastCommandTime = Time;
		
		if (!Process(Command))
		{
			UE_LOG(LogWaldo, Warning, TEXT("%hs - ignoring command [%s] while in state [%s]"), __FUNCTION__, *ToString(Command.GetType()), *ToString(State));
		}	
	}
}

bool UWaldoHostStateMachine::Process(const FCommand& Command)
{
	switch (Command.GetType())
	{
	case ECommandType::Reset:
		{
			FCommand Ack;
			FCommandEncoder(Ack).AcknowledgeReset();
			CommandByteStream->Send(Ack);

			UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Reset'"), __FUNCTION__);
			
			CommandByteStream->Reset();
			State = EWaldoHostState::Ready;
			
			OnReset.Broadcast();

			return true;
		}
	case ECommandType::Message:
		{
			FCommandDecoder Decoder(Command);
			FString Message;
			Decoder.Message(Message);

			UE_LOG(LogWaldo, Log, TEXT("%hs - received 'message' [%s]"), __FUNCTION__, *Message);

			OnMessage.Broadcast(Message);
			
			return true;		
		}
	default:
		break;
	}
	
	switch (State)
	{
		case EWaldoHostState::Reset:
		break;
		case EWaldoHostState::Ready:
			switch (Command.GetType())
			{
				case ECommandType::RegisterInput:
					{
						FCommandDecoder Decoder(Command);
						FInput Input;
						ensure(Decoder.RegisterInput(Input));
						
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Register Input' [%s] Id [%d] Pin [%d] Type [%s]"), __FUNCTION__, *Input.Label, Input.Id, Input.Pin, *ToString(Input.Type));

						OnRegisterInput.Broadcast(Input);
					}
					return true;
				case ECommandType::StartFrame:
					UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Start Frame'"), __FUNCTION__);
									
					State = EWaldoHostState::Frame;

					OnFrameStart.Broadcast();
					
					return true;
				default:
					break;
			}
		break;
		case EWaldoHostState::Frame:
			switch (Command.GetType())
			{
				case ECommandType::InputValue:
					{
						FCommandDecoder Decoder(Command);
						FInputValue InputValue;
						ensure(Decoder.InputValue(InputValue));
						
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Input Value' [%d] for Id [%d]"), __FUNCTION__, InputValue.Value, InputValue.Id);

						OnInputValue.Broadcast(InputValue);
					}
					return true;
				case ECommandType::EndFrame:
					{
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'End Frame'"), __FUNCTION__);
												
						FCommand Ack;
						FCommandEncoder(Ack).AcknowledgeFrame();
						CommandByteStream->Send(Ack);
				
						State = EWaldoHostState::Ready;

						OnFrameEnd.Broadcast();

						return true;
					}
				default:
					break;
			}
		break;
		default:
			UE_LOG(LogWaldo, Warning, TEXT("%hs - unknown state [%s]"), __FUNCTION__, *ToString(State));
			break;
	}

	return false;
}

float UWaldoHostStateMachine::GetTime() const
{
	return FPlatformTime::Seconds();
}
