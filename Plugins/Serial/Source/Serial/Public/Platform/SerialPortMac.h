#pragma once

#if PLATFORM_MAC

#include "Data/SerialPortDevice.h"

/**
 * Mac-specific implementation of Serial Port
 */
struct FSerialPortMac
{
	static TArray<FSerialPortDevice> EnumerateSerialPortDevices();
	
	FString ToString() const;
	bool Open(const FString& Device, int BaudRate);
	bool IsOpen() const;
	int Available() const;
	bool Read(TArray<uint8>& OutBuffer, int& OutBufferUsed);
	bool Write(const TArray<uint8>& Buffer, int& OutBufferUsed);
	bool Close();

private:
	int fd = -1;
};

#endif
