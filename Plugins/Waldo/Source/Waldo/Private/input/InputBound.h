#pragma once

#include "CoreMinimal.h"
#include "InputBound.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateInputValue, int, Value);

USTRUCT()
struct FInputBound
{
	GENERATED_BODY()

	UPROPERTY()
	FString Label;

	UPROPERTY()
	FDelegateInputValue Delegate;	
};
