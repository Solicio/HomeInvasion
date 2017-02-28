// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MController_Wolf.h"


AMController_Wolf::AMController_Wolf()
{

	MurdererType = EMurdererType::EWolf;
}


void AMController_Wolf::BeginPlay()
{
	Super::BeginPlay();
	// Note: Possess occurs after begin play.
}

// Called every frame
void AMController_Wolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
