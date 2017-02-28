// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "SaveSystem.h"

#include "Miscellaneous/EnvironmentManager.h"
#include "SaveOverloads.h"

// Sets default values
ASaveSystem::ASaveSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerSaveFilePath = GetGameRootLocation() + "Save/PlayerSave.save";
}

// Called when the game starts or when spawned
void ASaveSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASaveSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PrintText(FString::FromInt(CreatedObjects.Num()), -1, FColor::Red, 0.0f);
}

bool ASaveSystem::SaveRequiredInfo(const FString& LevelToCont)
{
	// Store which level should be loaded from pressing continue
	LevelToContinue = LevelToCont;

	// Create buffer and send it to the save function
	RequiredInfoSaveFilePath = GetGameRootLocation() + "Save/RequiredInfo.save";

	FBufferArchive ToBinary;
	if (SaveRequiredInfoDataToFile(RequiredInfoSaveFilePath, ToBinary))
	{
		return true;
	}

	return false;
}

bool ASaveSystem::LoadRequiredInfo()
{
	RequiredInfoSaveFilePath = GetGameRootLocation() + "Save/RequiredInfo.save";

	// Load the data from file
	if (!LoadRequiredInfoDataFromFile())
	{
		return false;
	}

	return true;
}

bool ASaveSystem::SaveRequiredInfoDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary)
{
	// Fill the buffer with the variables
	SaveLoadDataRequiredInfo(ToBinary);

	// No Data
	if (ToBinary.Num() <= 0)
		return false;

	// Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		if (ShouldDebugPrint)
			PrintText("Save Success!", -1, FColor::Green, 2.0f);

		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	if (ShouldDebugPrint)
		PrintText("File could not be saved", -1, FColor::Green, 2.0f);

	return false;
}

bool ASaveSystem::LoadRequiredInfoDataFromFile()
{
	// Array to take the loaded data
	TArray<uint8> TheBinaryArray;

	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *RequiredInfoSaveFilePath))
	{
		if (ShouldDebugPrint)
			PrintText("FFILEHELPER:>> Invalid File", -1, FColor::Green, 2.0f);

		return false;
	}

	// Checking size
	if (ShouldDebugPrint)
		PrintText("Array Size RequiredInfo:" + FString::FromInt(TheBinaryArray.Num()), -1, FColor::Green, 2.0f);

	//File Load Error
	if (TheBinaryArray.Num() <= 0)
		return false;

	// Read the Data Retrieved by GFileManager
	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	SaveLoadDataRequiredInfo(FromBinary);

	// Clean up 
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
	FromBinary.Close();

	return true;
}

// Load/save function for RequiredInfo
void ASaveSystem::SaveLoadDataRequiredInfo(FArchive& Ar)
{
	Ar << LevelToContinue;
	
	Ar << WorldsSaved;
}

bool ASaveSystem::SavePlayer()
{
	// Gather data to be saved.

	AFPSCharacter* Character = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AFPSPlayerController* Controller = Cast<AFPSPlayerController>(Character->GetController());

	if (!Character || !Controller)
		return false;

	// General
	StressLevel = Character->StressLevel;
	EnergyPercentage = Character->EnergyPercentage;
	ColdPercentage = Character->ColdPercentage;

	// Blood corruption
	CorruptionLevel = Character->CorruptionLevel;
	Blood = Character->Blood;
	Bloodstones = Character->Bloodstones;
	BloodRushInfo = Character->BloodRushInfo;
	BloodDashInfo = Character->BloodDashInfo;
	BeastCallingInfo = Character->BeastCallingInfo;
	HunterVisionInfo = Character->HunterVisionInfo;
	VampiricHungerInfo = Character->VampiricHungerInfo;

	// Inventory
	InventorySlots.Empty();
	FInventoryItemSaveSlot NewItem;
	for (int i = 0; i < Character->Inventory.Num(); i++)
	{
		if (Character->Inventory[i] != nullptr)
		{
			NewItem.ArrayIndex = Character->Inventory[i]->Index;
			NewItem.ClassToSpawn = Character->Inventory[i]->GetClass()->GetName();
			InventorySlots.Add(NewItem);
		}
	}

	// Objectives
	CompletedObjectives = Character->CompletedObjectives;

	ObjectiveSlots.Empty();
	FObjectiveSaveSlot NewObjective;
	for (int i = 0; i < Character->CurrentObjectives.Num(); i++)
	{
		NewObjective.ObjectiveName = Character->CurrentObjectives[i]->ObjectiveName;
		NewObjective.CurrentSegmentName = Character->CurrentObjectives[i]->CurrentSegment.SegmentName;
		ObjectiveSlots.Add(NewObjective);
	}

	// Dialogues
	CompletedDialogues = Controller->PlayerHUDWidget->CompletedDialogues;

	// Brother
	TArray<AActor*> CollectedFriends;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIO_PlayerFriend::StaticClass(), CollectedFriends);
	if (CollectedFriends.Num() > 0)
	{
		AIO_PlayerFriend* Friend = Cast<AIO_PlayerFriend>(CollectedFriends[0]);
		if (Friend != nullptr)
		{
			BrotherStats.Health = Friend->Health;
			BrotherStats.ColdPercentage = Friend->ColdPercentage;
			BrotherStats.EnergyPercentage = Friend->EnergyPercentage;
		}
	}

	// Create buffer and send it to the save function
	PlayerSaveFilePath = GetGameRootLocation() + "Save/PlayerSave.save";

	FBufferArchive ToBinary;
	if(SavePlayerDataToFile(PlayerSaveFilePath, ToBinary))
	{
		return true;
	}

	return false;
}

bool ASaveSystem::LoadPlayer()
{
	PlayerSaveFilePath = GetGameRootLocation() + "Save/PlayerSave.save";

	// Load the data from file
	if (!LoadPlayerDataFromFile())
	{
		return false;
	}

	// Set the loaded data
	AFPSCharacter* Character = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AFPSPlayerController* Controller = Cast<AFPSPlayerController>(Character->GetController());

	if (!Character || !Controller)
		return false;

	// General
	Character->StressLevel = StressLevel;
	Character->EnergyPercentage = EnergyPercentage;
	Character->ColdPercentage = ColdPercentage;

	// Blood corruption
	Character->CorruptionLevel = CorruptionLevel;
	Character->Blood = Blood;
	Character->Bloodstones = Bloodstones;
	Character->BloodRushInfo = BloodRushInfo;
	Character->BloodDashInfo = BloodDashInfo;
	Character->BeastCallingInfo = BeastCallingInfo;
	Character->HunterVisionInfo = HunterVisionInfo;
	Character->VampiricHungerInfo = VampiricHungerInfo;

	// Reset the abilities stats to their current level
	Character->BPSetAbilityStats();

	// Inventory
	Character->ClearInventory();
	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		UClass* ItemClass = FindObject<UClass>(ANY_PACKAGE, *InventorySlots[i].ClassToSpawn);

		if(ItemClass != nullptr)
			Character->AddInventoryItemIndex(ItemClass, InventorySlots[i].ArrayIndex, true);

		//PrintText(FString::FromInt(InventorySlots[i].ArrayIndex), -1, FColor::Red, 2.0f);
		//PrintText(InventorySlots[i].ClassToSpawn, -1, FColor::Red, 2.0f);
	}

	// Objectives
	Character->CompletedObjectives = CompletedObjectives;
	// Current objectives set in BP

	// Dialogue
	Controller->PlayerHUDWidget->CompletedDialogues = CompletedDialogues;

	// Brother
	TArray<AActor*> CollectedFriends;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIO_PlayerFriend::StaticClass(), CollectedFriends);
	if (CollectedFriends.Num() > 0)
	{
		AIO_PlayerFriend* Friend = Cast<AIO_PlayerFriend>(CollectedFriends[0]);
		if (Friend != nullptr)
		{
			Friend->Health = BrotherStats.Health;
			Friend->ColdPercentage = BrotherStats.ColdPercentage;
			Friend->EnergyPercentage = BrotherStats.EnergyPercentage;
		}
	}

	return true;
}

bool ASaveSystem::SavePlayerDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary)
{
	// Fill the buffer with the variables
	SaveLoadDataPlayer(ToBinary);

	// No Data
	if (ToBinary.Num() <= 0) 
		return false;

	// Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		if(ShouldDebugPrint)
			PrintText("Save Success!", -1, FColor::Green, 2.0f);
		
		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	if(ShouldDebugPrint)
		PrintText("File could not be saved", -1, FColor::Green, 2.0f);

	return false;
}

bool ASaveSystem::LoadPlayerDataFromFile()
{
	// Array to take the loaded data
	TArray<uint8> TheBinaryArray;

	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *PlayerSaveFilePath))
	{
		if(ShouldDebugPrint)
			PrintText("FFILEHELPER:>> Invalid File", -1, FColor::Green, 2.0f);
		
		return false;
	}

	// Checking size
	if(ShouldDebugPrint)
		PrintText("Array Size Player:" + FString::FromInt(TheBinaryArray.Num()), -1, FColor::Green, 2.0f);

	//File Load Error
	if (TheBinaryArray.Num() <= 0) 
		return false;

	// Read the Data Retrieved by GFileManager
	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	SaveLoadDataPlayer(FromBinary);

	// Clean up 
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
	FromBinary.Close();

	return true;
}

void ASaveSystem::SaveLoadDataPlayer(FArchive& Ar)
{
	// Used for both saving and loading to ensure the the variables are loaded in the same order and when they were saved.
	// Might need to save the game version also, so saves don't break after patches. Use the version ID to make sure that it can still be loaded and resaved.

	// General
	Ar << StressLevel;
	Ar << EnergyPercentage;
	Ar << ColdPercentage;

	// Inventory
	Ar << InventorySlots;

	// Blood Corruption
	Ar << CorruptionLevel;
	Ar << Blood;
	Ar << Bloodstones;
	Ar << BloodRushInfo;
	Ar << BloodRushInfo;
	Ar << BloodDashInfo;
	Ar << BeastCallingInfo;
	Ar << HunterVisionInfo;
	Ar << VampiricHungerInfo;

	// Objectives
	Ar << CompletedObjectives;
	Ar << ObjectiveSlots;

	// Dialogue
	Ar << CompletedDialogues;

	// Brother
	Ar << BrotherStats;

	//Ar << WorldsSaved;
}

//--------------------- World ---------------------//

bool ASaveSystem::SaveMap(const FString& MapName)
{
	AFPSCharacter* Character = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AFPSPlayerController* Controller = Cast<AFPSPlayerController>(Character->GetController());

	if (!Character || !Controller)
		return false;

	// Player Location
	PlayerLocation = Character->GetActorLocation();
	PlayerRotation = Controller->GetControlRotation();

	// Actors that have been created during play
	TArray<AActor*> CollectedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "CreatedDuringPlay", CollectedActors);
	
	FObjectCreatedDuringPlay NewCreatedObject;
	CreatedObjects.Empty();
	for (auto Itr(CollectedActors.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			if ((*Itr)->ActorHasTag("Interactable"))
			{
				AInteractableObject* InterObject = Cast<AInteractableObject>((*Itr));
				NewCreatedObject.ClassToSpawn = InterObject->GetClass()->GetName();

				FVector SpawnLoc = InterObject->Mesh->GetComponentLocation() + (FVector::UpVector * 25.0f);

				NewCreatedObject.Transform.SetLocation(SpawnLoc);
				NewCreatedObject.Transform.SetRotation(InterObject->Mesh->GetComponentQuat());
				CreatedObjects.Add(NewCreatedObject);
			}
			else
			{
				// Need to make sure this is going to work for any classes that are not Interactable Objects
				NewCreatedObject.ClassToSpawn = (*Itr)->GetClass()->GetName();
				NewCreatedObject.Transform.SetLocation((*Itr)->GetActorLocation());
				NewCreatedObject.Transform.SetRotation((*Itr)->GetActorRotation().Quaternion());
				CreatedObjects.Add(NewCreatedObject);
			}
		}
	}

	// Time
	CollectedActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvironmentManager::StaticClass(), CollectedActors);
	if (CollectedActors.Num() > 0)
	{
		Time = Cast<AEnvironmentManager>(CollectedActors[0])->GetTime();
	}

	// Weather
	GetWeather();

	// Generators
	Generators.Empty();
	GetGenerators();

	// Pickup object
	CollectedActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIO_PickupObject::StaticClass(), CollectedActors);

	FPickupObjectSaveSlot NewObjectSlot;
	PickupObjects.Empty();
	for (auto Iter(CollectedActors.CreateIterator()); Iter; Iter++)
	{
		if ((*Iter) != nullptr)
		{
			AIO_PickupObject* PickupObject = Cast<AIO_PickupObject>((*Iter));
			NewObjectSlot.GUID = PickupObject->GUID;

			FVector NewLoc = PickupObject->Mesh->GetComponentLocation() + (FVector::UpVector * 25.0f);

			NewObjectSlot.Transform.SetLocation(NewLoc);
			NewObjectSlot.Transform.SetRotation(PickupObject->Mesh->GetComponentQuat());
			PickupObjects.Add(NewObjectSlot);
		}
	}

	// Doors
	CollectedActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIO_Door::StaticClass(), CollectedActors);

	FDoorSaveSlot NewDoorSlot;
	Doors.Empty();
	for (auto Itere(CollectedActors.CreateIterator()); Itere; Itere++)
	{
		if ((*Itere) != nullptr)
		{
			AIO_Door* DoorObject = Cast<AIO_Door>((*Itere));
			NewDoorSlot.Rotation = DoorObject->GetDoorRotation();
			NewDoorSlot.Locked = DoorObject->Locked;
			NewDoorSlot.GUID = DoorObject->GUID;
			Doors.Add(NewDoorSlot);
		}
	}

	// Save Zones
	SaveZones.Empty();
	GetSaveZones();

	// Level Phase
	AHomeInvasionGameMode* GameMode = Cast<AHomeInvasionGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		CurrentLevelPhase = GameMode->GetCurrentPhase();
	}

	// Create buffer and send it to the save function
	//FString FullPath = WorldSaveFilePath + MapName + ".save";
	FString FullPath = GetGameRootLocation() + "Save/" + MapName + ".save";

	// Add the world to the array to be saved.
	if (!WorldsSaved.Contains(MapName))
		WorldsSaved.Add(MapName);

	FBufferArchive ToBinary;
	if (SaveWorldDataToFile(FullPath, ToBinary))
	{
		return true;
	}

	return false;
}

bool ASaveSystem::LoadMap(const FString& MapName)
{
	// Load the data from file
	//FString FullPath = WorldSaveFilePath + MapName + ".save";
	FString FullPath = GetGameRootLocation() + "Save/" + MapName + ".save";
	if (!LoadWorldDataFromFile(FullPath))
	{
		return false;
	}

	AFPSCharacter* Character = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AFPSPlayerController* Controller = Cast<AFPSPlayerController>(Character->GetController());

	if (!Character || !Controller)
		return false;

	// Player Location
	Character->SetActorLocation(PlayerLocation);
	Controller->SetControlRotation(PlayerRotation);

	// Destroy objects that were saved to be destroyed
	TArray<AActor*> Objects;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractableObject::StaticClass(), Objects);

	for (auto Itr(Objects.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			AInteractableObject* Object = Cast<AInteractableObject>((*Itr));
			if (Object != nullptr && ObjectsToBeDestroyed.Contains(Object->GUID))
			{
				Object->Destroy();
			}
		}
	}

	// Create the objects that were dynamically created before, also add the needed tag.
	for (auto Iter(CreatedObjects.CreateIterator()); Iter; Iter++)
	{
		UClass* ActorClass = FindObject<UClass>(ANY_PACKAGE, *(*Iter).ClassToSpawn);
		if (ActorClass != nullptr)
		{
			AInteractableObject* InterObject = SpawnBP<AInteractableObject>(GetWorld(), ActorClass, (*Iter).Transform.GetLocation(), (*Iter).Transform.GetRotation().Rotator());
			InterObject->Tags.Add("CreatedDuringPlay");
			InterObject->Mesh->SetWorldRotation((*Iter).Transform.GetRotation());
		}
	}

	// Time
	TArray<AActor*> CollectedActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvironmentManager::StaticClass(), CollectedActors);
	if (CollectedActors.Num() > 0)
	{
		Cast<AEnvironmentManager>(CollectedActors[0])->SetTime(Time);
	}

	// Weather
	SetWeather(Weather);

	// Generators
	SetGenerators();

	// Pickup Objects
	CollectedActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIO_PickupObject::StaticClass(), CollectedActors);

	// Loop through the actors in the world
	for (auto Iter(CollectedActors.CreateIterator()); Iter; Iter++)
	{
		if ((*Iter) != nullptr)
		{
			// Cast the actor to the right class
			AIO_PickupObject* Object = Cast<AIO_PickupObject>((*Iter));

			// Loop through the loaded array of structs to find a GUID match
			for (int i = 0; i < PickupObjects.Num(); i++)
			{
				if (Object->GUID == PickupObjects[i].GUID)
				{
					// Only move if the new location is not too close to the original position
					FVector TestLoc = Object->Mesh->GetComponentLocation() + (FVector::UpVector * 25.0f);
					if (!TestLoc.Equals(PickupObjects[i].Transform.GetLocation(), 5.0f))
					{
						Object->Mesh->SetWorldLocation(PickupObjects[i].Transform.GetLocation());
						Object->Mesh->SetWorldRotation(PickupObjects[i].Transform.GetRotation());
						//PrintText("Set Location - " + Object->GetName(), -1, FColor::Red, 2.0f);
					}
				}
			}
		}
	}

	// Doors
	CollectedActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIO_Door::StaticClass(), CollectedActors);

	for (auto Itere(CollectedActors.CreateIterator()); Itere; Itere++)
	{
		if ((*Itere) != nullptr)
		{
			AIO_Door* DoorObject = Cast<AIO_Door>((*Itere));
			
			for (int j = 0; j < Doors.Num(); j++)
			{
				if (DoorObject->GUID == Doors[j].GUID)
				{
					if (!DoorObject->GetDoorRotation().Equals(Doors[j].Rotation, 1.0f))
					{
						DoorObject->SetDoorRotation(Doors[j].Rotation);
						//PrintText("Set Rotation - " + DoorObject->GetName(), -1, FColor::Red, 2.0f);
					}
					DoorObject->Locked = Doors[j].Locked;
				}
			}
		}
	}

	// Save Zones
	SetSaveZones();

	// Add the path to this save file. Should already be added, but to be sure that it's there.
	//if (!WorldSaveFilePaths.Contains(FullPath))
	//	WorldSaveFilePaths.Add(FullPath);

	return true;
}

bool ASaveSystem::SaveWorldDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary)
{
	// Fill the buffer with the variables
	SaveLoadDataWorld(ToBinary);

	// No Data
	if (ToBinary.Num() <= 0)
		return false;

	// Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		if (ShouldDebugPrint)
			PrintText("Save Success!", -1, FColor::Green, 2.0f);

		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	if (ShouldDebugPrint)
		PrintText("File could not be saved", -1, FColor::Green, 2.0f);

	return false;
}

bool ASaveSystem::LoadWorldDataFromFile(const FString& FullFilePath)
{
	// Array to take the loaded data
	TArray<uint8> TheBinaryArray;

	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		if (ShouldDebugPrint)
			PrintText("FFILEHELPER:>> Invalid File", -1, FColor::Green, 2.0f);

		return false;
	}

	// Checking size
	if (ShouldDebugPrint)
		PrintText("Array Size World:" + FString::FromInt(TheBinaryArray.Num()), -1, FColor::Green, 2.0f);

	//File Load Error
	if (TheBinaryArray.Num() <= 0)
		return false;

	// Read the Data Retrieved by GFileManager
	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	SaveLoadDataWorld(FromBinary);

	// Clean up 
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
	FromBinary.Close();

	return true;
}

void ASaveSystem::SaveLoadDataWorld(FArchive& Ar)
{
	Ar << PlayerLocation;
	Ar << PlayerRotation;

	Ar << ObjectsToBeDestroyed;

	Ar << CreatedObjects;

	Ar << Time;
	Ar << Weather;

	Ar << Generators;

	Ar << PickupObjects;

	Ar << Doors;

	Ar << SaveZones;

	Ar << CurrentLevelPhase;
}

//--------------------- General ---------------------//

void ASaveSystem::AddObjectToBeDestroyed(FGuid Guid)
{
	if (!ObjectsToBeDestroyed.Contains(Guid))
	{
		ObjectsToBeDestroyed.Add(Guid);
	}
}

void ASaveSystem::DestroyDynamicObjects()
{
	TArray<AActor*> CreatedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "CreatedDuringPlay", CreatedActors);

	for (auto Itr(CreatedActors.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			(*Itr)->Destroy();
		}
	}
}

bool ASaveSystem::ValidRequiredInfoSave()
{
	RequiredInfoSaveFilePath = GetGameRootLocation() + "Save/RequiredInfo.save";
	return IFileManager::Get().FileExists(*RequiredInfoSaveFilePath);
}

bool ASaveSystem::ValidPlayerSave()
{
	PlayerSaveFilePath = GetGameRootLocation() + "Save/PlayerSave.save";
	return IFileManager::Get().FileExists(*PlayerSaveFilePath);
}

bool ASaveSystem::ValidWorldSave(const FString& MapName)
{
	//FString FullPath = WorldSaveFilePath + MapName + ".save";
	FString FullPath = GetGameRootLocation() + "Save/" + MapName + ".save";

	if(ShouldDebugPrint)
		PrintText(FullPath, -1, FColor::Green, 2.0f);
	
	return IFileManager::Get().FileExists(*FullPath);
}

bool ASaveSystem::ClearSave()
{
	bool FailedToDelete = false;
	for (int i = 0; i < WorldsSaved.Num(); i++)
	{
		FString MapSaveLocation = GetGameRootLocation() + "Save/" + WorldsSaved[i] + ".save";
		if (!IFileManager::Get().Delete(*MapSaveLocation))
			FailedToDelete = true;
	}

	WorldsSaved.Empty();

	PlayerSaveFilePath = GetGameRootLocation() + "Save/PlayerSave.save";
	RequiredInfoSaveFilePath = GetGameRootLocation() + "Save/RequiredInfo.save";
	return IFileManager::Get().Delete(*PlayerSaveFilePath) && IFileManager::Get().Delete(*RequiredInfoSaveFilePath) && !FailedToDelete;
}


FString ASaveSystem::GetGameRootLocation()
{
	return FPaths::ConvertRelativePathToFull(FPaths::GameDir());
}

