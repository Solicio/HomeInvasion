// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "InventoryItem.generated.h"

UCLASS()
class HOMEINVASION_API AInventoryItem : public AActor
{
	GENERATED_BODY()
	
	public:	
		
		// Sets default values for this actor's properties
		AInventoryItem();

		// Used when droppping this item from the inventory, spawns the world object.
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
		UClass* InventoryObjectBP;

		/** Called when object is used from the invetory */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
		void UseItem(AActor* Character);

		// Name of the object
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FString Name;

		// Image that is displayed inside the inventory
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UTexture2D* InventoryImage;

		// The text displayed when selecting the item inside the inventory
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FString ActionText;

		// The text displayed before the name of the item when picking up the item.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FString PickupPrefixText;

		// The index in the array/inventory
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 Index;


		// If the player can use the item from the inventory
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool UsableItem;

		// If the player can give away the item from the inventory screen
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool CanGiveAway;

		// If the player can drop the item from the inventory
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool CanDrop;


		// If when the player uses the item on the inventory screen it replenishes the players energy/cold
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool FoodItem;

		// How much of the value is restored
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (EditCondition = "FoodItem"))
		float FoodRestoreAmount;


		// If when the player uses the item on the inventory screen it restores health, to player or players friend
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool HealthItem;

		// How much of the value is restored
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (EditCondition = "HealthItem"))
		float HealthRestoreAmount;


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		float UseFontSize;


	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

};
