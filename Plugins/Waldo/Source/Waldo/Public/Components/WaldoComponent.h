#pragma once

#include "SerialPort.h"
#include "Input/WaldoInputBound.h"
#include "Input/WaldoInputValue.h"
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
	void BindDelegateForInput(const FString& Label, FDelegateWaldoInputValue Delegate);

	UFUNCTION(BlueprintCallable)
	void UnbindDelegateForInput(const FString& Label, FDelegateWaldoInputValue Delegate);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateFrameEnd);

	UPROPERTY(BlueprintAssignable)
	FDelegateFrameEnd OnFrameEnd;

protected:
	// >> UActorComponent
	virtual void BeginPlay() override;
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
	void HandleRegisterInput(const FWaldoInput& Input);

	UFUNCTION()
	void HandleFrameStart();

	UFUNCTION()
	void HandleInputValue(const FWaldoInputValue& InputValue);

	UFUNCTION()
	void HandleFrameEnd();
	
	UPROPERTY()
	TObjectPtr<USerialPort> SerialPort;

	UPROPERTY()
	TObjectPtr<UWaldoHostStateMachine> StateMachine;

	UPROPERTY()
	TArray<FWaldoInput> RegisteredInputs;

	UPROPERTY()
	TArray<FWaldoInputValue> FrameInputValues;

	UPROPERTY()
	TArray<FWaldoInputBound> InputBindings;
};