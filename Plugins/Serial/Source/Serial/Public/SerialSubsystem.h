// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SerialPortDevice.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SerialSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SERIAL_API USerialSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateSerialPortDeviceAdded, FSerialPortDevice, SerialPortDevice);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateSerialPortDeviceRemoved, FSerialPortDevice, SerialPortDevice);

	/**
	 * How often the connected serial ports should be polled
	 */
	UPROPERTY(EditAnywhere)
	float PollInterval = 1.0f;
	
	/**
	 * Generate an array of all connected serial port devices
	 * 
	 * @return Array of device for connected serial ports
	 */
	UFUNCTION(BlueprintCallable)
	TArray<FSerialPortDevice> EnumerateSerialPortDevices();

	/**
	 * Event broadcast whenever a serial port device is detected
	 */
	UPROPERTY(BlueprintAssignable)
	FDelegateSerialPortDeviceAdded OnSerialPortDeviceAdded;

	/**
	 * Event broadcast whenever a serial port device is removed
	 */
	UPROPERTY(BlueprintAssignable)
	FDelegateSerialPortDeviceRemoved OnSerialPortDeviceRemoved;

protected:
	// >> USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// << USubsystem

private:
	UPROPERTY()
	TArray<FSerialPortDevice> SerialPortDevices;
	
	UFUNCTION()
	void HandleTimer();
	
	void StartTimer();
	void StopTimer();
	
	UPROPERTY()
	FTimerHandle TimerHandle;
};
