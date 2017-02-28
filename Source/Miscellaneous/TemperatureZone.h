// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TemperatureZone.generated.h"

UCLASS()
class HOMEINVASION_API ATemperatureZone : public AActor
{
	GENERATED_BODY()
	
	public:	

		// Sets default values for this actor's properties
		ATemperatureZone();

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		// Cube component for overlap detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UBoxComponent* OverlapBox;

		// Component used to line trace from to check for line of sight
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* LineOfSightCheck;


		// The temperature that will be used in calculations when the player is inside this zone. Can be positive or negative.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		float Temperature;

		// If this temperature zone should count as player being inside
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool Indoors;

		// If the tempetarue zone requires nothing blocking the player to take effect. Line of sight is checked from the LineOfSightCheck component.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool RequiresLineOfSight;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool ShouldDebug;

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
};
