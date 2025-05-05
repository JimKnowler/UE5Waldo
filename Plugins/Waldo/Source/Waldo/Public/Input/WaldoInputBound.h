#pragma once

#include "CoreMinimal.h"
#include "WaldoInputBound.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateWaldoInputValue, int, Value);

USTRUCT()
struct FWaldoInputBound
{
	GENERATED_BODY()

	UPROPERTY()
	FString Label;

	UPROPERTY()
	FDelegateWaldoInputValue Delegate;	
};
