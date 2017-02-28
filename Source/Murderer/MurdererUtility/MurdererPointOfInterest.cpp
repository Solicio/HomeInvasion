// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MurdererPointOfInterest.h"


// Sets default values
AMurdererPointOfInterest::AMurdererPointOfInterest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	SetRootComponent(RootComp);

	InterestPower = 0.0f;

	// Add tag to the actor which the AI can recognize.
	Tags.Add("PointOfInterest");
}

// Called when the game starts or when spawned
void AMurdererPointOfInterest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMurdererPointOfInterest::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

