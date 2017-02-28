// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MCharacter_Stalker.h"

AMCharacter_Stalker::AMCharacter_Stalker()
{

	// Add tag to the AI which the actor can be recognized as.
	Tags.Add("Stalker");

}

void AMCharacter_Stalker::BeginPlay()
{
	Super::BeginPlay();
}

void AMCharacter_Stalker::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


