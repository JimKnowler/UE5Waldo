#pragma once

#include "WaldoCommand.h"

class FWaldoCommandEncoder {
public:
    FWaldoCommandEncoder(FWaldoCommand& command);

    ///////////////////////////////////////////////////
    // Device

    FWaldoCommand& EncodeReset();
    
    FWaldoCommand& EncodeMessage(const FString& message);

    ///////////////////////////////////////////////////
    // Host

    FWaldoCommand& EncodeAcknowledgeReset();

    FWaldoCommand& EncodeAcknowledgeFrame();

private:
    FWaldoCommand& Command;
};
