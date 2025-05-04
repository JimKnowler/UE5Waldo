#include "WaldoComponent.h"

#include "LogWaldo.h"
#include "state/WaldoHostStateMachine.h"

UWaldoComponent::UWaldoComponent()
{
	StateMachine = CreateDefaultSubobject<UWaldoHostStateMachine>("StateMachine");

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

bool UWaldoComponent::Connect(const FSerialPortDevice& Device, int BaudRate)
{
	if (IsConnected())
	{
		UE_LOG(LogWaldo, Error, TEXT("%hs - already connected"), __FUNCTION__);
		return false;
	}

	UE_LOG(LogWaldo, Log, TEXT("%hs - connecting to [%s]"), __FUNCTION__, *Device.Name);

	SerialPort = NewObject<USerialPort>(this);
	if (!SerialPort->Open(Device, BaudRate))
	{
		SerialPort = nullptr;

		UE_LOG(LogWaldo, Warning, TEXT("%hs - failed to connect"), __FUNCTION__);
		return false;
	}
	
	UE_LOG(LogWaldo, Log, TEXT("%hs - connected successfully"), __FUNCTION__);

	RegisterCallbacks();
	
	StateMachine->Reset(SerialPort);
	
	return true;
}

bool UWaldoComponent::IsConnected() const
{
	const bool bIsConnected = IsValid(SerialPort);

	return bIsConnected;
}

bool UWaldoComponent::Disconnect()
{
	if (!IsConnected())
	{
		UE_LOG(LogWaldo, Error, TEXT("%hs - not connected"), __FUNCTION__);
		return false;
	}

	UE_LOG(LogWaldo, Log, TEXT("%hs - disconnecting"), __FUNCTION__);

	UnregisterCallbacks();
	
	SerialPort->Close();
	SerialPort = nullptr;

	UE_LOG(LogWaldo, Log, TEXT("%hs - disconnected"), __FUNCTION__);
	
	return true;
}

void UWaldoComponent::BindDelegateForInput(const FString& Label, FDelegateInputValue Delegate)
{
	InputBindings.Add(FInputBound{
		.Label = Label,
		.Delegate = Delegate
	});
}

void UWaldoComponent::UnbindDelegateForInput(const FString& Label, FDelegateInputValue Delegate)
{
	for (int i = InputBindings.Num() - 1; i >= 0; i--)
	{
		FInputBound& Binding = InputBindings[i];
		if ((Binding.Label == Label) && (Binding.Delegate == Delegate))
		{
			InputBindings.RemoveAt(i);
		}
	}
}

void UWaldoComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsConnected())
	{
		StateMachine->Tick(DeltaTime);
	}
}

void UWaldoComponent::RegisterCallbacks()
{
	StateMachine->OnReset.AddDynamic(this, &ThisClass::HandleReset);
	StateMachine->OnMessage.AddDynamic(this, &ThisClass::HandleMessage);
	StateMachine->OnRegisterInput.AddDynamic(this, &ThisClass::HandleRegisterInput);
	StateMachine->OnFrameStart.AddDynamic(this, &ThisClass::HandleFrameStart);
	StateMachine->OnInputValue.AddDynamic(this, &ThisClass::HandleInputValue);
	StateMachine->OnFrameEnd.AddDynamic(this, &ThisClass::HandleFrameEnd);
}

void UWaldoComponent::UnregisterCallbacks()
{
	StateMachine->OnReset.RemoveDynamic(this, &ThisClass::HandleReset);
	StateMachine->OnMessage.RemoveDynamic(this, &ThisClass::HandleMessage);
	StateMachine->OnRegisterInput.RemoveDynamic(this, &ThisClass::HandleRegisterInput);
	StateMachine->OnFrameStart.RemoveDynamic(this, &ThisClass::HandleFrameStart);
	StateMachine->OnInputValue.RemoveDynamic(this, &ThisClass::HandleInputValue);
	StateMachine->OnFrameEnd.RemoveDynamic(this, &ThisClass::HandleFrameEnd);
}

void UWaldoComponent::HandleReset()
{
	RegisteredInputs.Reset();
	FrameInputValues.Reset();

	OnReset.Broadcast();
}

void UWaldoComponent::HandleMessage(const FString& Message)
{
	OnMessage.Broadcast(Message);
}

void UWaldoComponent::HandleRegisterInput(const FInput& Input)
{
	RegisteredInputs.Add(Input);
}

void UWaldoComponent::HandleFrameStart()
{
	FrameInputValues.Reset();
}

void UWaldoComponent::HandleInputValue(const FInputValue& InputValue)
{
	FrameInputValues.Add(InputValue);
}

void UWaldoComponent::HandleFrameEnd()
{
	OnFrameStart.Broadcast();
	
	for (const FInput& Input: RegisteredInputs)
	{
		const int Id = Input.Id;
		const FString& Label = Input.Label;
		
		for (const FInputValue& InputValue: FrameInputValues)
		{
			if (Id == InputValue.Id)
			{
				const int Value = InputValue.Value;

				for (FInputBound& Binding: InputBindings)
				{
					if (Binding.Label == Label)
					{
						if (!Binding.Delegate.ExecuteIfBound(Value))
						{
							UE_LOG(LogWaldo, Warning, TEXT("%hs - unbound delegate for [%s]"), __FUNCTION__, *Label);
						}
					}
				}
			}
		}
	}
	
	FrameInputValues.Reset();

	OnFrameEnd.Broadcast();
}
