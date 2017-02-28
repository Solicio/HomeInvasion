// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PlayerVisiblityPoints.h"


// Sets default values
APlayerVisiblityPoints::APlayerVisiblityPoints(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	SetRootComponent(RootComp);

	Active = true;

	Tags.Add("VisibilityPoint");
}

// Called when the game starts or when spawned
void APlayerVisiblityPoints::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerVisiblityPoints::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

