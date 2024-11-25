// Copyright Epic Games, Inc. All Rights Reserved.

#include "IAFinalGameMode.h"
#include "IAFinalCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIAFinalGameMode::AIAFinalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
