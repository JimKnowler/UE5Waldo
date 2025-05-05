#pragma once

#include "CoreMinimal.h"

#include "WaldoCommandType.h"

#include <stdint.h>

/**
 * @struct FWaldoCommand
 * @brief Encapsulate a command and its arguments
 */
class FWaldoCommand
{
public:
    EWaldoCommandType GetType() const;

    const TArray<uint8>& GetData() const;

private:
    friend class UWaldoCommandByteStream;
    friend class FWaldoCommandEncoder;
    
    EWaldoCommandType Type = EWaldoCommandType::Reset;

    TArray<uint8> Data;
};
