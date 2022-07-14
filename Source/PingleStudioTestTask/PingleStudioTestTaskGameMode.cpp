// Copyright Epic Games, Inc. All Rights Reserved.

#include "PingleStudioTestTaskGameMode.h"
#include "PingleStudioTestTaskCharacter.h"
#include "UObject/ConstructorHelpers.h"

APingleStudioTestTaskGameMode::APingleStudioTestTaskGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
