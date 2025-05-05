#pragma once

#include "WaldoCommand.h"
#include "Input/WaldoInput.h"
#include "Input/WaldoInputValue.h"

class FWaldoCommandDecoder
{
public:
    FWaldoCommandDecoder(const FWaldoCommand& command);

    bool RegisterInput(FWaldoInput& outInput) const;

    bool InputValue(FWaldoInputValue& outInputValue) const;

    bool Message(FString& outMessage) const;

private:
    const FWaldoCommand& Command;
};
