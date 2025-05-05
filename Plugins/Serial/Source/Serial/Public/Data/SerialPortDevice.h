#pragma once

#include "CoreMinimal.h"
#include "SerialPortDevice.generated.h"

USTRUCT(BlueprintType)
struct FSerialPortDevice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Description;

	bool operator==(const FSerialPortDevice& Other) const
	{
		return (Name == Other.Name) && (Description == Other.Description);
	}
};
