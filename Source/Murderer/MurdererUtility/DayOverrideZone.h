// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DayOverrideZone.generated.h"

UCLASS()
class HOMEINVASION_API ADayOverrideZone : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		ADayOverrideZone();

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		// Used to turn the zone on/off for special cases where it should be able to toggle.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool ShouldOverride;

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

};
