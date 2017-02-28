// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryItems/InventoryItem.h"

#include "InteractableObjects/InteractableObject.h"
#include "IO_PlayerFriend.generated.h"


/** Struct of variables needed to save and load the friend character. */
USTRUCT(BlueprintType)
struct FFriendSaveSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float EnergyPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float ColdPercentage;

	FFriendSaveSlot()
	{
		Health = 0;
		EnergyPercentage = 0;
		ColdPercentage = 0;
	}
};
/**
 * 
 */
UCLASS()
class HOMEINVASION_API AIO_PlayerFriend : public AInteractableObject
{
	GENERATED_BODY()
	
	public:

		/** Sets default values for this actor's properties */
		AIO_PlayerFriend(const FObjectInitializer& ObjectInitializer);
	
		/** Interact with the object, when the player is looking at the object (Default LMB) */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		virtual void Interact(AActor* InteractingActor) override;
	

		// The health of the character, dies if it reaches 0
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float Health;


		// The energy level of the character, if it drops to 0, the character dies.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float EnergyPercentage;

		// By how much the energy percentage should decrease every update
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float EnergyDecrease;

		// Multiplier value that is used when decreasing the energy
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float EnergyDecreaseMulti;


		// The cold level of the character if it drops to 0, the character dies.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float ColdPercentage;

		// By how much the cold percentage should decrease every update
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float ColdDecrease;

		// Multiplier value that is used when decreasing the energy
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friend Variables")
		float ColdDecreaseMulti;


		/** Function that handles getting items from the player */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Friend Functions")
		void GiveItem(AInventoryItem* Item);


	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
};
