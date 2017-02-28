// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../InteractableObjects/IO_Door.h"

#include "PlayerCharacter/HidingSpot/PlayerHidingSpot.h"
#include "PlayerHiding_Wardrobe.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API APlayerHiding_Wardrobe : public APlayerHidingSpot
{
	GENERATED_BODY()
	
	public:

		/** Sets default values for this actor's properties */
		APlayerHiding_Wardrobe(const FObjectInitializer& ObjectInitializer);

		//// Main door component for the wardrobe
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wardrobe Variables")
		//UChildActorComponent* DoorComponent1;

		//// Second door component for the wardrobe, will not always contain a door.
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wardrobe Variables")
		//UChildActorComponent* DoorComponent2;

		// Reference to the main door component
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wardrobe Variables")
		AIO_Door* Door1;

		// Second door reference, will not always contain a door. Check for nullptr before use.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wardrobe Variables")
		AIO_Door* Door2;

		// Check if both doors are open for the AI. Both doors are not AIClosed
		UFUNCTION(BlueprintCallable, Category = "Wardrobe Function")
		bool DoorsAIClosed();

		// Opens any closed doors, used by AI
		UFUNCTION(BlueprintCallable, Category = "Wardrobe Function")
		void AIOpenDoors();

		// Look at how the AI will be able to catch the player.
			// Turn of so that the doors does not affect navigation
			// Maybe nav link proxy to get closer?

	protected:
	
		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
};
