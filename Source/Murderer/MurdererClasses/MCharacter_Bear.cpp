// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MCharacter_Bear.h"

AMCharacter_Bear::AMCharacter_Bear()
{

	// Add tag to the AI which the actor can be recognized as.
	Tags.Add("Bear");

}

void AMCharacter_Bear::BeginPlay()
{
	Super::BeginPlay();
}

void AMCharacter_Bear::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}



