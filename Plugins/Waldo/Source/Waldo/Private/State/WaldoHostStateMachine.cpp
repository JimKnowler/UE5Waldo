#include "WaldoHostStateMachine.h"

#include "LogWaldo.h"
#include "Command/WaldoCommandDecoder.h"
#include "Command/WaldoCommandEncoder.h"

UWaldoHostStateMachine::UWaldoHostStateMachine()
{
	CommandByteStream = CreateDefaultSubobject<UWaldoCommandByteStream>(TEXT("CommandByteStream"));
}

void UWaldoHostStateMachine::Reset(USerialPort* inSerialPort)
{
	SerialPort = inSerialPort;
	State = EWaldoHostState::Reset;

	CommandByteStream->Setup(inSerialPort);

	LastCommandTime = GetTime();
}

bool UWaldoHostStateMachine::Tick(float DeltaTime)
{
	while (true)
	{
		const float Time = GetTime();
	
		FWaldoCommand Command;
		if (!CommandByteStream->Receive(Command))
		{
			const float ElapsedTime = Time - LastCommandTime;

			if (ElapsedTime > MaxTimeBetweenCommands)
			{
				CommandByteStream->Reset();
				
				FWaldoCommand Reset;
				FWaldoCommandEncoder(Reset).Reset();
				CommandByteStream->Send(Reset);

				LastCommandTime = Time;
			}
			
			break;
		}

		LastCommandTime = Time;
		
		if (!Process(Command))
		{
			UE_LOG(LogWaldo, Warning, TEXT("%hs - ignoring command [%s] while in state [%s]"), __FUNCTION__, *ToString(Command.GetType()), *ToString(State));
		}
	}

	return true;
}

bool UWaldoHostStateMachine::Process(const FWaldoCommand& Command)
{
	switch (Command.GetType())
	{
	case EWaldoCommandType::Reset:
		{
			FWaldoCommand Ack;
			FWaldoCommandEncoder(Ack).AcknowledgeReset();
			CommandByteStream->Send(Ack);

			UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Reset'"), __FUNCTION__);
			
			CommandByteStream->Reset();
			State = EWaldoHostState::Ready;
			
			OnReset.Broadcast();

			return true;
		}
	case EWaldoCommandType::Message:
		{
			FWaldoCommandDecoder Decoder(Command);
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
				case EWaldoCommandType::RegisterInput:
					{
						FWaldoCommandDecoder Decoder(Command);
						FWaldoInput Input;
						ensure(Decoder.RegisterInput(Input));
						
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Register Input' [%s] Id [%d] Pin [%d] Type [%s]"), __FUNCTION__, *Input.Label, Input.Id, Input.Pin, *ToString(Input.Type));

						OnRegisterInput.Broadcast(Input);
					}
					return true;
				case EWaldoCommandType::StartFrame:
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
				case EWaldoCommandType::InputValue:
					{
						FWaldoCommandDecoder Decoder(Command);
						FWaldoInputValue InputValue;
						ensure(Decoder.InputValue(InputValue));
						
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Input Value' [%d] for Id [%d]"), __FUNCTION__, InputValue.Value, InputValue.Id);

						OnInputValue.Broadcast(InputValue);
					}
					return true;
				case EWaldoCommandType::EndFrame:
					{
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'End Frame'"), __FUNCTION__);
												
						FWaldoCommand Ack;
						FWaldoCommandEncoder(Ack).AcknowledgeFrame();
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
