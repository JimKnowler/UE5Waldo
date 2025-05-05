#include "SerialSubsystem.h"
#include "SerialPort.h"

TArray<FSerialPortDevice> USerialSubsystem::EnumerateSerialPortDevices()
{
	return SerialPortDevices;
}

void USerialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	SerialPortDevices = FSerialPortPlatform::EnumerateSerialPortDevices();

	StartTimer();
}

void USerialSubsystem::Deinitialize()
{
	Super::Deinitialize();

	StopTimer();
}

void USerialSubsystem::HandleTimer()
{
	TArray<FSerialPortDevice> NewSerialPortDevices = FSerialPortPlatform::EnumerateSerialPortDevices();

	TArray<FSerialPortDevice> AddedDevices;
	TArray<FSerialPortDevice> RemovedDevices;
	
	for (const FSerialPortDevice& NewDevice: NewSerialPortDevices)
	{
		if (INDEX_NONE == SerialPortDevices.Find(NewDevice))
		{
			AddedDevices.Add(NewDevice);
		}
	}

	for (const FSerialPortDevice& Device: SerialPortDevices)
	{
		if (INDEX_NONE == NewSerialPortDevices.Find(Device))
		{
			RemovedDevices.Add(Device);
		}
	}

	if (AddedDevices.IsEmpty() && RemovedDevices.IsEmpty())
	{
		return;
	}

	SerialPortDevices = NewSerialPortDevices;

	for (const FSerialPortDevice& AddedDevice: AddedDevices)
	{
		OnSerialPortDeviceAdded.Broadcast(AddedDevice);
	}

	for (const FSerialPortDevice& RemovedDevice: RemovedDevices)
	{
		OnSerialPortDeviceRemoved.Broadcast(RemovedDevice);
	}
}

void USerialSubsystem::StartTimer()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (ensure(IsValid(GameInstance)))
	{
		GameInstance->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&ThisClass::HandleTimer,
			PollInterval,
			true );
	}
}

void USerialSubsystem::StopTimer()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (ensure(IsValid(GameInstance)))
	{
		GameInstance->GetTimerManager().ClearTimer(TimerHandle);
	}
}
