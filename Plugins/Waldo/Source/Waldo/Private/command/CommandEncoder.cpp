#include "CommandEncoder.h"

FCommandEncoder::FCommandEncoder(FCommand& inCommand) : Command(inCommand)
{

}

FCommand& FCommandEncoder::Reset()
{
    Command.Type = ECommandType::Reset;
    Command.Data.Reset();

    return Command;
}

FCommand& FCommandEncoder::Message(const FString& message)
{
    Command.Type = ECommandType::Message;

    const int Length = message.Len();
    Command.Data.Reset(Length);
    for (int i = 0; i < Length; ++i)
    {
        Command.Data.Add(static_cast<char>(message[i]));
    }

    return Command;
}

FCommand& FCommandEncoder::AcknowledgeReset()
{
    Command.Type = ECommandType::AcknowledgeReset;
    Command.Data.Reset();

    return Command;
}

FCommand& FCommandEncoder::AcknowledgeFrame()
{
    Command.Type = ECommandType::AcknowledgeFrame;
    Command.Data.Reset();

    return Command;
}
