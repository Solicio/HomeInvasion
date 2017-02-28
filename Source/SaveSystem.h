// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerCharacter/FPSCharacter.h"
#include "InteractableObjects/IO_PlayerFriend.h"

#include "GameFramework/Actor.h"
#include "SaveSystem.generated.h"

/** Struct of variables needed to save and load objects that were created during play. */
USTRUCT(BlueprintType)
struct FObjectCreatedDuringPlay
{
	GENERATED_BODY()

	// What class the item was
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString ClassToSpawn;

	// What location and rotation the object had.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FTransform Transform;

	FObjectCreatedDuringPlay()
	{
		ClassToSpawn = "";
		Transform = FTransform();
	}
};

/** Struct of the generators most important variables */
USTRUCT(BlueprintType)
struct FGeneratorSaveSlot
{
	GENERATED_BODY()

	// The generators GUID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FGuid GUID;

	// If the generator was on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	bool GeneratorOn;

	// How much fuel was in the generator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float FuelLevel;
};

/** Struct of the save zone, used to store if the player has saved there */
USTRUCT(BlueprintType)
struct FSaveZoneSaveSlot
{
	GENERATED_BODY()

	// The zone GUID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FGuid GUID;

	// If the player has saved here
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	bool SavedHere;
};

UCLASS()
class HOMEINVASION_API ASaveSystem : public AActor
{
	GENERATED_BODY()
	
	public:	

		/** Sets default values for this actor's properties */
		ASaveSystem();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System")
		bool ShouldDebugPrint;


		//--- RequiredInfo Save File ---//

		/** Save the SavedWorlds and level to continue variables to the disk 
		*	This is used by the main menu for clearing save files and loading the correct level when pressing continue
		*/
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool SaveRequiredInfo(const FString& LevelToCont);

		/** Load and set the saved RequiredInfo variables */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool LoadRequiredInfo();

		// What level the game should load when pressing continue in the main menu. Takes the current world when saving.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save System")
		FString LevelToContinue;

		// Location where the file is being saved, requires full path
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
		FString RequiredInfoSaveFilePath;

		bool SaveRequiredInfoDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary);
		bool LoadRequiredInfoDataFromFile();
		void SaveLoadDataRequiredInfo(FArchive& Ar);


		//--- Player ---//

		/** Save the player variables to the disk
		*	Transform, Inventory, Blood Corruption, Dialogue, General Stats, Objectives, Brother Stats.
		*/
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool SavePlayer();

		/** Load and set the saved player variables */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool LoadPlayer();

		// Location where the file is being saved, requires full path
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
		FString PlayerSaveFilePath;

		bool SavePlayerDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary);
		bool LoadPlayerDataFromFile();
		void SaveLoadDataPlayer(FArchive& Ar);

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save System")
		TArray<FObjectiveSaveSlot> ObjectiveSlots;


		//--- World ---//

		/** Save the specified maps variables
		*	Interactable Objects, World Events, Weather, Doors, etc.
		*/
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool SaveMap(const FString& MapName);

		/** Load and set the saved world variables */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool LoadMap(const FString& MapName);

		//// Location where the file is being saved, excluding the file name.
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System")
		//FString WorldSaveFilePath;

		// Stores all the paths to the different world save files. Saved in the RequiredInfo file.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
		TArray<FString> WorldsSaved;
		//TArray<FString> WorldSaveFilePaths;


		/*	Array containing the Guids of the objects that are in the world that should be destroyed.
		*	Get all interactable objects by class and look through to find matching Guids and destroy.
		*	This array will be saved to disk and then loaded up on game reloads to know which objects should be deleted.
		*/
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
		TArray<FGuid> ObjectsToBeDestroyed;

		// Add object into the array to be destroyed when loading the game.
		UFUNCTION(BlueprintCallable, Category = "Save System")
		void AddObjectToBeDestroyed(FGuid Guid);


		/** Get the current weather in the world. This function sets the Weather variable */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void GetWeather();

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save System")
		FString Weather;

		/** Set the current weather in the world. */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void SetWeather(const FString& InWeather);


		/** Gets all the generators and then fills the Generators array with info */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void GetGenerators();

		/** Sets all the generators with the generator array */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void SetGenerators();

		// Stores all the generators value when saving and loading.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save System")
		TArray<FGeneratorSaveSlot> Generators;


		/** Gets all the save zones and then fills the save zone array with info */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void GetSaveZones();

		/** Sets all the save zones with the save zones array */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void SetSaveZones();

		// Stores all the save zones values when saving and loading.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save System")
		TArray<FSaveZoneSaveSlot> SaveZones;

		// The current phase the level is on.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save System")
		FString CurrentLevelPhase;


		bool SaveWorldDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary);
		bool LoadWorldDataFromFile(const FString& FullFilePath);
		void SaveLoadDataWorld(FArchive& Ar);


		//--- General ---//

		/** Should be called after calling the load functions, will call update functions etc. */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save System")
		void PostLoadCall(bool PlayerLoaded, bool WorldLoaded);

		/** Checks path for the correct file */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool ValidRequiredInfoSave();

		/** Checks path for the correct file */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool ValidPlayerSave();

		/** Checks path for the correct file */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool ValidWorldSave(const FString& MapName);

		/** Delets any save files */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		bool ClearSave();

		/**	This function destroys all objects that has the tag CreatedDuringPlay. 
		*	This will be needed when doing a reload with restarting the map. So there are no duplicates.
		*/
		UFUNCTION(BlueprintCallable, Category = "Save System")
		void DestroyDynamicObjects();

		/** Gets the games root directory on the disk */
		UFUNCTION(BlueprintCallable, Category = "Save System")
		FString GetGameRootLocation();

	protected:

		//--- Player ---//

		// General
		float StressLevel;
		float EnergyPercentage;
		float ColdPercentage;

		// Blood Corruption
		float CorruptionLevel;
		float Blood;
		int32 Bloodstones;
		FBloodCorruptionAbilityInfo BloodRushInfo;
		FBloodCorruptionAbilityInfo BloodDashInfo;
		FBloodCorruptionAbilityInfo BeastCallingInfo;
		FBloodCorruptionAbilityInfo HunterVisionInfo;
		FBloodCorruptionAbilityInfo VampiricHungerInfo;

		// Inventory
		TArray<FInventoryItemSaveSlot> InventorySlots;

		// Objectives
		TArray<FName> CompletedObjectives;
		
		// Dialogue
		TArray<FName> CompletedDialogues;

		// Brother
		FFriendSaveSlot BrotherStats;


		//--- World ---//
		
		FVector PlayerLocation;
		FRotator PlayerRotation;

		TArray<FObjectCreatedDuringPlay> CreatedObjects;
		TArray<FPickupObjectSaveSlot> PickupObjects;
		TArray<FDoorSaveSlot> Doors;

		float Time;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

};
