﻿#pragma once

#if PLATFORM_WINDOWS

#include <windows.h>

#include "Data/SerialPortDevice.h"

/**
 * Windows-specific implementation of Serial Port
 */
struct FSerialPortWindows
{
	static TArray<FSerialPortDevice> EnumerateSerialPortDevices();
	
	FString ToString() const;
	bool Open(const FString& Device, int BaudRate);
	bool IsOpen() const;
	int Available() const;
	bool Read(TArray<uint8>& OutBuffer, int& OutBufferUsed);
	bool Write(const uint8* WriteBuffer, int WriteBufferSize, int& OutBufferUsed);
	bool Close();

private:
	HANDLE Port = INVALID_HANDLE_VALUE;
};

#endif
