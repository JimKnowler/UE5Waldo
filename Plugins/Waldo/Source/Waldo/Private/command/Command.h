#pragma once

#include "CoreMinimal.h"

#include "CommandType.h"

#include <stdint.h>

/**
 * @struct FCommand
 * @brief Encapsulate a command and its arguments
 */
class FCommand
{
public:
    ECommandType GetType() const;

    const TArray<uint8>& GetData() const;

private:
    friend class UCommandByteStream;
    friend class FCommandEncoder;
    
    ECommandType Type = ECommandType::Reset;

    TArray<uint8> Data;
};
