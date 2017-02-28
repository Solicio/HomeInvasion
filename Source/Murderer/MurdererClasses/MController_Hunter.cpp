// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MController_Hunter.h"


AMController_Hunter::AMController_Hunter()
{

	MurdererType = EMurdererType::EHunter;
}


void AMController_Hunter::BeginPlay()
{
	Super::BeginPlay();
	// Note: Possess occurs after begin play.
}

// Called every frame
void AMController_Hunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

