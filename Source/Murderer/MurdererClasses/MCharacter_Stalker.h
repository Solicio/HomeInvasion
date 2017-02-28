// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Murderer/MurdererCharacter.h"
#include "MCharacter_Stalker.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AMCharacter_Stalker : public AMurdererCharacter
{
	GENERATED_BODY()

public:

	AMCharacter_Stalker();

protected:

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;


};