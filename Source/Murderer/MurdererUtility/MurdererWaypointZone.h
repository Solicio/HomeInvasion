// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MurdererWaypoint.h"

#include "GameFramework/Actor.h"
#include "MurdererWaypointZone.generated.h"

UCLASS()
class HOMEINVASION_API AMurdererWaypointZone : public AActor
{
	GENERATED_BODY()
	
	public:	
	
		/** Sets default values for this actor's properties */
		AMurdererWaypointZone(const FObjectInitializer& ObjectInitializer);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool ShouldPrintInfo;

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		// Cube component for trace detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UBoxComponent* OverlapBox;

		// If the player is inside the overlap box
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		bool PlayerInside;

		// How long the player has been inside this Zone
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		float PlayerInsideDuration;

		// The current Zone ID of the Zone, set during play by the Waypoint manager
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		int32 ZoneID;

		// All the waypoints inside this Zone
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		TArray<AMurdererWaypoint*> Waypoints;

		/** Set the ID of the Zone */
		UFUNCTION(BlueprintCallable, Category = "General Functions")
		void SetID(int32 NewID);


	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

		/** called when something enters the sphere component */
		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		/** called when something leaves the sphere component */
		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
