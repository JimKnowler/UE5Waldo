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
				FWaldoCommandEncoder(Reset).EncodeReset();
				if (!CommandByteStream->Send(Reset))
				{
					UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to send 'Reset'"), __FUNCTION__);
					return false;
				}

				LastCommandTime = Time;
			}
			
			break;
		}

		LastCommandTime = Time;

		bool HasErrored = false;
		if (!Process(Command, HasErrored))
		{
			if (HasErrored)
			{
				// TODO: state could have changed while trying to process the command
				
				UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to process command [%s] while in state [%s]"), __FUNCTION__, *ToString(Command.GetType()), *ToString(State));
				return false;
			}

			UE_LOG(LogWaldo, Warning, TEXT("%hs - ignoring command [%s] while in state [%s]"), __FUNCTION__, *ToString(Command.GetType()), *ToString(State));
		}
	}

	return true;
}

bool UWaldoHostStateMachine::Process(const FWaldoCommand& Command, bool& OutHasErrored)
{
	switch (Command.GetType())
	{
	case EWaldoCommandType::Reset:
		{
			UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Reset'"), __FUNCTION__);
			
			FWaldoCommand Ack;
			FWaldoCommandEncoder(Ack).EncodeAcknowledgeReset();
			if (!CommandByteStream->Send(Ack))
			{
				UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to send 'AcknowledgeReset'"), __FUNCTION__);
				OutHasErrored = true;
				return false;
			}
			
			CommandByteStream->Reset();
			State = EWaldoHostState::Ready;
			
			OnReset.Broadcast();

			return true;
		}
	case EWaldoCommandType::Message:
		{
			FWaldoCommandDecoder Decoder(Command);
			FString Message;
			if (!Decoder.DecodeMessage(Message))
			{
				UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to decode 'Message'"), __FUNCTION__);
				OutHasErrored = true;
				return false;
			}

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
						if (!ensure(Decoder.DecodeRegisterInput(Input)))
						{
							UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to decode 'RegisterInput'"), __FUNCTION__);
							OutHasErrored = true;
							return false;
						}
						
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
						if (!ensure(Decoder.DecodeInputValue(InputValue)))
						{
							UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to decode 'InputValue'"), __FUNCTION__);
							OutHasErrored = true;
							return false;
						}
						
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'Input Value' [%d] for Id [%d]"), __FUNCTION__, InputValue.Value, InputValue.Id);

						OnInputValue.Broadcast(InputValue);
					}
					return true;
				case EWaldoCommandType::EndFrame:
					{
						UE_LOG(LogWaldo, Log, TEXT("%hs - received 'EndFrame'"), __FUNCTION__);
												
						FWaldoCommand Ack;
						FWaldoCommandEncoder(Ack).EncodeAcknowledgeFrame();
						if (!CommandByteStream->Send(Ack))
						{
							UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to send 'AcknowledgeFrame'"), __FUNCTION__);
							OutHasErrored = true;
							return false;
						}
				
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
			OutHasErrored = true;
			break;
	}

	return false;
}

float UWaldoHostStateMachine::GetTime() const
{
	return FPlatformTime::Seconds();
}
