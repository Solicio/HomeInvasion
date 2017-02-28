// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "ObjectiveData.generated.h"

USTRUCT(BlueprintType)
struct FObjectiveSegment
{

	GENERATED_BODY()

	/*	Segment name to check for when trying to check if the objective should be progressed when player competes something.
	*	Also used to set the next segment when progressing the objective.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString SegmentName;

	// What the player needs to accomplish.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString SegmentDescription;

	// Where to put the track location in the world.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Struct")
	FVector TrackLocation;

};

USTRUCT(BlueprintType)
struct FObjectiveSaveSlot
{
	GENERATED_BODY()

	// The name of the entire objective
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Struct")
	FString ObjectiveName;

	// The name of the current segment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString CurrentSegmentName;

};

/** A player objective. Contians all the different segments for the player to complete in game.
 * 
 */
UCLASS(BlueprintType)
class HOMEINVASION_API UObjectiveData : public UDataAsset
{
	GENERATED_BODY()
		
	public:

	// The name of the entire objective. Not the individual parts
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FString ObjectiveName;

	// The different parts this objective is segmented into
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FObjectiveSegment> ObjectiveSegments;

	// The current segment to be completed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FObjectiveSegment CurrentSegment;

	// How important this objective is. The higher the value the more important the objective.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	int32 ObjectiveImportance;
	
	// If the objective is active, can be completed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool Active = true;
	
};
