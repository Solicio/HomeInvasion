// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractableObjects/InteractableObject.h"
#include "IO_InventoryObject.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AIO_InventoryObject : public AInteractableObject
{
	GENERATED_BODY()
	
	public:

		/** Sets default values for this actor's properties */
		AIO_InventoryObject(const FObjectInitializer& ObjectInitializer);
	
		/** Interact with the object, when the player is looking at the object (Default LMB) */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		virtual void Interact(AActor* InteractingActor) override;

		/** Called when player has picked up this object, before it's destroyed */
		//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		//void BPPickedUp();

		// Used to create a new actor that is stored in the characters array
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
		UClass* InventoryItemBP;


		// If the player requires another item in order to interact with this object.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool NeedsOtherItem;

		// The name of the required item.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (EditCondition = "NeedsOtherItem"))
		FString RequiredItem;

	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
	
};
