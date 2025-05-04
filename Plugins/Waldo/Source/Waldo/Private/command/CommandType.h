#pragma once

#include "CommandType.generated.h"

UENUM()
enum class ECommandType : int8
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

inline FString ToString(ECommandType Type)
{
    switch (Type)
    {
        case ECommandType::Reset:
            return "Reset";
        case ECommandType::RegisterInput:
            return "RegisterInput";
        case ECommandType::StartFrame:
            return "StartFrame";
        case ECommandType::InputValue:
            return "InputValue";
        case ECommandType::EndFrame:
            return "EndFrame";
        case ECommandType::Message:
            return "Message";
        default:
            return "Unknown";
    }
}
