#include "WaldoCommand.h"

EWaldoCommandType FWaldoCommand::GetType() const 
{
    return Type;
}

const TArray<uint8>& FWaldoCommand::GetData() const
{
    return Data;
}
