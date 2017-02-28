// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerCharacter/FPSCharacter.h"
#include "InteractableObjects/InventoryItems/InventoryItem.h"
//#include "InteractableObjects/InteractableObject.h"
#include "SaveSystem.h"

FORCEINLINE FArchive& operator<<(FArchive& Ar, FBloodCorruptionAbilityInfo& AbilityInfo)
{
	Ar << AbilityInfo.Unlocked;
	Ar << AbilityInfo.CurrentAbilityCooldown;
	Ar << AbilityInfo.AbilityLevel;
	Ar << AbilityInfo.CostMultiplier;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FInventoryItemSaveSlot& InventoryItem)
{
	Ar << InventoryItem.ClassToSpawn;
	Ar << InventoryItem.ArrayIndex;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FObjectiveSaveSlot& Objective)
{
	Ar << Objective.ObjectiveName;
	Ar << Objective.CurrentSegmentName;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FObjectCreatedDuringPlay& Object)
{
	Ar << Object.ClassToSpawn;
	Ar << Object.Transform;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FFriendSaveSlot& Friend)
{
	Ar << Friend.Health;
	Ar << Friend.EnergyPercentage;
	Ar << Friend.ColdPercentage;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FGeneratorSaveSlot& Generator)
{
	Ar << Generator.GUID;
	Ar << Generator.GeneratorOn;
	Ar << Generator.FuelLevel;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FPickupObjectSaveSlot& Object)
{
	Ar << Object.GUID;
	Ar << Object.Transform;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FDoorSaveSlot& Door)
{
	Ar << Door.Rotation;
	Ar << Door.Locked;
	Ar << Door.GUID;

	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FSaveZoneSaveSlot& Zone)
{
	Ar << Zone.GUID;
	Ar << Zone.SavedHere;

	return Ar;
}

/**
 * 
 */
class HOMEINVASION_API SaveOverloads
{
	public:
		SaveOverloads();
		~SaveOverloads();


	public:

};

