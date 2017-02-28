// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AISense_Hearing.h"

#include "HomeInvasionGameMode.h"

#include "PlayerVisiblityPoints.h"
#include "FPSPlayerController.h"

#include "../InteractableObjects/InteractableObject.h"
#include "../InteractableObjects/IO_Door.h"
#include "../InteractableObjects/IO_PickupObject.h"
#include "../InteractableObjects/InventoryItems/InventoryItem.h"

#include "../Miscellaneous/TemperatureZone.h"

#include "Phone/PlayerPhone.h"

#include "HidingSpot/PlayerHidingSpot.h"

#include "ObjectiveData.h"

#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

/** Used to keep track of the current movement state the player is in.*/
UENUM(BlueprintType)
enum class EPlayerMovementState
{
	EStandingStill,
	EStandingStillCrouched,
	EWalking,
	EWalkingSlow,
	EWalkingCrouched,
	ESprinting,
	EFalling
};

/** All the different types of step sound override in the game. */
UENUM(BlueprintType)
enum class EStepSoundOverride
{
	ENone,
	EWater,
	EGlass,
	ESticks
};

/** General information for all the blood corruption abilities */
USTRUCT(BlueprintType)
struct FBloodCorruptionAbilityInfo
{
	GENERATED_BODY()

	// If the ability is unlocked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	bool Unlocked;

	// How long before the player can use the ability again
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Struct")
	float AbilityCooldown;

	// Used for counting down to 0 again so the ability can be used.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Struct")
	float CurrentAbilityCooldown;

	// How much the ability has been ranked up.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	int32 AbilityLevel;

	// How much the ability cost to use
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Struct")
	float Cost;

	// How much corruption the ability gives the player.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Struct")
	float CorruptionIncrease;

	// Multiplied by the cost for the total value to use the ability, used for increasing cost.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float CostMultiplier;

	FBloodCorruptionAbilityInfo()
	{
		Unlocked = true;
		AbilityCooldown = 5.0f;
		CurrentAbilityCooldown = 0.0f;
		AbilityLevel = 1;
		Cost = 10.0f;
		CostMultiplier = 1.0f;
		CorruptionIncrease = 5.0f;
	}
};

/** Struct used when saving the inventory, stores which class the item was and at what location. */
USTRUCT(BlueprintType)
struct FInventoryItemSaveSlot
{
	GENERATED_BODY()

	// What class the item was
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString ClassToSpawn;

	// Which index in the array the item had
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	int32 ArrayIndex;

	FInventoryItemSaveSlot()
	{
		ClassToSpawn = "";
		ArrayIndex = 0;
	}
};

UCLASS()
class HOMEINVASION_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	public:

		// Sets default values for this character's properties 
		AFPSCharacter(const FObjectInitializer& ObjectInitializer);

		// First person camera
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* FirstPersonCameraComponent;

		/** Camera boom positioning the camera beside the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* CameraBoom;

		// Mesh used for shadows and player visibility points
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent*	ShadowMesh;

		// Mesh used for shadows and player visibility points
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics Handle")
		UPhysicsHandleComponent* PickupPhysicsHandle;


		// Ignored by AI
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool ShouldBeIgnored;

		// If the AI can kill the player
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool CanBeKilled;

		// Is the player dead / dying. To have the AI stop chasing.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool IsDead;

		// If the player is inside a light zone.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool InLightZone;

		// Set to true when the player has fallen down using ragdoll physics when dying
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		bool FallingDead;

		/** Used to kill the player, sets so that the character does not take input and makes the character fall over*/
		UFUNCTION(BlueprintCallable, Category = "General")
		void KillPlayer(bool ShouldFallDown, float FallOverDelay);

		/** Used to have the players skeletal mesh go ragdoll and have the player die*/
		UFUNCTION(BlueprintCallable, Category = "General")
		void FallOver();


		//------ Stress Variables ---------//

		/*	The stress level of the player.
		*	Stress is increased by situations where the player is making alot of noise or when the player is close to the Murderer
		*	Stress makes various GUI elements appear, makes the breathing louder, etc.
		*/ 
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stress")
		float StressLevel;

		/** Adds the value to the Stress Level. Positive or negative */
		UFUNCTION(BlueprintCallable, Category = "Stress")
		void AddToStressLevel(float InValue);


		//------ Cold Variables ---------//

		// On/Off variable
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		bool ManageColdPercentage;

		/*	The cold level of the player.
		*	Goes from 100%->0%, 100% = Warm, 0% = Dead
		*	Cold is decreased normally, faster depending on certain variables
		*	Cold increases in certain temperature zones
		*	Cold affects the players controls/speed
		*	The cold percentage is updated every one second
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float ColdPercentage;

		// Normal decrease value that is used during the day
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float NormalDecrease;

		// Nightly decrease value that is used during the night, 18:00->6:00
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float NightDecrease;

		// How much energy is low energy, any value under this is considered low
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float LowEnergy;

		// How much cold should decrease by every second when on low energy
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float LowEnergyDecrease;

		// If the player has wet clothes
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		bool HasWetClothes;

		// How much should be subtracted because of wet clothes each second
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float WetClothesDecrease;

		// What temperature zones values the player is currently in. Checked every one second.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cold")
		float TemperatureZoneValue;

		// The multiplier applied to the movementspeed while below 30% cold
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cold")
		float ColdMovementMulti;

		/** Adds the value to the Cold Level. Positive or negative */
		UFUNCTION(BlueprintCallable, Category = "Cold")
		void AddToColdPercentage(float InValue);


		//------ Energy Variables ---------//

		// On/Off variable
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		bool ManageEnergyPercentage;

		/*	The energy level of the player.
		*	Goes from 100%->-10%, 100% = high energy, -10% = Dead
		*	The player energy decreases when idling, decreases quicker when moving.
		*	When on low energy, gets cold quicker.
		*	Can be refilled by eating food
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float EnergyPercentage;

		// Value to decrease each second when player is idle
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float IdleDecrease;

		// Value to decrease each second when player is walking slowly
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float WalkingSlowDecrease;

		// Value to decrease each second when player is walking
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float WalkingDecrease;

		// Value to decrease each second when player is running
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float RunningDecrease;

		/** Adds the value to the Energy Level. Positive or negative */
		UFUNCTION(BlueprintCallable, Category = "Energy")
		void AddToEnergyPercentage(float InValue);


		//------ Breathing Variables ---------//

		// If the player should take breaths
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		bool ShouldBreathe;

		// The standard time between each breath. The original value, this value will not be changed during play.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float MaxBreathDelay;
		
		// The current time between each breath. This value is dependant on the MaxBreathDelay value together with the stress level.
		// Will change during play.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Breathing")
		float CurMaxBreathDelay;

		// The value that is reduced and reset when taking breaths
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float BreathDelay;

		// The strength of the noise made by each breath. Noise = 0.0-1.0. HearingAggro increase = (Strength * 10.0f)
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float BreathNoiseStrength;


		//------ Movement Variables ---------//

		// Keeps track of the players current movementstate
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Movement")
		TEnumAsByte<EPlayerMovementState> PlayerMovementState;

		// The strength of the noise made by each normal step. Noise = 0.0-1.0. HearingAggro increase = (Strength * 10.0f)
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float WalkingNoiseStrength;

		// The strength of the noise made by each slow walking step. Noise = 0.0-1.0. HearingAggro increase = (Strength * 10.0f)
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float SlowWalkingNoiseStrength;

		// The strength of the noise made by each sprinting step. Noise = 0.0-1.0. HearingAggro increase = (Strength * 10.0f)
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float SprintingNoiseStrength;

		// Time spent falling. Used for killing player when landing or playing a special animation.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Movement")
		float FallingDuration;

		// If the player has been falling for more then this value, the player will die.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float FallingDeathLimit;

		// Keeps track of if the player is in a override zone for step sounds.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		TEnumAsByte<EStepSoundOverride> StepOverride;

		// How much the noise made from the step should be increased.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float StepOverrideNoiseMulti;


		// ------ Visibility ------ //

		// Used for spawning the visibility BP
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visibility")
		UClass* VisibilityPointBP;

		// Amount of visibility points
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visibility")
		int32 VisibilityPointsNum;

		// Array of the visibility points the character has. 
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visibility")
		TArray<APlayerVisiblityPoints*> VisibilityPoints;

		// If the player is currently inside a hidingspot
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
		bool InsideHidingSpot;

		// The current hidingspot the player is inside
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
		APlayerHidingSpot* CurrentHidingSpot;


		// ------ Phone ------ //

		// If the player has picked up the phone. Needed to bring out the phone to look at.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone")
		bool HasPhone;

		// If the player has the phone out infront of the player
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone")
		bool HasPhoneOut;

		// If the player should spawn with the phone
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Phone")
		bool StartWithPhone;

		// Used for spawning the phone blueprint class
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Phone")
		TSubclassOf <APlayerPhone> PhoneClassBP;

		// The actual phone the player has picked up. nullptr if the player has no picked it up.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phone")
		APlayerPhone* PlayerPhone;

		/** Have the player create the phone and set neccesary variables */
		UFUNCTION(BlueprintCallable, Category = "Phone")
		void PickUpPhone();

		/** Used to bring out and take back the phone */
		//UFUNCTION(BlueprintCallable, Category = "Phone")
		//void TakeOutPhone(bool ForceDown);

		/** Used to bring out and take back the phone */
		UFUNCTION(BlueprintCallable, Category = "Phone")
		void SetPhoneActive(bool NewActive, bool ForceDown);


		// ------ PickupObjects ------ //
		
		// If the player is allowed to pick up objects
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupObjects")
		bool CanPickUp;

		// How far away from the camera the object should be
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupObjects")
		float DistanceFromPlayer;

		// How far down the object should be relative to the camera location.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PickupObjects")
		float CameraDownZ;

		// How far the object can go away from the player before it is forced to let go
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupObjects")
		float MaxDistanceFromPlayer;

		// How far away from the camera the object should be
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupObjects")
		float ThrowForce;

		// If the player is allowed to pick up objects
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PickupObjects")
		bool HoldingObject;

		// If the player is allowed to pick up objects
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PickupObjects")
		AIO_PickupObject* HeldObject;

		/** Have the player pick up the sent in actor */
		UFUNCTION(BlueprintCallable, Category = "PickupObjects")
		void PickUpObject(AActor* PickupObject);

		/** Let go of the current picked up object */
		UFUNCTION(BlueprintCallable, Category = "PickupObjects")
		void LetGoObject(bool ShouldThrow);


		// ------ Inventory ------ //

		// Array of the items in the inventory
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		TArray<AInventoryItem*> Inventory;

		/** Empties the players inventory */
		UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ClearInventory();

		/** Used to add items to the inventory */
		UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddInventoryItem(UClass* InventoryItemBP, bool SuppressText = false);

		/** Used to add items to the inventory */
		UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddInventoryItemIndex(UClass* InventoryItemBP, int32 ArrayIndex, bool SuppressText = false);

		/** Remove items from the inventory */
		UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveInventoryItem(AInventoryItem* Item);

		/** Removes the first instance of item with specified name */
		UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveInventoryItemByName(FString Name);

		/** Used to check the inventory array for items with the right name */
		UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool HasItem(FString ItemName);


		// ------ Objectives ------ //
		
		// The current objectives the player has.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objectives")
		TArray<UObjectiveData*> CurrentObjectives;

		// Which objectives have been completed
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objectives")
		TArray<FName> CompletedObjectives;

		// The current highlighted objective, the objective to show on the map and icon for in the world.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objectives")
		UObjectiveData* HighlightedObjective;

		/** Adds a new objective into the array. */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		void AddObjective(UObjectiveData* Objective, bool SuppressText = false);

		/** Adds a new objective into the array of completed objectives. */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		void AddCompletedObjective(FName ObjectiveName);

		/** 
		*	Called after adding/removing an objective. Checks if the current highlighted objective is the most important one. 
		*	If it's not, set the most important objective to the highlighted one.
		*/
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		void ReCheckHighlightedObjective();

		/** Sets the current segment on a objective. */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		void SetObjectiveSegment(FString ObjectiveName, FString SegmentName, bool SuppressText = false);

		/**  Check if the player currently has a certain objective in it's array. */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		bool HasObjective(FString ObjectiveName);

		/** Check if the segment provided is the active one on the objective provided. */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		bool IsOnSegment(FString ObjectiveName, FString SegmentName);

		/** Removes a objective from the array */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		void RemoveObjectiveRef(UObjectiveData* Objective);

		/** Removes a objective from the array */
		UFUNCTION(BlueprintCallable, Category = "Objectives")
		void RemoveObjectiveName(FString ObjectiveName);


		// ------ Visible Enemies ------ //

		// Stores references to all the murderers that can be seen by the player.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visible Enemies")
		TArray<AMurdererCharacter*> VisibleMurderers;

		/** Checks if a provided murderer is in view of the player */
		UFUNCTION(BlueprintCallable, Category = "Visible Enemies")
		bool IsMurdererVisible(AMurdererCharacter* Murderer);


		// ------ Blood Corruption ------ //
		
		// How corrupt the player has become from using the corruption abilites.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		float CorruptionLevel;

		// How much blood the player has to spend on abilities.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		float Blood;

		// How many bloodstones the player has to spend on unlocking and upgrading.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		int32 Bloodstones;

		/** Used by the inout functions in controller to select which ability to call */
		void SelectBCAbility(EBloodCorruptionAbilities Ability);

		/** This function sets the abilities cost/cooldown/duration etc based on the abilities level
		*	The stats are gathered from the data table.
		*/
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption")
		void BPSetAbilityStats();

		/** Get the ability info of specified ability */
		UFUNCTION(BlueprintCallable, Category = "Blood Corruption")
		FBloodCorruptionAbilityInfo GetAbilityInfo(EBloodCorruptionAbilities Ability);

		/** Used by the inout functions in controller to select which ability to call */
		UFUNCTION(BlueprintCallable, Category = "Blood Corruption")
		void UpgradeAbility(EBloodCorruptionAbilities Ability, int32 Level, bool ShouldUnlockAbility);


		//-- Blood Rush --//

		/** Blood Corruption - Movement Increase */
		void BCBloodRush();

		// General variables for the blood rush ability
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Rush")
		FBloodCorruptionAbilityInfo BloodRushInfo;

		// For how long the movement speed increase will last.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Rush")
		float BloodRushDuration;

		// If blood rush is currently active
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Rush")
		bool BloodRushActive;

		// How much faster the player will be moving.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Rush")
		float BloodRushSpeedMulti;

		// How much more noise the player will make when moving.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Rush")
		float BloodRushNoiseMulti;

		/** Called after setting movement speed and noise multipliers. Used to keep track of duration and deactivating. */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption : Blood Rush")
		void BPBloodRushActivate();


		//-- Blood Dash --//

		/** Blood Corruption - Quick Dash Forward */
		void BCBloodDash();

		// General variables for the blood dash ability
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Dash")
		FBloodCorruptionAbilityInfo BloodDashInfo;

		// If the player is currently in a dash
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Dash")
		bool BloodDashing;

		// How long until the player input is locked for. Ground friction is activated after his duration also.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Dash")
		float LockPlayerFor;

		// How long the dash should be. The higher value the longer the dash. Combine with LockPlayerFor to make the dash smooth.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Blood Dash")
		float DashStrength;

		/** Called after the general blood corruption cd, cost etc i handled. */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption : Blood Dash")
		void BPBloodDashActivate();


		//-- Beast Calling --//

		/** Blood Corruption - Calls nearby animals to come and distract the enemies */
		void BCBeastCalling();

		// General variables for the beast calling ability
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Beast Calling")
		FBloodCorruptionAbilityInfo BeastCallingInfo;

		// The distance the player can call the animals.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Beast Calling")
		float BeastCallingRange;

		/** Called to force interest for the animals */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption : Beast Calling")
		void BPBeastCallingActivate();


		//-- Hunter Vision --//

		/** Blood Corruption - Enables the player to see enemies */
		void BCHunterVision();

		// General variables for the hunter vision ability
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Hunter Vision")
		FBloodCorruptionAbilityInfo HunterVisionInfo;

		// If the player is currently in a dash
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Hunter Vision")
		bool UsingHunterVision;

		// For how long the movement speed increase will last.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Hunter Vision")
		float HunterVisionDuration;

		// The distance the player can see things with the hunter vision.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Hunter Vision")
		float HunterVisionRange;

		/** Called after the general blood corruption cd, cost etc i handled. */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption : Hunter Vision")
		void BPHunterVisionActivate();


		//-- Vampiric Hunger --//

		/** Blood Corruption - Turn blood into energy */
		void BCVampiricHunger();

		// General variables for the vampiric hunger ability
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Vampiric Hunger")
		FBloodCorruptionAbilityInfo VampiricHungerInfo;

		// For how long the movement speed increase will last.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption : Vampiric Hunger")
		float VampiricHungerEnergyGain;

		/** Called after turning blood into energy. */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption : Vampiric Hunger")
		void BPVampHungerActivate();


		// ------ Utility Functions ------ //

		/** Rotates the character to look at specified location. LockInput = Stops player input for camera until it's done */
		UFUNCTION(BlueprintCallable, Category = "Custom Movement")
		void PlayerLookAt(FVector Location, float Speed, bool LockInput, bool UnlockWhenDone);

		/** Get the current state that the player is currently in. Example: walking, sprinting, faliing, standingstill */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		EPlayerMovementState CurrentMovementState();
		
		/** If the player is moving at all */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		bool PlayerMoving();

		/** Is the player crouched */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		bool PlayerCrouched();

		/** Is the player moving while crouched */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		bool PlayerMovingCrouched();

		/** Is the player running */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		bool PlayerSprinting();

		/** Is the player walking normally */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		bool PlayerWalking();

		/** Is the player walking slowly */
		UFUNCTION(BlueprintPure, Category = "Custom Movement")
		bool PlayerWalkingSlow();

		/** Create a noise for the AI to hear */
		UFUNCTION(BlueprintCallable, Category = "AI Related")
		void CreateNoise(FVector Location, float Loudness/*, bool InfiniteRange*/);

		/** Used to send text to the display inteaction string */
		UFUNCTION(BlueprintCallable, Category = "HUD")
		void ShowDisplayInteractionString(FString Text);


	private:

		// Sort function to put the most important objectives first in the array.
		inline static bool ConstPredicate(const UObjectiveData& ip1, const UObjectiveData& ip2)
		{
			return (ip1.ObjectiveImportance > ip2.ObjectiveImportance);
		}

		// Player look at
		bool ShouldTurn;
		FVector LookAtLocation;
		float TurnSpeed;
		bool ShouldUnlock;

		// Stress
		FTimerHandle StressTimer;
		float TimeSinceStressChange;

		// Cold
		FTimerHandle ColdTimer;

		// Energy
		FTimerHandle EnergyTimer;

		/** Functions that updates the stress level every 1 second, lowers or increases */
		void UpdateStressLevel();

		/** Functions that updates the Cold percentage every 1 second, lowers or increases */
		void UpdateColdPercentage();

		/** Functions that updates the Energy percentage every 1 second, lowers or increases */
		void UpdateEnergyPercentage();

		/** Manages the breathing of the player */
		void UpdateBreathing(float DeltaTime);

		// The current gamemode instance that is being used
		AHomeInvasionGameMode* CurrentGameMode;

		// Current player controller which is possesing the player
		AFPSPlayerController* CurrentPlayerController;

		// Handle to manage the time
		FTimerHandle DelayedDyingTimer;

		/** Checks if there is cooldowns that needs to be counted down and counts them down */
		void UpdateAbilityCooldowns(float Delta);

		// Trace parameters for the line trace
		FCollisionQueryParams TraceParams;

	protected:

		/** Function called when player lands after being in the air */
		virtual void Landed(const FHitResult& Hit) override;

		/** Used to check if the location specified is infront of this actor */
		bool LocationInFront(FVector inLocation);

		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
};
