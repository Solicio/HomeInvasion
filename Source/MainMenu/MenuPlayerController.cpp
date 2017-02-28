// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MenuPlayerController.h"

AMenuPlayerController::AMenuPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AMenuPlayerController::BeginPlay()
{
	if (MainMenuWidgetClass != nullptr && MainMenuWidget == nullptr)
	{
		// Create Widget
		MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);

		if (!MainMenuWidget)
			return;

		// Add it to the viewport so the Construct() method in the UUserWidget:: is run.
		MainMenuWidget->AddToViewport();
		
		// Puts the focus onto the menu
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(MainMenuWidget->GetCachedWidget());
		SetInputMode(Mode);

		// Fade in the game after load
		MainMenuWidget->FadeIn(FColor::Black, 1.0f);
	}

	Super::BeginPlay();
}

void AMenuPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



