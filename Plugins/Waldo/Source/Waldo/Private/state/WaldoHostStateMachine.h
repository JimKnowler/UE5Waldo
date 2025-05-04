#pragma once

#include "CoreMinimal.h"
#include "WaldoHostState.h"
#include "SerialPort.h"
#include "command/Command.h"
#include "command/CommandByteStream.h"
#include "input/Input.h"
#include "WaldoHostStateMachine.generated.h"

UCLASS()
class UWaldoHostStateMachine : public UObject
{
	GENERATED_BODY()

public:
	UWaldoHostStateMachine();
	
	void Reset(USerialPort* SerialPort);

	void Tick(float DeltaTime);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateReset);

	UPROPERTY(BlueprintAssignable)
	FDelegateReset OnReset;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateMessage, const FString&, Message);

	UPROPERTY(BlueprintAssignable)
	FDelegateMessage OnMessage;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateRegisterInput, const FInput&, Input);

	UPROPERTY(BlueprintAssignable)
	FDelegateRegisterInput OnRegisterInput;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameStart);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameStart OnFrameStart;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateInputValue, const FInputValue&, InputValue);

	UPROPERTY(BlueprintAssignable)
	FDelegateInputValue OnInputValue;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameEnd);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameEnd OnFrameEnd;

	UPROPERTY()
	float MaxTimeBetweenCommands = 0.5f;
	
private:
	bool Process(const FCommand& Command);

	float GetTime() const;
	
	UPROPERTY()
	TObjectPtr<USerialPort> SerialPort;

	UPROPERTY()
	TObjectPtr<UCommandByteStream> CommandByteStream;

	EWaldoHostState State = EWaldoHostState::None;
	
	float LastCommandTime = 0.0f;
};

