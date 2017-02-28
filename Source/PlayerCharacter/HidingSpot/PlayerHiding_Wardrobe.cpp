// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PlayerHiding_Wardrobe.h"

APlayerHiding_Wardrobe::APlayerHiding_Wardrobe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpotType = EHidingSpotTypes::EWardrobe;

	//DoorComponent1 = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("Door Comp 1"));
	//DoorComponent1->AttachParent = RootComponent;

	//DoorComponent2 = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("Door Comp 2"));
	//DoorComponent2->AttachParent = RootComponent;
}

// Called when the game starts or when spawned
void APlayerHiding_Wardrobe::BeginPlay()
{
	Super::BeginPlay();

	//Door1 = Cast<AIO_Door>(DoorComponent1->ChildActor);
	//Door2 = Cast<AIO_Door>(DoorComponent2->ChildActor);
}

// Called every frame
void APlayerHiding_Wardrobe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (DoorsAIClosed())
	//{
	//	PrintText("Doors Closed", -1, FColor::Cyan, 0.0f);
	//}
	//else
	//{
	//	PrintText("Doors Opened", -1, FColor::Cyan, 0.0f);
	//}
}

bool APlayerHiding_Wardrobe::DoorsAIClosed()
{
	// Returns the current closed state of the main door if there only is one door
	if (Door1 != nullptr && Door2 == nullptr)
	{
		return Door1->AIClosed;
	}
	// If there is two doors
	else if (Door1 != nullptr && Door2 != nullptr)
	{
		// If one of them are closed, return that they are closed
		if (Door1->AIClosed || Door2->AIClosed)
		{
			return true;
		}
		// Else return that they are not closed
		else
		{
			return false;
		}
	}

	return true;
}

void APlayerHiding_Wardrobe::AIOpenDoors()
{
	// Check if the door is valid and closed before opening.
	if (Door1 != nullptr && Door1->AIClosed)
	{
		Door1->AIAutoOpen();
	}

	if (Door2 != nullptr && Door2->AIClosed)
	{
		Door2->AIAutoOpen();
	}
}