#pragma once

#include "CoreMinimal.h"
#include "InputValue.generated.h"

USTRUCT()
struct FInputValue
{
	GENERATED_BODY()

	UPROPERTY()
	int Id = -1;

	UPROPERTY()
	int Value = -1;
};
