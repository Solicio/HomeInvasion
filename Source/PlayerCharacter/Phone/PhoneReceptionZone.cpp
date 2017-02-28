// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PhoneReceptionZone.h"


// Sets default values
APhoneReceptionZone::APhoneReceptionZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = SceneRoot;

	OverlapSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Overlap Sphere"));
	OverlapSphere->SetSphereRadius(500.0f);
	OverlapSphere->bGenerateOverlapEvents = true;
	//OverlapSphere->AttachParent = RootComponent;
	OverlapSphere->SetupAttachment(RootComponent);

	Tags.Add("ReceptionZone");
}

// Called when the game starts or when spawned
void APhoneReceptionZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhoneReceptionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

