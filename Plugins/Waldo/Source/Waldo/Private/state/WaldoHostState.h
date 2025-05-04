#pragma once

#include "WaldoHostState.generated.h"

UENUM()
enum class EWaldoHostState : uint8
{
	None,
	Reset,						// waiting to receive reset command from device
	Ready,						// waiting for next command to be received from device
	Frame,						// currently receiving commands for a frame, until 'EndFrame' is received
};

inline FString ToString(EWaldoHostState State)
{
	switch (State)
	{
		case EWaldoHostState::None:
			return TEXT("None");
		case EWaldoHostState::Reset:
			return TEXT("Reset");
		case EWaldoHostState::Ready:
			return TEXT("Ready");
		case EWaldoHostState::Frame:
			return TEXT("Frame");
		default:
			return TEXT("unknown");
	}
}