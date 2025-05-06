#pragma once

#include "WaldoCommand.h"
#include "WaldoCommandByteStream.generated.h"

class USerialPort;
/**
 * @class UWaldoCommandByteStream
 * @brief Parse input from serial connection to generate commands, and send responses
 * 
 */
UCLASS()
class UWaldoCommandByteStream : public UObject
{
public:
    GENERATED_BODY()
    
    UWaldoCommandByteStream();

    void Setup(USerialPort* inSerialPort);

    void Reset();

    bool Receive(FWaldoCommand& OutCommand);
    
    bool Send(const FWaldoCommand& Command) const;

private:
    void ReadFromSerialPort();

    void SkipToGuard();
    
    // is there enough data in the buffer to parse the next command
    bool IsReadyToParseCommand() const;

    // parse buffer into OutCommand and empty the buffer
    void ParseCommandAndConsumeBuffer(FWaldoCommand& OutCommand);
    
    TArray<uint8> Buffer;

    TArray<uint8> Guard;

    UPROPERTY()
    TObjectPtr<USerialPort> SerialPort;
};
