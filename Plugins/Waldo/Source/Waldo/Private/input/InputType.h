#pragma once

UENUM()
enum class EInputType : uint8 {
    Digital = 0,
    Analog = 1
};

inline FString ToString(EInputType Type)
{
    switch (Type)
    {
        case EInputType::Digital:
            return "Digital";
        case EInputType::Analog:
            return "Analog";
        default:
            return "Unknown";
    }
}