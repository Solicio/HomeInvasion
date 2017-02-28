// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MController_Bear.h"

AMController_Bear::AMController_Bear()
{

	MurdererType = EMurdererType::EBear;
}


void AMController_Bear::BeginPlay()
{
	Super::BeginPlay();
	// Note: Possess occurs after begin play.
}

// Called every frame
void AMController_Bear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


