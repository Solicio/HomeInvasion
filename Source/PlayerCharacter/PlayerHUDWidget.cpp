// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PlayerHUDWidget.h"

#include "FPSPlayerController.h"

UPlayerHUDWidget::UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Start out faded out
	FadeOpacity = 1.0f;
	CurrentFadeState = EFadeState::EFadedOut;
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
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

	// Interaction text
	if (InteractionTextDuration > 0.0f)
	{
		InteractionTextDuration -= DeltaTime;
	}
	else
	{
		InteractionText = FText::GetEmpty();
	}
}

EHUDState UPlayerHUDWidget::GetHUDState()
{
	if (PlayerController == nullptr)
		return EHUDState::EGame;

	if (IsPaused)
	{
		return EHUDState::EPaused;
	}
	else if (InventoryActive)
	{
		return EHUDState::EInventory;
	}
	else if (MapActive)
	{
		return EHUDState::EMap;
	}
	else if (InspectModeActive)
	{
		return EHUDState::EInspect;
	}
	else if (WheelOpen)
	{
		return EHUDState::EAbilityWheel;
	}
	else if (UpgradeScreenVisible)
	{
		return EHUDState::EUpgrade;
	}
	else if (PlayerController->CharacterHasPhoneOut)
	{
		return EHUDState::EPhone;
	}
	
	return EHUDState::EGame;
}

EPlayerBreathingState UPlayerHUDWidget::GetBreathingState()
{
	if (PlayerController == nullptr)
		return EPlayerBreathingState::ENormal;

	// Inhaling
	if (PlayerController->InhaleDuration > 0.3f)
	{
		return EPlayerBreathingState::EInhaling;
	}
	// Exhaling, Successful
	else if (PlayerController->ExhaleDuration > 0.0f && PlayerController->SuccessfulBreath)
	{
		return EPlayerBreathingState::ESuccessfulBreath;
	}
	// Exhaling
	else if (PlayerController->ExhaleDuration > 0.0f && !PlayerController->SuccessfulBreath)
	{
		return EPlayerBreathingState::EExhaling;
	}
	// Coughing
	else if (PlayerController->CoughDuration > 0.0f)
	{
		return EPlayerBreathingState::ECoughing;
	}

	return EPlayerBreathingState::ENormal;
}

void UPlayerHUDWidget::DisplayInteractionText(FText Text, float Duration)
{
	// Set the text and duration to display the text
	InteractionText = Text;
	InteractionTextDuration = Duration;
}

void UPlayerHUDWidget::DisplayInteractionString(FString String, float Duration)
{
	// Set the text and duration to display the text
	InteractionText = FText::FromString(String);
	InteractionTextDuration = Duration;
}

void UPlayerHUDWidget::TogglePause()
{
	//PrintText("Toggle Pause", -1, FColor::Red, 2.0f);
	IsPaused = !IsPaused;
	BPTogglePause();
}

void UPlayerHUDWidget::ToggleInventory(bool GiveMode)
{
	if (!IsPaused && !InspectModeActive && !WheelOpen)
	{
		UpdateInventory();
		InventoryActive = !InventoryActive;
		BPToggleInventory(GiveMode);
	}
}

void UPlayerHUDWidget::ToggleMap()
{
	if (!IsPaused && !InspectModeActive && !WheelOpen)
	{
		MapActive = !MapActive;
		BPToggleMap();
	}
}

void UPlayerHUDWidget::SetWheelVisible(bool NewVisible)
{
	if (!IsPaused && !InspectModeActive && !MapActive)
	{
		if (WheelOpen != NewVisible)
		{
			WheelOpen = NewVisible;
			BPWheelVisible(WheelOpen);
		}
	}
}

void UPlayerHUDWidget::SetUpgradeScreenVisible(bool NewVisible)
{
	if (!IsPaused && !InspectModeActive && !MapActive)
	{
		if (UpgradeScreenVisible != NewVisible)
		{
			UpgradeScreenVisible = NewVisible;
			BPUpgradeScreenChange(UpgradeScreenVisible);
		}
	}
}

void UPlayerHUDWidget::SetSaveScreenVisible(bool NewVisible)
{
	if (!IsPaused && !InspectModeActive && !MapActive)
	{
		if (SaveScreenVisible != NewVisible)
		{
			SaveScreenVisible = NewVisible;
			BPSaveScreenChange(SaveScreenVisible);
		}
	}
}

//void UPlayerHUDWidget::DisplaySubtitleText(FText Text, float Duration)
//{
//	// Set the text and duration to display the text
//	//InteractionText = Text;
//	//InteractionTextDuration = Duration;
//}

void UPlayerHUDWidget::FadeIn(FColor Color, float Length)
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

void UPlayerHUDWidget::FadeOut(FColor Color, float Length)
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

EFadeState UPlayerHUDWidget::GetFadeState()
{
	return CurrentFadeState;
}
