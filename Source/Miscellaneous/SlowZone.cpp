// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "SlowZone.h"


// Sets default values
ASlowZone::ASlowZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	// Create the box used to check if the player is inside
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap Box"));
	OverlapBox->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
	OverlapBox->bGenerateOverlapEvents = true;
	OverlapBox->SetupAttachment(RootComponent);

	WindStartLocation = CreateDefaultSubobject<USceneComponent>(TEXT("WindStartLocation"));
	//WindStartLocation->AttachTo(RootComponent);
	WindStartLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Active = true;
	PlayerSpeedMulti = 0.5f;
	BaseSpeedMulti = 1.0f;

	Tags.Add("SlowZone");
}

// Called when the game starts or when spawned
void ASlowZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlowZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

