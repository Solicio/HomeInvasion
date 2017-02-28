// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "InteractableObjects/InteractableObject.h"
#include "IO_SoundObject.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AIO_SoundObject : public AInteractableObject
{
	GENERATED_BODY()
	
	public:	
	
		/** Sets default values for this actor's properties */
		AIO_SoundObject(const FObjectInitializer& ObjectInitializer);

		// Used to get the location where the AI should walk to before checking the hidingspot
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UArrowComponent* AICheckLocationArrow;

		// Used to register for senses and to report noise events.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		UAIPerceptionStimuliSourceComponent* StimuliSourceComp;

		// Cube component for trace detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UBoxComponent* OverlapBox;


		/** Interact with the object, when the player is looking at the object (Default LMB) */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		virtual void Interact(AActor* InteractingActor);
	
		
		// Where the AI should run to before checking the hidingspot
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		FVector AICheckLocation;

		// If the object is playing sound
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		bool PlayingSound;

		// The noise strength that the object will create
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float NoiseStrength;

		// The delay between each noise created for the AI to hear
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float NoiseDelay;

		/** Used to activate the currently selected element on the phone */
		UFUNCTION(BlueprintCallable, Category = "Noise")
		void CreateNoise();


	protected:	

		// Handle to manage the time
		FTimerHandle NoiseTimerHandle;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
};
