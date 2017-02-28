// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

#include "MurdererAIPerceptionComponent.h"

#include "MurdererCharacter.h"
#include "MurdererUtility/MurdererWaypoint.h"
#include "MurdererUtility/MurdererPointOfInterest.h"

#include "../PlayerCharacter/PlayerVisiblityPoints.h"
#include "../PlayerCharacter/HidingSpot/PlayerHidingSpot.h"

#include "../InteractableObjects/IO_Door.h"

#include "HomeInvasionGameMode.h"

#include "AIController.h"
#include "MurdererController.generated.h"

/** Used by the AI to keep track how much noise has been heard from certain HidingSpots */
USTRUCT(BlueprintType)
struct FHidingSpotNoise
{
	GENERATED_BODY()

	// Amount of noise heard from this hidingspot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float NoiseHeard;

	// Time since the last noise was heard from this hidingspot, set to 0 and counts up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float TimeSinceLastNoise;

	// Actor that is sensed by the perception comp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	APlayerHidingSpot* HidingSpot;

	FHidingSpotNoise()
	{
		TimeSinceLastNoise = 0.0f;
		NoiseHeard = 0.0f;
		HidingSpot = nullptr;
	}
};

/** Used by the AI when randomly generating new locations to move to */
USTRUCT(BlueprintType)
struct FRandomTargetLocation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float Radius;

	//Constructor
	FRandomTargetLocation()
	{
		Radius = 0.0f;
		Location = FVector::ZeroVector;
	}
};


/** Used to keep track of actors that are in view by the perception component
*	The perception component cannot handle visibility of objects behind certain object types, therefore this struct and line traces keeps track of it
*/
USTRUCT(BlueprintType)
struct FSightSensedActor
{
	GENERATED_BODY()

	// Actor that is sensed by the perception comp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	AActor* SensedActor;

	// If the actor is visible, checked by line traces
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	bool Visible;

	void Destroy()
	{
		SensedActor = nullptr;
	}

	//Constructor
	FSightSensedActor()
	{
		SensedActor = nullptr;
		Visible = false;
	}
};

UENUM(BlueprintType)
enum class EMurdererType
{
	EHunter,
	EStalker,
	EWolf,
	EBear,
	EDeer
};


/** Base controller class for all AI's.
*	Each different type of AI will have it's own controller that inherits from this base class.
*	This class has base values that all the AI's will use when in the game.
*	Each different murderer will have it's own blackboard that must contain the base key id values contained in this class.
*/
UCLASS()
class HOMEINVASION_API AMurdererController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;
	
	/** Take control of pawn */
	virtual void Possess(class APawn* InPawn);

	public:

		AMurdererController(/*const FObjectInitializer& ObjectInitializer*/);

		//-- Sensing Properties --//

		// AI Perception component used for sensing actors
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception Comp")
		UMurdererAIPerceptionComponent* PerceptionComp;

		// Amount of visibility points seen
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing")
		int32 VisiblePlayerPoints;

		// Actors currently beeing seen
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing")
		TArray<FSightSensedActor> SightSensedActors;


		// What type of AI this controller is
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI General")
		TEnumAsByte<EMurdererType> MurdererType;


		//-- Point of interest and head turning --//

		// The location that the AI should look at in the world
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing")
		FVector MostInterestingPoint;

		// The rotation for the skeletal mesh's head to look at
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing")
		FRotator LookAtRotation;

		// All the points of interest currently in view
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing")
		TArray<AMurdererPointOfInterest*> VisiblePoIs;

		// The duration that the AI will look at the point of interests
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing")
		float LookAtDuration;


		//-- Hearing --//

		// 1-100 Value, if it reaches >= 100 the AI will get a forced interest function call to that location
		// AI hearing sounds will cause this value to go up
		// The value will decrease after 3 seconds of no change to it
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing")
		float HearingAggro;

		// How much noise needs to be heard from a hidingspot before the AI will check it
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing")
		float HearingHidingSpotLimit;

		// Array of the current hidingspots that the AI has heard noises from.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing")
		TArray<FHidingSpotNoise> HeardHidingSpots;

		// Rotate to face sound location after the hearing aggro passes this value
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing")
		float FaceSoundLimit;

		// Time the AI has to wait before rotating to face sound again
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing")
		float FaceSoundCooldown;

		// Move these, they do not need to be public
		float LastHearingChange;
		FVector HearingLookAtLoc;


		//-- Aggro Increase --//

		// Standard flat increase to aggro when player is in view
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float AggroIncrease;

		// Multiplier for increasing the aggro when the player is running.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float PlayerSprintingIncrease;

		// Multiplier for increasing the aggro when the player is walking.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float PlayerWalkingIncrease;

		// Multiplier for increasing the aggro when the player is walking slowly.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float PlayerWalkingSlowIncrease;

		// Multiplier for increasing the aggro when the player is moving while crouched.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float PlayerCrouchMovingIncrease;

		// Multiplier for increasing the aggro when the player is falling.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float PlayerFallingIncrease;

		// Minimum distance between the player and AI before the distance multiplier kicks in.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float MinDistance;

		// Multiplier used when player is within MinDistance
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float DistanceMultiplier;

		// Multiplier for when the player is fully visible, all Visibility Points, 1 for no change
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float FullyVisibleMultiplier;

		// Wether or not the flashlight can be seen.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing Increase")
		bool CanSeeFlashlight;

		// How much the aggro should increase when the flashlight is in view. Not a multiplier
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Increase")
		float FlashlightIncrease;


		//-- Aggro Decrease --//

		// If the aggro value drops below this, stop chasing the player.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Decrease")
		float StopChasingValue;

		// Standard flat decrease to aggro when player is not in view
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Decrease")
		float AggroDecrease;

		// Multiplier for decrease when player is not in view but AI is still chasing.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sensing Decrease")
		float LostViewChaseDecrease;


		//-- General variables and functions --//

		// If the AI is of the type that attacks the player or not.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI General")
		bool IsAggressive;

		// Used to make sure the AI retreats during the day. The humanoids does not chase the player during the day.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI General")
		bool IsHumanoid;

		/** If the player is being chased by this AI */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		bool IsChasing() const;

		/** If the player is being chased by this AI */
		UFUNCTION(BlueprintPure, Category = "AI Functions")
		bool IsFleeing();

		// Time since the AI last saw the player character
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI General")
		float TimeSincePlayerSeen;

		// This value is checked against time since player seen to determine what to do when player enters a hidingspot
		// If the time is less than this value, check the hidingspot. If the time is higher than this value, stop chasing the player.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI General")
		float HidingSpotTimeLimit;

		// The ID used when looking for new waypoints to travel to. Set when spawning AI.
		// Each waypoint and AI holds their own ID, these two must match in order for the AI to use the waypoints.
		// Used to have certain AI's use certain waypoints while other AI's use other waypoints.
		// The current waypoints that should be used when selecting a new waypoint.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI General")
		int32 UsableWaypointID;

		// The waypoint ID used when it's nighttime. Set during AI spawn and when time changes.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI General")
		int32 NightTimeWaypointID;

		// The waypoint ID used it's daytime. Set during AI spawn and when time changes.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI General")
		int32 DayTimeWaypointID;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI General")
		bool ShouldPrintInfo;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI General")
		bool ShouldDebugDraw;

		// Used by the BT to look for random locations to travel too. Each struct provides a location and radius.
		// The array will mainly only contain more than one struct if traveling to a waypoint that has child locations.
		// When the AI should roam, this array needs to have atleast one elemnt that contains the origin point and the radius.
		// Should always be emptied before new values are added.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI General")
		TArray<FRandomTargetLocation> RandomTargetLocations;

		/** Called by the nav link proxy when the AI has reached it that there is a closed door that the AI needs to open */
		UFUNCTION(BlueprintCallable, Category = "AI General")
		void MakeAwareOfDoor(AIO_Door* Door);

		/** Called by the character when it enters or leaves a day override zone */
		UFUNCTION(BlueprintCallable, Category = "AI General")
		void SetDayOverride(bool Inside);


		//-- Functions --//
		// C++ called functions & Behavior tree called functions

		/** Perception updated function used by AI Perception component */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void AISense(TArray<AActor*> SensedActors);

		/** Get the blackboard */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		UBlackboardComponent* GetBlackboard();

		/** Find the player in the world and set blackboard value */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void FindPlayer();

		/** Set the blackboard waypoint to a new waypoint to travel too. UseZones to check which zone the player is in, Check duraiton for minimum duration player needs to be in a zone for it to count */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void SelectNewWaypoint(bool UseZones, float CheckDuration);

		/** The functions that increases and decreases the aggro for the AI */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void AggroCheck(float DeltaTime);

		/** The functions that increases and decreases the aggro for the AI, used by the stalker */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void AggroCheckStalker(float DeltaTime);

		/** Function used to have the AI move to a certain location */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void ForceInterestInLoc(FVector Location);

		/** Set the AI to wanting to flee from provided location, sets the BB bool and vector value */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void FleeFromLocation(FVector Location);

		/** Called by character to notify that the player flashlight is on the character */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void FlashlightOnSelf();

		/** Manages what the AI should be looking at, what the AI should turn it's head to face */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void ManageLookAround(float DeltaTime);

		/** Function used for when the player is being chased. Resets the look around variables and looks at player */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void LookAtPlayer();

		/** Service function that is used by the BT to check if the player can currently be attacked */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		bool CanAttackPlayer();

		/** Service function that is used by the BT to check if the player is currently in view, checks the players PlayerPoints. Also checks if the AI knows about the player hiding. */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void CanSeeTarget();

		/** Service function that is used by the BT to check if the player is currently in view, checks the players PlayerPoints */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void CanSeeTargetNoHiding();

		/** Directly add a value to the Aggro value, positive or negative */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void AddToAggro(float Value);

		/** Call this function to stun the enemy */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void Stun();

		/** Stops the AI from chasing the player, reduces aggro to 0 */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void StopChasing();

		/** Stops the AI from stalking the player, reduces aggro to 0 */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void StopStalking();


	protected:

		// Function used to handle the noises heard by the hearing sense, to shorten the actual AISense function.
		void HeardNoise(FAIStimulus Stimuli);

		// General Blackboard Variables used by every murderer
		uint8 TargetKeyID;				// Player
		uint8 LastKnownLocKeyID;		// Last known location of player
		uint8 CanSeeTargetKeyID;		// Target in view
		uint8 ShouldChaseTargetKeyID;	// Should the AI chase the player
		uint8 ShouldStalkKeyID;			// Should the AI stalk the player, Stalker only
		uint8 PlayerDeadKeyID;			// If the player is dead
		
		uint8 SelfActorKeyID;			// Blackboard reference to the pawn
		uint8 AggroPercentageKeyID;		// 0-100.0f. If 100, chase the player
		uint8 PreventAggroReductionKeyID;// Stops aggro from reducing
		uint8 TargetWaypointKeyID;		// Waypoint to go to.
		uint8 TargetLocationKeyID;		// The vector that is used for all movement
		uint8 ForcedInterestLocKeyID;	// Used when the AI should go to a specfic location that is not player loc or waypoint.
		uint8 InterestingNoiseLocKeyID; // Location used to rotate to face sound after the AI has heard a certain amount of sound
		uint8 StunnedKeyID;				// If the AI is stunned

		uint8 ShouldFleeKeyID;			// If the AI should flee from something
		uint8 ScaryLocationKeyID;		// If the AI should flee from something

		uint8 ClosedDoorKeyID;			// The closed door that is infront of the AI
		uint8 DoorAILookLocKeyID;		// The location of the scene comp that the AI will rotate towards

		uint8 HidingSpotKeyID;			// The hidingspot that should be checked
		uint8 HidingSpotLocKeyID;		// THe location the AI should walk to before checking the hidingspot
		uint8 KnowPlayerHidingKeyID;	// If the AI know the player is hiding in a hidingspot

		uint8 SoundObjectKeyID;			// The soundobject that is causing noise
		uint8 SoundObjectLocKeyID;		// THe location the AI should walk to before turning off the object

		uint8 DayTimeKeyID;				// If the current time is day.
		uint8 FlashlightOnSelfKeyID;	// If the player flashlight is on the AI

		uint8 OverrideDayKeyID;			// If the Ai is inside the an active DayOverride zone.

		uint8 InitializedAIKeyID;		// Wether the BT has run the initilize branch or not
		
		uint8 TempInt;

		// Used by LookAtDuration to count down until next look at change.
		float LookAtPointDelay;

		// Hearing
		float FaceSoundFaceCountdown;

		float PreventAggroIncrease;

		// Value that counts down to 0, set to a value by FlashlightOnSelf. If the value is >0, the bb value is true.
		float FlashlightCountdown;

		// Pawn controlled by this controller
		AMurdererCharacter* ControlledPawn;

		// Current GameMode
		AHomeInvasionGameMode* CurrentGameMode;

		// Trace parameters for the LoS line trace
		FCollisionQueryParams TraceParams;

		/** Used to check if the location specified is infront of this actor */
		bool LocationInFront(FVector inLocation);

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
};
