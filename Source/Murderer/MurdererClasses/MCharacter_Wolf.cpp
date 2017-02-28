// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MCharacter_Wolf.h"

AMCharacter_Wolf::AMCharacter_Wolf()
{

	// Add tag to the AI which the actor can be recognized as.
	Tags.Add("Wolf");

}

void AMCharacter_Wolf::BeginPlay()
{
	Super::BeginPlay();
}

void AMCharacter_Wolf::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


