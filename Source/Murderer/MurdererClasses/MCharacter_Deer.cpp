// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MCharacter_Deer.h"

AMCharacter_Deer::AMCharacter_Deer()
{

	// Add tag to the AI which the actor can be recognized as.
	Tags.Add("Deer");

	
}

void AMCharacter_Deer::BeginPlay()
{
	Super::BeginPlay();
}

void AMCharacter_Deer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


