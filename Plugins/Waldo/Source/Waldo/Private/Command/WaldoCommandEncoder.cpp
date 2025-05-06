#include "WaldoCommandEncoder.h"

FWaldoCommandEncoder::FWaldoCommandEncoder(FWaldoCommand& inCommand) : Command(inCommand)
{

}

FWaldoCommand& FWaldoCommandEncoder::EncodeReset()
{
    Command.Type = EWaldoCommandType::Reset;
    Command.Data.Reset();

    return Command;
}

FWaldoCommand& FWaldoCommandEncoder::EncodeMessage(const FString& message)
{
    Command.Type = EWaldoCommandType::Message;

    const int Length = message.Len();
    Command.Data.Reset(Length);
    for (int i = 0; i < Length; ++i)
    {
        Command.Data.Add(static_cast<char>(message[i]));
    }

    return Command;
}

FWaldoCommand& FWaldoCommandEncoder::EncodeAcknowledgeReset()
{
    Command.Type = EWaldoCommandType::AcknowledgeReset;
    Command.Data.Reset();

    return Command;
}

FWaldoCommand& FWaldoCommandEncoder::EncodeAcknowledgeFrame()
{
    Command.Type = EWaldoCommandType::AcknowledgeFrame;
    Command.Data.Reset();

    return Command;
}
