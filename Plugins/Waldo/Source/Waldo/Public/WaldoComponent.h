#pragma once

#include "SerialPort.h"
#include "input/InputBound.h"
#include "input/InputValue.h"
#include "WaldoComponent.generated.h"

class UWaldoHostStateMachine;

UCLASS(Abstract, Blueprintable)
class WALDO_API UWaldoComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UWaldoComponent();

	UFUNCTION(BlueprintCallable)
	bool Connect(const FSerialPortDevice& Device, int BaudRate = 9600);

	UFUNCTION(BlueprintCallable)
	bool IsConnected() const;

	UFUNCTION(BlueprintCallable)
	bool Disconnect();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateReset);

	UPROPERTY(BlueprintAssignable)
	FDelegateReset OnReset;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateMessage, const FString&, Message);

	UPROPERTY(BlueprintAssignable)
	FDelegateMessage OnMessage;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameStart);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameStart OnFrameStart;

	UFUNCTION(BlueprintCallable)
	void BindDelegateForInput(const FString& Label, FDelegateInputValue Delegate);

	UFUNCTION(BlueprintCallable)
	void UnbindDelegateForInput(const FString& Label, FDelegateInputValue Delegate);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameEnd);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameEnd OnFrameEnd;

protected:
	// >> UActorComponent
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// << UActorComponent

private:
	void RegisterCallbacks();
	void UnregisterCallbacks();

	UFUNCTION()
	void HandleReset();

	UFUNCTION()
	void HandleMessage(const FString& Message);
	
	UFUNCTION()
	void HandleRegisterInput(const FInput& Input);

	UFUNCTION()
	void HandleFrameStart();

	UFUNCTION()
	void HandleInputValue(const FInputValue& InputValue);

	UFUNCTION()
	void HandleFrameEnd();
	
	UPROPERTY()
	TObjectPtr<USerialPort> SerialPort;

	UPROPERTY()
	TObjectPtr<UWaldoHostStateMachine> StateMachine;

	UPROPERTY()
	TArray<FInput> RegisteredInputs;

	UPROPERTY()
	TArray<FInputValue> FrameInputValues;

	UPROPERTY()
	TArray<FInputBound> InputBindings;
};