#include "Command.h"

ECommandType FCommand::GetType() const 
{
    return Type;
}

const TArray<uint8>& FCommand::GetData() const
{
    return Data;
}
