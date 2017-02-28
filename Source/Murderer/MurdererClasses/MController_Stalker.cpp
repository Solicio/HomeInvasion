// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MController_Stalker.h"


AMController_Stalker::AMController_Stalker()
{

	MurdererType = EMurdererType::EStalker;
}


void AMController_Stalker::BeginPlay()
{
	Super::BeginPlay();
	// Note: Possess occurs after begin play.
}

// Called every frame
void AMController_Stalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

