#pragma once

#include "CoreMinimal.h"
#include "WaldoInputValue.generated.h"

USTRUCT()
struct FWaldoInputValue
{
	GENERATED_BODY()

	UPROPERTY()
	int Id = -1;

	UPROPERTY()
	int Value = -1;
};
