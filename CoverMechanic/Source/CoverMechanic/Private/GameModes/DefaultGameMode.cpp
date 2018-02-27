// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultGameMode.h"
#include "ConstructorHelpers.h"

ADefaultGameMode::ADefaultGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerCharacterClassFinder(TEXT("/Game/Blueprints/Player/BP_PlayerCharacter"));
	DefaultPawnClass = PlayerCharacterClassFinder.Class;
}
