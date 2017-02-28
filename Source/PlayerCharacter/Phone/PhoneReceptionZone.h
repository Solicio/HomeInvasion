// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PhoneReceptionZone.generated.h"

UCLASS()
class HOMEINVASION_API APhoneReceptionZone : public AActor
{
	GENERATED_BODY()
	
	public:	

		// Sets default values for this actor's properties
		APhoneReceptionZone(const FObjectInitializer& ObjectInitializer);

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		// Cube component for trace detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USphereComponent* OverlapSphere;


		// The current Zone ID of the Zone, set during play by the Waypoint manager
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		int32 ReceptionStrength;

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
};
