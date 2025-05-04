#pragma once

#include "Command.h"
#include "CommandByteStream.generated.h"

class USerialPort;
/**
 * @class UCommandByteStream
 * @brief Parse input from serial connection to generate commands, and send responses
 * 
 */
UCLASS()
class UCommandByteStream : public UObject
{
public:
    GENERATED_BODY()
    
    UCommandByteStream();

    void Setup(USerialPort* inSerialPort);

    void Reset();

    bool Receive(FCommand& OutCommand);
    
    void Send(const FCommand& Command);

private:
    void ReadFromSerialPort();

    void SkipToGuard();
    
    // is there enough data in the buffer to parse the next command
    bool IsReadyToParseCommand() const;

    // parse buffer into OutCommand and empty the buffer
    void ParseCommandAndConsumeBuffer(FCommand& OutCommand);
    
    TArray<uint8> Buffer;

    TArray<uint8> Guard;

    UPROPERTY()
    TObjectPtr<USerialPort> SerialPort;
};
