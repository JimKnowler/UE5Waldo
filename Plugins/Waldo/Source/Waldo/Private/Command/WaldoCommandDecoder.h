#pragma once

#include "WaldoCommand.h"
#include "Input/WaldoInput.h"
#include "Input/WaldoInputValue.h"

class FWaldoCommandDecoder
{
public:
    FWaldoCommandDecoder(const FWaldoCommand& command);

    bool DecodeRegisterInput(FWaldoInput& outInput) const;

    bool DecodeInputValue(FWaldoInputValue& outInputValue) const;

    bool DecodeMessage(FString& outMessage) const;

private:
    const FWaldoCommand& Command;
};
