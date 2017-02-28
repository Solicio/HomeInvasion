// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "IO_InventoryObject.h"

#include "../PlayerCharacter/FPSCharacter.h"

// Sets default values
AIO_InventoryObject::AIO_InventoryObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set the needed mesh physics variables
	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(0.1f);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	Mesh->SetNotifyRigidBodyCollision(true);

	Name = "Inventory Object";
	RequiredItem = "Required item";

	Tags.Add("InventoryObject");
}


// Called when the game starts or when spawned
void AIO_InventoryObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AIO_InventoryObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIO_InventoryObject::Interact(AActor* InteractingActor)
{
	Super::Interact(InteractingActor);

	AFPSCharacter* Player = Cast<AFPSCharacter>(InteractingActor);

	// If this object requires another item in the players inventory to be picked up.
	if (NeedsOtherItem)
	{
		// The player has the item
		if (Player->HasItem(RequiredItem))
		{
			// Call the character that the item should be added and then destroy the object if it was added.
			if (Player->AddInventoryItem(InventoryItemBP))
			{
				// Let the save system known it was picked up
				AddToBeDestroyed();
				Destroy();
			}
		}
		else
		{
			Player->ShowDisplayInteractionString("Requires a " + RequiredItem);
		}
	}
	else
	{
		// Call the character that the item should be added and then destroy the object if it was added.
		if (Player->AddInventoryItem(InventoryItemBP))
		{
			// Let the save system known it was picked up
			AddToBeDestroyed();
			Destroy();
		}
	}
}