// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

#include "Perception/AIPerceptionComponent.h"
#include "MurdererAIPerceptionComponent.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API UMurdererAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

	UMurdererAIPerceptionComponent(const FObjectInitializer& ObjectInitializer);
	
	public:
	
		/** Returns the sight config */
		UAISenseConfig_Sight* GetSightConfig(){ return SightConfig; }

		UPROPERTY(VisibleAnywhere, Category = "AI Sensing")
		UAISenseConfig_Sight* SightConfig;

		UPROPERTY(VisibleAnywhere, Category = "AI Sensing")
		UAISenseConfig_Hearing* HearingConfig;
		
		void SetUpSight(float SightRange, float LoseSightRange, float PeripheralVisionAngle);
		void SetUpHearing(float HearingRange);
};
