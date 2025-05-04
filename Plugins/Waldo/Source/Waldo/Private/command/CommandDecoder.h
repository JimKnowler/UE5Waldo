#pragma once

#include "Command.h"
#include "input/Input.h"
#include "input/InputValue.h"

class FCommandDecoder
{
public:
    FCommandDecoder(const FCommand& command);

    bool RegisterInput(FInput& outInput) const;

    bool InputValue(FInputValue& outInputValue) const;

    bool Message(FString& outMessage) const;

private:
    const FCommand& Command;
};
