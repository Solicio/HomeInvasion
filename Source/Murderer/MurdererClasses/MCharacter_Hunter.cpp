// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MCharacter_Hunter.h"

AMCharacter_Hunter::AMCharacter_Hunter()
{

	// Add tag to the AI which the actor can be recognized as.
	Tags.Add("Hunter");

}

void AMCharacter_Hunter::BeginPlay()
{
	Super::BeginPlay();
}

void AMCharacter_Hunter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


