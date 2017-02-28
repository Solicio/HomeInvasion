// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MurdererWaypoint.h"


// Sets default values
AMurdererWaypoint::AMurdererWaypoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	SetRootComponent(RootComp);
	
	WaypointRadius = 500.0f;

	WaypointRadiusSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Sphere Radius"));
	WaypointRadiusSphere->SetSphereRadius(10.0f);
	//WaypointRadiusSphere->AttachTo(RootComp);
	WaypointRadiusSphere->SetupAttachment(RootComp);

	WaypointID = 0;

	Active = true;
}

// Called when the game starts or when spawned
void AMurdererWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMurdererWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

