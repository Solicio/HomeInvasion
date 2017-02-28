// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MController_Deer.h"

AMController_Deer::AMController_Deer()
{

	IsAggressive = false;
	MurdererType = EMurdererType::EDeer;
}


void AMController_Deer::BeginPlay()
{
	Super::BeginPlay();
	// Note: Possess occurs after begin play.
}

// Called every frame
void AMController_Deer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


