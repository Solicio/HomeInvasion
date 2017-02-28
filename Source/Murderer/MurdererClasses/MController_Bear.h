// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Murderer/MurdererController.h"
#include "MController_Bear.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AMController_Bear : public AMurdererController
{
	GENERATED_BODY()
	
	public:

		AMController_Bear();


	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
	
	
};
