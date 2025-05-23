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


bool UWaldoCommandByteStream::Send(const FWaldoCommand& Command) const
{
    if (!ensure(SerialPort))
    {
        UE_LOG(LogWaldo, Warning, TEXT("%hs - SerialPort is not connected"), __FUNCTION__);
        return false;
    }

    UE_LOG(LogWaldo, Log, TEXT("%hs - send command type [%c]"), __FUNCTION__, TCHAR(Command.GetType()));
    
    const TArray<uint8_t>& Data = Command.GetData();
    const int DataSize = Data.Num();
    if (!ensure(DataSize < 256))
    {
        UE_LOG(LogWaldo, Error, TEXT("%hs - DataSize [%d] >= 256"), __FUNCTION__, DataSize);
        return false;
    }
    
    const uint8_t Type = static_cast<uint8_t>(Command.GetType());
    
    TArray<uint8_t, TFixedAllocator<256>> WriteBuffer;
    WriteBuffer.Append(Guard);
    WriteBuffer.Add(Type);
    WriteBuffer.Add(DataSize);
    WriteBuffer.Append(Data);
    
    int Used = 0;
    const bool bSuccess = SerialPort->Write(WriteBuffer, Used);
    if (!bSuccess)
    {
        UE_LOG(LogWaldo, Warning, TEXT("%hs - SerialPort::Write failed"), __FUNCTION__);
        return false;
    }
    
    const bool bAllBytesWritten = (Used == WriteBuffer.Num());
    if (!bAllBytesWritten)
    {
        UE_LOG(LogWaldo, Warning, TEXT("%hs - SerialPort::Write only wrote [%d] of [%d] bytes"), __FUNCTION__, Used, WriteBuffer.Num());
        return false;
    }

    return true;
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
    
    if (BufferSize < (GuardSize + CommandHeaderSize))
    {
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
