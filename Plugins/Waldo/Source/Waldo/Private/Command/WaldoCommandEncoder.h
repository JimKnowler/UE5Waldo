#pragma once

#include "WaldoCommand.h"

class FWaldoCommandEncoder {
public:
    FWaldoCommandEncoder(FWaldoCommand& command);

    ///////////////////////////////////////////////////
    // Device

    FWaldoCommand& Reset();
    
    FWaldoCommand& Message(const FString& message);

    ///////////////////////////////////////////////////
    // Host

    FWaldoCommand& AcknowledgeReset();

    FWaldoCommand& AcknowledgeFrame();

private:
    FWaldoCommand& Command;
};
