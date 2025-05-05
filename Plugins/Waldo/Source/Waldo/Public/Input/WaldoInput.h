#pragma once

#include "CoreMinimal.h"
#include "WaldoInputType.h"
#include "WaldoInput.generated.h"

/**
 * 
 */
USTRUCT()
struct FWaldoInput {
    GENERATED_BODY()

    UPROPERTY()
    FString Label;

    UPROPERTY()
    int Pin = -1;

    UPROPERTY()
    int Id = -1;

    UPROPERTY()
    EWaldoInputType Type = EWaldoInputType::Analog;
};
