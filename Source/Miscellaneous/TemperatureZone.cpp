// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "TemperatureZone.h"


// Sets default values
ATemperatureZone::ATemperatureZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// The root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	// Create the box used to check if the player is inside
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap Box"));
	OverlapBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	OverlapBox->bGenerateOverlapEvents = true;
	//OverlapBox->AttachParent = RootComponent;
	OverlapBox->SetupAttachment(RootComponent);

	LineOfSightCheck = CreateDefaultSubobject<USceneComponent>(TEXT("LineOfSightCheck"));
	//LineOfSightCheck->AttachTo(RootComponent);
	LineOfSightCheck->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Tags.Add("TemperatureZone");
}

// Called when the game starts or when spawned
void ATemperatureZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATemperatureZone::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

}

