// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LightZone.generated.h"

UCLASS()
class HOMEINVASION_API ALightZone : public AActor
{
	GENERATED_BODY()
	
	public:	

		// Sets default values for this actor's properties
		ALightZone();
		
		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		//// Component used to set where the light should affect the AI
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		//UShapeComponent* TriggerVolume;


		// If the zones lights are on
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		bool LightsOn;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		TArray<ALight*> ConnectedLights;

		// The delay between checking if the lights are on or not
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		float CheckLightsEvery;

		/** Check if any of the lights are on */
		UFUNCTION(BlueprintCallable, Category = "General Functions")
		void CheckLights();

		/** Check if any of the lights are on */
		UFUNCTION(BlueprintImplementableEvent, Category = "General Functions")
		void LightCheck();

		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		//TSubclassOf<class UNavArea> DefaultArea;

		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		//TSubclassOf<class UNavArea> ObstacleArea;

		// SET THIS SOMEWHERE
		UWorld* WorldRef;

	private:

		FTimerHandle CheckLightsTimer;

		UShapeComponent* TriggerShape;

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
};
