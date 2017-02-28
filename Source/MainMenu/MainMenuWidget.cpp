// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MainMenuWidget.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Start out faded out
	FadeOpacity = 1.0f;
	CurrentFadeState = EFadeState::EFadedOut;
}

void UMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!FadeComplete)
	{
		// Increase opacity of fade image to 1.0, fade out
		if (CurrentFadeDirection == EFadeDirection::EFadeOut)
		{
			if (FadeOpacity < 1.0f)
				FadeOpacity += DeltaTime * FadeSpeed;

			if (FadeOpacity >= 1.0f)
			{
				CurrentFadeState = EFadeState::EFadedOut;
				FadeComplete = true;
			}
		}
		// Decrease opacity of fade image to 0.0, fade in
		else if (CurrentFadeDirection == EFadeDirection::EFadeIn)
		{
			if (FadeOpacity > 0.0f)
				FadeOpacity -= DeltaTime * FadeSpeed;

			if (FadeOpacity <= 0.0f)
			{
				CurrentFadeState = EFadeState::EFadedIn;
				FadeComplete = true;
			}
		}

		// Clamp the value so it's not over or under the min and max
		FadeOpacity = FMath::Clamp(FadeOpacity, 0.0f, 1.0f);
	}
}

void UMainMenuWidget::LoadLevel(FName LevelName, APlayerController* Controller)
{
	// Puts the focus onto the game
	FInputModeGameOnly GameMode;
	Controller->SetInputMode(GameMode);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void UMainMenuWidget::FadeIn(FColor Color, float Length)
{
	// Set the color and speed
	FadeColor = Color;
	FadeSpeed = 1 / Length;

	// Set the current stade to fading and which direction it should be faded
	CurrentFadeState = EFadeState::EFading;
	CurrentFadeDirection = EFadeDirection::EFadeIn;

	// Start the fade
	FadeComplete = false;
}

void UMainMenuWidget::FadeOut(FColor Color, float Length)
{
	// Set the color and speed
	FadeColor = Color;
	FadeSpeed = 1 / Length;

	// Set the current stade to fading and which direction it should be faded
	CurrentFadeState = EFadeState::EFading;
	CurrentFadeDirection = EFadeDirection::EFadeOut;

	// Start the fade
	FadeComplete = false;
}

EFadeState UMainMenuWidget::GetFadeState()
{
	return CurrentFadeState;
}