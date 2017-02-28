// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "LevelPhasesData.generated.h"

USTRUCT(BlueprintType)
struct FPhase
{
	GENERATED_BODY()

	/*	Name to check for when trying to check if the levels phase should be progressed when player competes something.
	*	Also used to set the next phase when progressing the level.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString PhaseName;

	// What the player needs to accomplish.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString PhaseDescription;

};

/**
 * 
 */
UCLASS(BlueprintType)
class HOMEINVASION_API ULevelPhasesData : public UDataAsset
{
	GENERATED_BODY()
	
	public:

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
		FString LevelName;
	
		// The different phases that exists in the level.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		TArray<FPhase> Phases;
	
		// The current phase of the level.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FPhase CurrentPhase;
};
