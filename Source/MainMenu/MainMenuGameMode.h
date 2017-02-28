// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AMainMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
	public:

		AMainMenuGameMode(const FObjectInitializer& ObjectInitializer);

		//--- Settings ---//

		/** Applies FOV, Brightness, AA Method, Audio levels and various other settings */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Settings")
		void ApplyGeneralSettings(bool ApplySound = true, bool ApplyVideo = true, bool ApplyGeneral = true);

	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

};
