#include "WaldoCommandByteStream.h"

#include "LogWaldo.h"
#include "SerialPort.h"

namespace
{
    constexpr int CommandHeaderSize = 2;                // [0] = Type, [1] = Size
}

UWaldoCommandByteStream::UWaldoCommandByteStream()
{
    Buffer.Reset(32);

    Guard = {'W','A','L','D','O'};
}

void UWaldoCommandByteStream::Setup(USerialPort* inSerialPort)
{
    SerialPort = inSerialPort;
}

void UWaldoCommandByteStream::Reset()
{
    Buffer.Reset();
}

bool UWaldoCommandByteStream::Receive(FWaldoCommand& OutCommand)
{
    if (!ensure(SerialPort))
    {
        return false;
    }
    
    const int Available = SerialPort->Available();

    for (int i=0; i<Available; i++)
    {
        ReadFromSerialPort();

        SkipToGuard();

        if (IsReadyToParseCommand())
        {
            ParseCommandAndConsumeBuffer(OutCommand);

            return true;
        }
    }

    return false;
}


void UWaldoCommandByteStream::Send(const FWaldoCommand& command)
{
    if (!ensure(SerialPort))
    {
        UE_LOG(LogWaldo, Warning, TEXT("%hs - SerialPort is not connected"), __FUNCTION__);
        return;
    }

    UE_LOG(LogWaldo, Log, TEXT("%hs - send command type [%c]"), __FUNCTION__, TCHAR(command.GetType()));
    
    const TArray<uint8_t>& Data = command.GetData();
    const int DataSize = Data.Num();
    ensure(DataSize < 256);

    
    const uint8_t type = static_cast<uint8_t>(command.GetType());

    int Used = 0;
    
    // Guard
    bool bSuccess = SerialPort->Write(Guard, Used);
    ensure(bSuccess);
    ensure(Used == Guard.Num());

    // 1 byte - type
    bSuccess = SerialPort->Write(type);
    ensure(bSuccess);

    // 1 byte - payload size
    bSuccess = SerialPort->Write(DataSize);
    ensure(bSuccess);

    // n bytes - payload
    bSuccess = SerialPort->Write(Data, Used);
    ensure(bSuccess);
    ensure(Used == DataSize);
}

void UWaldoCommandByteStream::ReadFromSerialPort()
{
    uint8 byte;
    ensure(SerialPort->Read(byte));

    Buffer.Add(byte);
}

void UWaldoCommandByteStream::SkipToGuard()
{
    // Skip bytes in the buffer if they don't begin with a valid guard.
    //
    // @note We could skip all data in the buffer if there's no guard at the beginning
    
    while (!Buffer.IsEmpty())
    {
        bool bHasValidGuard = true;
        
        const int GuardSize = FMath::Min(Guard.Num(), Buffer.Num());
        
        for (int i=0; i<GuardSize; i++)
        {
            if (Buffer[i] != Guard[i])
            {
                Buffer.RemoveAt(0);
                bHasValidGuard = false;
                break;
            }
        }

        if (bHasValidGuard)
        {
            return;
        }
    }
}

bool UWaldoCommandByteStream::IsReadyToParseCommand() const
{
    const int GuardSize = Guard.Num();
    const int BufferSize = Buffer.Num();
    
    if (BufferSize < (GuardSize + CommandHeaderSize)) {
        return false;
    }

    const int CommandPayloadSize = Buffer[GuardSize + 1];
    const int CommandSize = GuardSize + CommandHeaderSize + CommandPayloadSize;
    if (BufferSize < CommandSize) 
    {
        return false;
    }
    
    ensure(BufferSize == CommandSize);

    return true;
}

void UWaldoCommandByteStream::ParseCommandAndConsumeBuffer(FWaldoCommand& OutCommand)
{
    const int GuardSize = Guard.Num();
    
    const int Type = Buffer[GuardSize + 0];
    const int CommandPayloadSize = Buffer[GuardSize + 1];

    const int CommandSize = GuardSize + CommandHeaderSize + CommandPayloadSize;
    ensure(Buffer.Num() == CommandSize);

    OutCommand.Type = static_cast<EWaldoCommandType>(Type);
    OutCommand.Data.Reset();
    OutCommand.Data.Append(Buffer.GetData() + GuardSize + CommandHeaderSize, CommandPayloadSize);

    Buffer.Reset();
}
