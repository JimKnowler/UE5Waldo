#pragma once

#include "CoreMinimal.h"
#include "InputType.h"
#include "Input.generated.h"

USTRUCT()
struct FInput {
    GENERATED_BODY()

    UPROPERTY()
    FString Label;

    UPROPERTY()
    int Pin = -1;

    UPROPERTY()
    int Id = -1;

    UPROPERTY()
    EInputType Type = EInputType::Analog;
};
