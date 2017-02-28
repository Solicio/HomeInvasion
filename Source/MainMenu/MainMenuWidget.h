// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../PlayerCharacter/PlayerHUDWidget.h"

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"


/**
 * 
 */
UCLASS()
class HOMEINVASION_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

	public:

		UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

		/** Opens specified level */
		UFUNCTION(BlueprintCallable, Category = "Level Progression")
		void LoadLevel(FName LevelName, APlayerController* Controller);


		//-- Fade In/Out --//

		/** Fade from specified color to the game */
		UFUNCTION(BlueprintCallable, Category = "Fade In/Out")
		void FadeIn(FColor Color = FColor::Black, float Length = 1.0f);

		/** Fade from game to the specified color */
		UFUNCTION(BlueprintCallable, Category = "Fade In/Out")
		void FadeOut(FColor Color = FColor::Black, float Length = 1.0f);

		// Opacity of fade image
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade In/Out")
		float FadeOpacity;

		// If the fade has been completed. Fully visible game or fully blocked by the fade image
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade In/Out")
		bool FadeComplete;

		/** Get the current state of the fade. Fully faded out/in or currently not known */
		UFUNCTION(BlueprintPure, Category = "Fade In/Out")
		EFadeState GetFadeState();

		// Color of the fade image
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade In/Out")
		FLinearColor FadeColor;

	private:

		// Direction the image should be faded
		EFadeDirection CurrentFadeDirection;

		// The current state the fade is in.
		EFadeState CurrentFadeState;

		// Speed the image should fade at. FadeSpeed * DeltaTime. Value 1 for fade over 1 second, 0.5 for fade over 2 seconds
		float FadeSpeed;


	protected:

		/** Called every frame */
		virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
