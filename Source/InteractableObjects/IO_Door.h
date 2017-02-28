// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "InteractableObject.h"
#include "IO_Door.generated.h"


/** Struct of variables needed to save and load the friend character. */
USTRUCT(BlueprintType)
struct FDoorSaveSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	bool Locked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FGuid GUID;

	FDoorSaveSlot()
	{
		Rotation = FRotator();
		Locked = false;
	}
};

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AIO_Door : public AInteractableObject
{
	GENERATED_BODY()

	public:

		/** Sets default values for this actor's properties */
		AIO_Door(const FObjectInitializer& ObjectInitializer);
	
		// The player will get the location of this comp when grabbing the door and will rotate the camera to look at this location.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Look at Comp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* LookAtSceneComp;

		/** Interact with the object, when the player is looking at the object (Default LMB) */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		virtual void Interact(AActor* InteractingActor) override;

		/** Used to move the door, open or close, depending on the float input value. Used by the player */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		void MoveDoor(float MouseInputVal, FName HandleName);

		/** Used by the player to automatically open or close the door using one click */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		void AutoOpen(FName HandleName);


		// If the door can only open in one way.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool OpensOneWay;

		// If the door can be just clicked and will automatically be opened
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool CanAutoOpen;

		// If the door is opening outward into negative yaw values. Normally doors open into positive yaw values.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool OpensIntoNegativeValue;

		// The max value the door can open in the Yaw axis. If the door opens into negative yaw, this value is negated.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		float MaximumYawRotation;

		// Normal lock on door, if locked cannot be opened
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool Locked;

		// What name to check the players inventory for a key.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		FString KeyName;

		// If the key should be removed from the players inventory after unlocking the door.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool RemoveKeyAfterUnlocking;

		// If the door is in the closed rotation, if it is, it will not be affected by physics
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Variables")
		bool Closed;

		// If the door is held by a player
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Variables")
		bool PlayerHeld;

		// If the door is held by an AI
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool Debugging;

		// If the door is held by an AI
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object Variables", meta = (EditCondition = "UseCustomInitalRot"))
		FRotator InitialDoorRotation;

		UPROPERTY(BlueprintReadOnly)
		bool UseCustomInitalRot;

		/** Sets the door mesh rotation to provided value */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		void SetDoorRotation(FRotator Rotation);

		/** Gets the door mesh rotation */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		FRotator GetDoorRotation();


		//--- AI Related ---//

		// If the door is held by an AI
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool AIHeld;

		// The value checked against the door rotation to see if the door is closed for the AI.
		// Closedrotation + This value is checked, if the door is within the value, it is closed.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Variables")
		float AIClosedRotation;

		// Similar to the normal Closed variable, except this variable is used to check for closed rotation +- 90 degrees.
		// The variable is used to check if the door needs to be opened more before the AI can go through it.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Variables")
		bool AIClosed;

		//// Used by the AI to see if the door is open enough to just run straight through it and ramming the door up.
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Variables")
		//bool AIRammable;

		// The location that the AI should look at on the door
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		FVector AILookAtLoc;

		/** Used by the AI to open the door. Will always open the door. */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		bool AIAutoOpen();


	protected:

		// Check if door should unlock
		void ShouldUnlock();

		// General movement variables
		float Speed;
		bool Moving;
		bool MovingForward;
		bool MovingBackward;
		bool MovingPastClosedRot;
		bool MovingPastMaximumRot;

		// Used to stop the door from closing when the door is not held
		float AutoOpened;

		float AIOpened;

		FRotator ClosedRotation;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
	
		UFUNCTION()
		void HitOtherActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
};
