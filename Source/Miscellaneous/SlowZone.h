// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SlowZone.generated.h"

UCLASS()
class HOMEINVASION_API ASlowZone : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		ASlowZone();

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		// Cube component for overlap detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UBoxComponent* OverlapBox;

		// Component used to line trace from to check for which direction the wind is coming from
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* WindStartLocation;

		// If the zone is active
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool Active;

		// If the slow only should be applied when moving towards the wind direction
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool BasedOfWind;

		// If the slow should be based of wind but if there should be a base slow even when not moving towards the wind.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables", meta = (EditCondition = "BasedOfWind"))
		float BaseSpeedMulti;

		// How much the movement speed should be slowed, 1 = no change, 0.5 = half speed
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		float PlayerSpeedMulti;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool ShouldDebug;

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

	
	
};
