// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "InteractableObject.h"
#include "IO_PickupObject.generated.h"

/** Struct of the GUID and location of saved/loaded pickup object */
USTRUCT(BlueprintType)
struct FPickupObjectSaveSlot
{
	GENERATED_BODY()

	// The GUID of the object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FGuid GUID;

	// The objects transform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FTransform Transform;
};

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AIO_PickupObject : public AInteractableObject
{
	GENERATED_BODY()

	public:

		/** Sets default values for this actor's properties */
		AIO_PickupObject(const FObjectInitializer& ObjectInitializer);
	
		// Used to register for senses and to report noise events.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		UAIPerceptionStimuliSourceComponent* StimuliSourceComp;


		/** Interact with the object, when the player is looking at the object (Default LMB) */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		virtual void Interact(AActor* InteractingActor) override;
	

		// If the player is currently holding the object
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		bool PlayerHeld;

		// If the object has entered a stationary state, not simulating physics.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		bool Stationary;

		// If the object has been thrown by the player, turned to false when colliding
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		bool Thrown;

		// If the objects has move from the spawn location
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		bool HasBeenMoved;
		
		// The current speed the object is moving at
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float ObjectSpeed;

		// This value will lower the object relative to the players camera, the taller the object is, the higher this value should be.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float PlayerCameraDownZ;


		//---- Noise and Sound ----//

		// Time needed to pass betweem playing sound effect
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float SoundCooldown;

		// Time needed to pass betweem playing AI Noise
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float NoiseCooldown;

		// Value used when counting down the noise cooldown
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float NoiseCDCountdown;

		// The strength of the noise made when colliding after being thrown
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float ThrownNoiseStrength;

		// The strength of the noise made when colliding normally
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Variables")
		float CollisionNoiseStrength;


		/** Used to activate the currently selected element on the phone */
		UFUNCTION(BlueprintCallable, Category = "Noise")
		void CreateNoise(FVector Location, float Loudness);

		/** Called when the object collides with something */
		UFUNCTION(BlueprintImplementableEvent, Category = "Object Functions")
		void Collision(bool WasThrown, float Speed, bool HitEnemy);

		/** Called when the object should play an audio clip */
		UFUNCTION(BlueprintImplementableEvent, Category = "Object Functions")
		void PlaySound(float Speed);


	protected:

		float SoundCDCountdown;
		float DurationNotMoving;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

		UFUNCTION()
		void HitOtherActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
};
