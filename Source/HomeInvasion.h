// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef __HOMEINVASION_H__
#define __HOMEINVASION_H__

#include "Engine.h"

//#define print(text, key, color) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 0.5f, color, text)

/** Keys: AI: 1-20, Player: 21-40, Interactable Objects: 41-50, Misc: 51-60 */
FORCEINLINE void PrintText(FString Text, int32 Key, FColor Color = FColor::White, float Duration = 0.5f)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Text);
	}
}

template <typename ASpawnBP>
FORCEINLINE ASpawnBP* SpawnBP(
	UWorld* TheWorld,
	UClass* TheBP,
	const FVector& Loc,
	const FRotator& Rot,
	const ESpawnActorCollisionHandlingMethod bNoCollisionFail = ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
	AActor* Owner = NULL,
	APawn* Instigator = NULL
	)
{
	if (!TheWorld) return NULL;
	if (!TheBP) return NULL;
	//~~~~~~~~~~~

	FActorSpawnParameters SpawnInfo;
	/*SpawnInfo.bNoCollisionFail = bNoCollisionFail;*/
	SpawnInfo.SpawnCollisionHandlingOverride = bNoCollisionFail;
	SpawnInfo.Owner = Owner;
	SpawnInfo.Instigator = Instigator;
	SpawnInfo.bDeferConstruction = false;

	return TheWorld->SpawnActor<ASpawnBP>(TheBP, Loc, Rot, SpawnInfo);
}

#endif
