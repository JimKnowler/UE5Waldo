#pragma once

UENUM()
enum class EWaldoInputType : uint8 {
    Digital = 0,
    Analog = 1
};

inline FString ToString(EWaldoInputType Type)
{
    switch (Type)
    {
        case EWaldoInputType::Digital:
            return "Digital";
        case EWaldoInputType::Analog:
            return "Analog";
        default:
            return "Unknown";
    }
}