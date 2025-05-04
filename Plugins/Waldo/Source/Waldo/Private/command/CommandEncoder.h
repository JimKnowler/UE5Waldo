#pragma once

#include "Command.h"

class FCommandEncoder {
public:
    FCommandEncoder(FCommand& command);

    ///////////////////////////////////////////////////
    // Device

    FCommand& Reset();
    
    FCommand& Message(const FString& message);

    ///////////////////////////////////////////////////
    // Host

    FCommand& AcknowledgeReset();

    FCommand& AcknowledgeFrame();

private:
    FCommand& Command;
};
