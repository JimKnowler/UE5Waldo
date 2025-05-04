#if PLATFORM_MAC

#include "Platform/SerialPortMac.h"

#include <sys/ioctl.h>
#include <sys/termios.h>

#define KERNEL 0
#include <serial/IOSerialKeys.h>
#undef KERNEL

#include <IOKitLib.h>
#include <CFDictionary.h>
#include <IOTypes.h>


#include <CoreFoundation.h>

#include "LogSerial.h"

namespace
{
	speed_t ConvertBaudRateToSpeed(int baudRate)
	{
		switch (baudRate)                               // Set the speed (baudRate)
		{
		case 110:
			return B110;
		case 300:
			return B300;
		case 600:
			return B600;
		case 1200:
			return B1200;
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		case 57600:
			return B57600;
		case 115200:
			return B115200;
		default :
			printf("Unsupported baud rate - default to 9600\n");
			return B9600;
		}
	}
}

TArray<FSerialPortDevice> FSerialPortMac::EnumerateSerialPortDevices()
{
	TArray<FSerialPortDevice> Devices;
	
	// Get master port
	mach_port_t masterPort;
	if (IOMainPort(MACH_PORT_NULL, &masterPort) != KERN_SUCCESS)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get IOKit master port"));
		return {};
	}
    
	// Match serial devices
	CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOSerialBSDServiceValue);
	if (matchingDict == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create matching dictionary"));
		return {};
	}
    
	// Get iterator
	io_iterator_t iterator;
	if (IOServiceGetMatchingServices(masterPort, matchingDict, &iterator) != KERN_SUCCESS)
	{
		UE_LOG(LogTemp, Error, TEXT("Error getting matching services"));
		return {};
	}
    
	// Iterate through serial devices
	io_object_t SerialDevice;
	while ((SerialDevice = IOIteratorNext(iterator)))
	{
		auto Lambda = [SerialDevice](CFStringRef Key) -> FString
		{
			FString Result;
			
			CFTypeRef DevicePath = IORegistryEntryCreateCFProperty(SerialDevice, Key, kCFAllocatorDefault, 0);
			if (DevicePath)
			{
				char Path[PATH_MAX];
				if (CFStringGetCString((CFStringRef)DevicePath, Path, PATH_MAX, kCFStringEncodingUTF8))
				{
					Result = FString(Path);
				}

				CFRelease(DevicePath);
			}

			return Result;
		};

		FSerialPortDevice Device {
			.Name = Lambda(CFSTR(kIOCalloutDeviceKey)),
			.Description = Lambda(CFSTR("USB Product Name"))
		};
				
		Devices.Add(Device);
		
		IOObjectRelease(SerialDevice);
	}
    
	IOObjectRelease(iterator);
    
	return Devices;
}

FString FSerialPortMac::ToString() const
{
	FString Result = FString::Printf(TEXT("%d"), fd);
	return Result;
}

bool FSerialPortMac::Open(const FString& Device, int BaudRate)
{
	// Open port (note: this will fail if arduino serial console is open)
	fd = ::open(TCHAR_TO_ANSI(*Device), O_RDWR | O_NOCTTY | O_NDELAY);

	// verify file descriptor 
	if (fd == -1)  
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - Device cannot be opened [%d]"), __FUNCTION__, fd);
		return false;
	}

	int Result;
	
#if 1
	// based on example : https://www.pololu.com/docs/0J73/15.5
	
	// Flush away any bytes previously read or written
	Result = tcflush(fd, TCIOFLUSH);
	if (Result != 0)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - Failed to tcflush [%d]"), __FUNCTION__, fd);
		Close();
		return false;
	}
#endif
	
#if 1
	Result = fcntl(fd, F_SETFL, FNDELAY);                    // Open the device in nonblocking mode
	if (Result != 0)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - Failed to set non-blocking mode [%d]"), __FUNCTION__, fd);
		Close();
		return false;
	}
#endif
	
	termios options;
	
	// Set parameters
	Result = tcgetattr(fd, &options);                        // Get the current options of the port
	if (Result != 0)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - Failed to tcgetattr [%d]"), __FUNCTION__, fd);
		Close();
		return false;
	}

#if 1
	bzero(&options, sizeof(options));               // Clear all the options

	options.c_cflag |= ( CLOCAL | CREAD | CS8);    // Configure the device : 8 bits, no parity, no control
	options.c_iflag |= ( IGNPAR | IGNBRK );			// Ignore Parity errors, Ignore 'Break'
	options.c_cc[VTIME]=0;                          // Timer unused
	options.c_cc[VMIN]=0;                           // At least on character before satisfy reading
#else
	// based on example : https://www.pololu.com/docs/0J73/15.5
	
	// Turn off any options that might interfere with our ability to send and
	// receive raw binary bytes.
	options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
	options.c_oflag &= ~(ONLCR | OCRNL);
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
 
	// Set up timeouts: Calls to read() will return as soon as there is
	// at least one byte available or when 100 ms has passed.
	options.c_cc[VTIME] = 1;
	options.c_cc[VMIN] = 0;
#endif

	speed_t Speed = ConvertBaudRateToSpeed(BaudRate);
	::cfsetispeed(&options, Speed);                 // Set the baud rate
	::cfsetospeed(&options, Speed);
	
	Result = ::tcsetattr(fd, TCSANOW, &options);             // Activate the settings immediately
	if (Result != 0)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - Failed to tcsetattr [%d]"), __FUNCTION__, fd);
		Close();
		return false;
	}

	return true;
}

bool FSerialPortMac::IsOpen() const
{
	bool bIsOpen = (fd != -1);

	return bIsOpen;
}

int FSerialPortMac::Available() const
{
	size_t ReadSize = 0;
	
	::ioctl(fd, FIONREAD, &ReadSize);

	return ReadSize;
}

bool FSerialPortMac::Read(TArray<uint8>& OutBuffer, int& OutBufferUsed)
{
	size_t ReadSize = Available();
	
	if (ReadSize > 0)
	{
		// safety - reduce max read size
		const size_t BufferSize = OutBuffer.Num();
		ReadSize = FMath::Min(ReadSize, BufferSize);
		
		// If the number of bytes read is equal to the number of bytes retrieved
		uint8* Data = OutBuffer.GetData();
		const ssize_t Result = ::read(fd, Data, ReadSize);
		if (Result != ReadSize)
		{
			UE_LOG(LogSerial, Error, TEXT("%hs - failed to read - result [%lu] expected [%d]"), __FUNCTION__, Result, ReadSize);
			return false;
		}
	}

	OutBufferUsed = ReadSize;

	return true;
}

bool FSerialPortMac::Write(const TArray<uint8>& Buffer, int& OutBufferUsed)
{
	const uint8* Data = Buffer.GetData();
	const size_t NumBytes = Buffer.Num();
	
	::ssize_t NumBytesWritten = ::write(fd, Data, NumBytes);
	if (NumBytesWritten > NumBytes)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - failed to write [%lu]"), __FUNCTION__, NumBytesWritten);
		return false;
	}

	OutBufferUsed = NumBytesWritten;
	return true;
}

bool FSerialPortMac::Close()
{
	int Result = ::close(fd);
	if (Result != 0)
	{
		UE_LOG(LogSerial, Error, TEXT("%hs - failed to close port [%d]"), __FUNCTION__, Result);
		return false;
	}

	fd = -1;

	return true;
}

#endif
