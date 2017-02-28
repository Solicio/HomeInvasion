// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "IO_PlayerFriend.h"


// Sets default values
AIO_PlayerFriend::AIO_PlayerFriend(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	EnergyPercentage = 100.0f;
	ColdPercentage = 100.0f;

	EnergyDecrease = 1.0f;
	ColdDecrease = 1.0f;

	EnergyDecreaseMulti = 1.0f;
	ColdDecreaseMulti = 1.0f;
}

// Called when the game starts or when spawned
void AIO_PlayerFriend::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AIO_PlayerFriend::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AIO_PlayerFriend::Interact(AActor* InteractingActor)
{
	Super::Interact(InteractingActor);

}