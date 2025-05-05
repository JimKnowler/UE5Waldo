#include "SerialPort.h"

#include "LogSerial.h"

USerialPort::USerialPort()
{
}

USerialPort::~USerialPort()
{
	if (IsOpen())
	{
		Close();
	}
}

bool USerialPort::Open(const FSerialPortDevice& Device, int BaudRate)
{
	if (IsOpen())
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - Port is already opened [%s]"), __FUNCTION__, *Impl.ToString());
		return false;
	}

	const FString& DeviceName = Device.Name;

	UE_LOG(LogSerial, Log, TEXT("%hs - opening connecting to device [%s] with BaudRate [%d]"), __FUNCTION__, *DeviceName, BaudRate);

	const bool Result = Impl.Open(DeviceName, BaudRate);

	if (Result)
	{
		UE_LOG(LogSerial, Log, TEXT("%hs - successfully opened connection to device [%s]"), __FUNCTION__, *DeviceName);
	} else {
		UE_LOG(LogSerial, Warning, TEXT("%hs - failed to open connection to device [%s]"), __FUNCTION__, *DeviceName);
	}
	
	return Result;
}

bool USerialPort::IsOpen() const
{
	return Impl.IsOpen();
}

int USerialPort::Available() const
{
	return Impl.Available();
}

bool USerialPort::Read(TArray<uint8>& OutBuffer, int& OutBufferUsed)
{
	if (!IsOpen())
	{
		UE_LOG(LogSerial, Warning, TEXT("%hs - unable to read from closed port"), __FUNCTION__);
		return false;
	}

	return Impl.Read(OutBuffer, OutBufferUsed);
}

bool USerialPort::Read(uint8& OutByte)
{
	if (!IsOpen())
	{
		UE_LOG(LogSerial, Warning, TEXT("%hs - unable to read from closed port"), __FUNCTION__);
		return false;
	}

	TArray<uint8> Buffer;
	Buffer.AddUninitialized(1);
	int Used = 0;
	const bool bSuccess = Impl.Read(Buffer, Used);

	if (!bSuccess || (Used != 1))
	{
		return false;
	}

	OutByte = Buffer[0];

	return true;
}

bool USerialPort::Write(const TArray<uint8>& Buffer, int& OutBufferUsed)
{
	if (!IsOpen())
	{
		UE_LOG(LogSerial, Warning, TEXT("%hs - unable to write to closed port"), __FUNCTION__);
		return false;
	}

	return Impl.Write(Buffer.GetData(), Buffer.Num(), OutBufferUsed);
}

bool USerialPort::Write(const TArray<uint8, TFixedAllocator<256>>& Buffer, int& OutBufferUsed)
{
	if (!IsOpen())
	{
		UE_LOG(LogSerial, Warning, TEXT("%hs - unable to write to closed port"), __FUNCTION__);
		return false;
	}

	return Impl.Write(Buffer.GetData(), Buffer.Num(), OutBufferUsed);
}

bool USerialPort::Write(uint8 Byte)
{
	if (!IsOpen())
	{
		UE_LOG(LogSerial, Warning, TEXT("%hs - unable to write to closed port"), __FUNCTION__);
		return false;
	}

	TArray<uint8> Buffer;
	Buffer.Add(Byte);

	int Used = 0;
	bool bSuccess = Write(Buffer, Used);

	return bSuccess && (Used == 1);
}	

bool USerialPort::Close()
{
	if (!IsOpen())
	{
		UE_LOG(LogSerial, Warning, TEXT("%hs - unable to close an already closed port"), __FUNCTION__);
		return false;
	}

	return Impl.Close();
}
