#pragma once

#include "CoreMinimal.h"
#include "Data/SerialPortDevice.h"

#if PLATFORM_WINDOWS
# include "Platform/SerialPortWindows.h"
#elif PLATFORM_MAC
# include "Platform/SerialPortMac.h"
#endif

#include "SerialPort.generated.h"

#if PLATFORM_WINDOWS
typedef FSerialPortWindows FSerialPortPlatform;
#elif PLATFORM_MAC
typedef FSerialPortMac FSerialPortPlatform;
#endif

UCLASS()
class SERIAL_API USerialPort : public UObject
{
	GENERATED_BODY()

public:
	USerialPort();
	virtual ~USerialPort();
	
	/**
	 * Open a serial port to a device
	 * 
	 * @param Device on Mac this is the '/dev/...' name for the port, on Windows this is the 'COMx' name for the port 
	 * @param BaudRate Communication speed. This should match speed at which serial port is opened on your device.
	 *
	 * @note Currently assumes 8 bit, no parity, 1 stop bit, no flow control
	 * 
	 * @return true, if the port is opened successfully
	 */
	bool Open(const FSerialPortDevice& Device, int BaudRate);

	/**
	 * Report whether the serial port is currently open
	 * 
	 * @return true, if the port is currently open
	 */
	bool IsOpen() const;

	/**
	 * Find out how much data is available to read from the serial port
	 * 
	 * @return the number of bytes that are currently available to read
	 */
	int Available() const;
	
	/**
	 * Perform a non-blocking read.
	 *
	 * Note: The number of bytes returned could be smaller than the size of the output buffer
	 * 
	 * @param OutBuffer Pre-Allocated buffer
	 * @param OutBufferUsed Number of bytes used in the buffer
	 * @return true, if successful
	 */
	bool Read(TArray<uint8>& OutBuffer, int& OutBufferUsed);

	/**
	 * Perform a non-blocking read
	 * 
	 * @param OutByte The byte that was read from the serial port
	 * @return true, if a byte was read successfully
	 */
	bool Read(uint8& OutByte);

	/**
	 * Perform a non-blocking write.
	 *
	 * Note: The number of bytes returned could be smaller than the size of the buffer
	 * @param Buffer Data to write to serial port
	 * @param OutBufferUsed The amount of data from the buffer that was written
	 * @return true, if any data was written to the port - check OutBufferUsed for the actual number of bytes written
	 */
	bool Write(const TArray<uint8>& Buffer, int& OutBufferUsed);

	/**
	 * Perform a non-blocking write of one byte
	 * 
	 * @param Byte The byte to write to the serial port
	 * @return true, if successful
	 */
	bool Write(uint8 Byte);

	/**
	 * Close serial port
	 * 
	 * @return true, if successfully opened 
	 */
	bool Close();
	
private:
	FSerialPortPlatform Impl;
};
