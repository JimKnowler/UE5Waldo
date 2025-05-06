#pragma once

#include "CoreMinimal.h"
#include "WaldoHostState.h"
#include "SerialPort.h"
#include "Command/WaldoCommand.h"
#include "Command/WaldoCommandByteStream.h"
#include "Input/WaldoInput.h"
#include "WaldoHostStateMachine.generated.h"

UCLASS()
class UWaldoHostStateMachine : public UObject
{
	GENERATED_BODY()

public:
	UWaldoHostStateMachine();

	// Reset the StateMachine to initial conditions + use the supplied SerialPort
	void Reset(USerialPort* SerialPort);

	/**
	 * Tick the state machine every frame
	 * 
	 * @param DeltaTime Time elapsed since the last frame 
	 *
	 * @return false, if the connection to the client has failed
	 */
	bool Tick(float DeltaTime);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateReset);

	UPROPERTY(BlueprintAssignable)
	FDelegateReset OnReset;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateMessage, const FString&, Message);

	UPROPERTY(BlueprintAssignable)
	FDelegateMessage OnMessage;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateRegisterInput, const FWaldoInput&, Input);

	UPROPERTY(BlueprintAssignable)
	FDelegateRegisterInput OnRegisterInput;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameStart);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameStart OnFrameStart;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateInputValue, const FWaldoInputValue&, InputValue);

	UPROPERTY(BlueprintAssignable)
	FDelegateInputValue OnInputValue;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameEnd);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameEnd OnFrameEnd;

	UPROPERTY()
	float MaxTimeBetweenCommands = 0.5f;
	
private:
	bool Process(const FWaldoCommand& Command, bool& OutHasErrored);

	float GetTime() const;
	
	UPROPERTY()
	TObjectPtr<USerialPort> SerialPort;

	UPROPERTY()
	TObjectPtr<UWaldoCommandByteStream> CommandByteStream;

	EWaldoHostState State = EWaldoHostState::None;
	
	float LastCommandTime = 0.0f;
};

