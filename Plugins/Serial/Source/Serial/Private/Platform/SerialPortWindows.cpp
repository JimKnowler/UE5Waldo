#if PLATFORM_WINDOWS

#include "Platform/SerialPortWindows.h"

#include "LogSerial.h"

#include <SetupAPI.h>
#include <devguid.h>

namespace
{
    FString GetLastErrorAsString()
    {
        DWORD error_code = ::GetLastError();
    	
        char buffer[256];
        DWORD size = ::FormatMessageA(
          FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
          nullptr, error_code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
          buffer, sizeof(buffer), nullptr);

    	if (size == 0)
        {
	        return FString::Printf(TEXT("%u:Unknown error code"), error_code);
        }

    	FString Result = FString::Printf(TEXT("%u:%s"), error_code, ANSI_TO_TCHAR(buffer) );
    	
    	return Result;    	
    }
}

#include <iostream>

TArray<FSerialPortDevice> FSerialPortWindows::EnumerateSerialPortDevices()
{	
	// Get the device information set for all present devices
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - SetupDiGetClassDevs failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		
		return {};
	}

	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	TArray<FSerialPortDevice> Result;

	// Enumerate devices
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		FSerialPortDevice Device;

		// Get the description
		char Description[256];
		DWORD DescriptionSize = 0;
        if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, nullptr, reinterpret_cast<PBYTE>(Description), sizeof(Description), &DescriptionSize))
        {
        	Device.Description = ANSI_TO_TCHAR(Description);
        }
		
		// Get the registry key
		HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
		if (hKey != INVALID_HANDLE_VALUE)
		{
			// Read the port name
			char PortName[256];
			DWORD Size = sizeof(PortName);
			DWORD Type = REG_SZ;
			if (RegQueryValueExA(hKey, "PortName", nullptr, &Type, reinterpret_cast<LPBYTE>(PortName), &Size) == ERROR_SUCCESS)
			{
				Device.Name = ANSI_TO_TCHAR(PortName);
			}
			
			RegCloseKey(hKey);
		}
		
		Result.Add(Device);
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return Result;
}

FString FSerialPortWindows::ToString() const
{
	return FString::Printf(TEXT("%p"), Port);
}

bool FSerialPortWindows::Open(const FString& Device, int BaudRate)
{
	const FString FullPathToDevice = TEXT("\\\\.\\") + Device;

	// do not share read/write/delete with any other process
	constexpr int ShareMode = 0;
	
	Port = ::CreateFileA(TCHAR_TO_ANSI(*FullPathToDevice), GENERIC_READ | GENERIC_WRITE, ShareMode, nullptr,
	OPEN_EXISTING, 0, nullptr);

	if (Port == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - CreateFileA failed [%s]"), __FUNCTION__, *GetLastErrorAsString());

		return false;
	}
 
	// Flush away any bytes previously read or written.
	if (!::FlushFileBuffers(Port))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - FlushFileBuffers failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		Close();

		return false;
	}
	
	// Set the baud rate and other options.
	DCB State = {0};
	::SecureZeroMemory(&State, sizeof(DCB));
	State.DCBlength = sizeof(DCB);
	
	if (!::GetCommState(Port, &State))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - GetCommState failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		Close();

		return false;
	}

	// Set Baud / Parity
	State.BaudRate = BaudRate;
	State.ByteSize = 8;
	State.Parity = NOPARITY;
	State.StopBits = ONESTOPBIT;

	// Disable Flow Control
	State.fInX = false;
	State.fOutX = false;
	State.fOutxCtsFlow = false;
	State.fRtsControl = RTS_CONTROL_DISABLE;
	State.fDtrControl = DTR_CONTROL_DISABLE;
	State.fOutxDsrFlow = false;
	State.fBinary = true;
		
	if (!::SetCommState(Port, &State))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - SetCommState failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		Close();

		return false;
	}

	// Configure read and write operations to time out immediately
	COMMTIMEOUTS Timeouts = {0};
	Timeouts.ReadIntervalTimeout = 0;
	Timeouts.WriteTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutConstant = 0;
	Timeouts.WriteTotalTimeoutConstant = 0;
	
	if (!::SetCommTimeouts(Port, &Timeouts))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - SetCommTimeouts failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		Close();

		return false;
	}

	::PurgeComm(Port, PURGE_RXCLEAR);
	::PurgeComm(Port, PURGE_TXCLEAR);
 
	return true;
}

bool FSerialPortWindows::IsOpen() const
{
	bool bIsOpen = (Port != INVALID_HANDLE_VALUE);

	return bIsOpen;
}

int FSerialPortWindows::Available() const
{
	COMSTAT comStat;
	DWORD errors;
	if (!::ClearCommError(Port, &errors, &comStat))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - ClearCommError failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		return 0;
	}

	DWORD Available = comStat.cbInQue;

	return Available;
}

bool FSerialPortWindows::Read(TArray<uint8>& OutBuffer, int& OutBufferUsed)
{
	COMSTAT comStat;
	DWORD errors;
	if (!::ClearCommError(Port, &errors, &comStat))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - ClearCommError failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		return false;
	}

	DWORD Available = comStat.cbInQue;
	
	uint8* Buffer = OutBuffer.GetData();
	const DWORD BufferSize = OutBuffer.Num();
	const DWORD ReadSize = FMath::Min(BufferSize, Available);
	
	DWORD Received = 0;
	if (!::ReadFile(Port, Buffer, ReadSize, &Received, nullptr))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - ReadFile failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		return false;
	}
	
	OutBufferUsed = Received;

	return true;
}

bool FSerialPortWindows::Write(const TArray<uint8>& Buffer, int& OutBufferUsed)
{
	const uint8* WriteBuffer = Buffer.GetData();
	const DWORD WriteBufferSize = Buffer.Num();
	
	DWORD Written = 0;
	if (!::WriteFile(Port, WriteBuffer, WriteBufferSize, &Written, nullptr))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - WriteFile failed [%s]"), __FUNCTION__, *GetLastErrorAsString());
		return false;
	}

	OutBufferUsed = Written;

	return true;
}

bool FSerialPortWindows::Close()
{
	if (!::CloseHandle(Port))
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - CloseHandle failed [%s]"), __FUNCTION__, *GetLastErrorAsString());

		return false;
	}

	Port = INVALID_HANDLE_VALUE;

	return true;
}

#endif
