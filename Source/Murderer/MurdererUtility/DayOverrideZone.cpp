// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "DayOverrideZone.h"


// Sets default values
ADayOverrideZone::ADayOverrideZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	ShouldOverride = true;

	Tags.Add("DayOverride");
}

// Called when the game starts or when spawned
void ADayOverrideZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADayOverrideZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

