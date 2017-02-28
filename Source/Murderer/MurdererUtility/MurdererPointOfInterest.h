// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MurdererPointOfInterest.generated.h"

UCLASS()
class HOMEINVASION_API AMurdererPointOfInterest : public AActor
{
	GENERATED_BODY()
	
	public:	

		/** Sets default values for this actor's properties */
		AMurdererPointOfInterest(const FObjectInitializer& ObjectInitializer);
		
		// Root, attach onto this
		USceneComponent* RootComp;

		// How interesting the point is, will be compared to other points in view.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
		float InterestPower;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick( float DeltaSeconds ) override;
};
