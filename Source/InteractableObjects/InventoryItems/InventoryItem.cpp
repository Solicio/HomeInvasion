// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "InventoryItem.h"


// Sets default values
AInventoryItem::AInventoryItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UsableItem = true;
	CanGiveAway = false;
	CanDrop = true;
	UseFontSize = 18.0f;

	PickupPrefixText = "Picked up";
	ActionText = "Used";
}

// Called when the game starts or when spawned
void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

