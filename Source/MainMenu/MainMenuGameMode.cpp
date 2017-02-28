// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MainMenuGameMode.h"


AMainMenuGameMode::AMainMenuGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Set default player controller class to our blueprint
	static ConstructorHelpers::FClassFinder<APlayerController> ControllerBPClass(TEXT("/Game/Player/Menu/BP_MenuController"));
	if (ControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = ControllerBPClass.Class;
	}
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AMainMenuGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
