// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MurdererWaypoint.generated.h"


UCLASS()
class HOMEINVASION_API AMurdererWaypoint : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AMurdererWaypoint(const FObjectInitializer& ObjectInitializer);

		// Root, attach onto this
		USceneComponent* RootComp;
		
		// Child radius used by the AI to find a random location
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
		TArray<float> ChildRadiuses;

		// Radius the AI will walk around in.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Settings")
		USphereComponent* WaypointRadiusSphere;

		// Radius that the AI selects random locations in
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
		float WaypointRadius;

		// If the waypoint is active, if false = AI wont go there
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
		bool Active;

		// Used to check which waypoints certain murderers can travel too
		// This value and the murderers UsableWaypointID must match in order for the murderer to travel to this WP
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
		int32 WaypointID;

		// Child locations that are used by the AI when patrolling a waypoint. Relative to the world
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Settings")
		TArray<FVector> ChildLocations;

	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick( float DeltaSeconds ) override;
	
};
