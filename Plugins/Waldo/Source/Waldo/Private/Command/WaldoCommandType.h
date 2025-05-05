#pragma once

#include "WaldoCommandType.generated.h"

UENUM()
enum class EWaldoCommandType : int8
{
    // Device
    Reset = 'R',
    RegisterInput = 'I',
    StartFrame = 'F',
    InputValue = 'V',
    EndFrame = 'E',
    Message = 'M',
    
    // Host
    AcknowledgeReset = 'A',
    AcknowledgeFrame = 'K'
};

inline FString ToString(EWaldoCommandType Type)
{
    switch (Type)
    {
        case EWaldoCommandType::Reset:
            return "Reset";
        case EWaldoCommandType::RegisterInput:
            return "RegisterInput";
        case EWaldoCommandType::StartFrame:
            return "StartFrame";
        case EWaldoCommandType::InputValue:
            return "InputValue";
        case EWaldoCommandType::EndFrame:
            return "EndFrame";
        case EWaldoCommandType::Message:
            return "Message";
        default:
            return "Unknown";
    }
}
