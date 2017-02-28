// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MurdererAIPerceptionComponent.h"


UMurdererAIPerceptionComponent::UMurdererAIPerceptionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Sight
	SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("Sight Config"));
	ConfigureSense(*SightConfig);

	// Hearing
	HearingConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Hearing>(this, TEXT("Hearing Config"));
	ConfigureSense(*HearingConfig);

	// Dominant sense - Sight
	SetDominantSense(SightConfig->GetSenseImplementation());
}

void UMurdererAIPerceptionComponent::SetUpSight(float SightRange, float LoseSightRange, float PeripheralVisionAngle)
{
	// Setting values for the sight config
	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	ConfigureSense(*SightConfig);
}

void UMurdererAIPerceptionComponent::SetUpHearing(float HearingRange)
{
	// Setting values for the hearing config
	HearingConfig->HearingRange = HearingRange;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	ConfigureSense(*HearingConfig);
}