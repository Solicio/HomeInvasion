// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MurdererController.h"
#include "../HomeInvasion/PlayerCharacter/FPSCharacter.h"
#include "../InteractableObjects/IO_SoundObject.h"

AMurdererController::AMurdererController(/*const FObjectInitializer& ObjectInitializer*/)
	//: Super(ObjectInitializer)
{
	// Create the blackboard and behavior tree
	BlackboardComp = /*ObjectInitializer.*/CreateDefaultSubobject<UBlackboardComponent>(/*this, */TEXT("BlackboardComp"));
	BehaviorComp = /*ObjectInitializer.*/CreateDefaultSubobject<UBehaviorTreeComponent>(/*this, */TEXT("BehaviorComp"));

	// Setup the perception component
	PerceptionComp = /*ObjectInitializer.*/CreateDefaultSubobject<UMurdererAIPerceptionComponent>(/*this, */TEXT("PerceptionComp"));
	PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMurdererController::AISense);

	// Default aggro values
	AggroIncrease = 20.0f;
	PlayerSprintingIncrease = 7.5f;
	PlayerWalkingIncrease = 5.0f;
	PlayerWalkingSlowIncrease = 2.5f;
	PlayerCrouchMovingIncrease = 2.5f;
	PlayerFallingIncrease = 7.5f;
	MinDistance = 400.0f;
	DistanceMultiplier = 10.0f;
	StopChasingValue = 50.0f;
	FullyVisibleMultiplier = 2.0f;

	CanSeeFlashlight = false;
	FlashlightIncrease = 30.0f;

	AggroDecrease = 5.0f;
	LostViewChaseDecrease = 3.0f;
	
	VisiblePlayerPoints = 0;

	// AI look at default values
	LookAtPointDelay = 0.0f;
	LookAtDuration = 3.0f;

	// Hearing
	HearingHidingSpotLimit = 30.0f;
	FaceSoundLimit = 60.0f;
	FaceSoundCooldown = 10.0f;

	TimeSincePlayerSeen = 0.0f;
	HidingSpotTimeLimit = 3.0f;

	IsAggressive = true;

	// Make sure it is set to the default
	UsableWaypointID = 0;

	TempInt = 0;
}

void AMurdererController::BeginPlay()
{
	Super::BeginPlay();
	// Note: Possess occurs after begin play.
}

void AMurdererController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);
	
	ControlledPawn = Cast<AMurdererCharacter>(InPawn);
	if (ControlledPawn && ControlledPawn->AIBehavior)
	{
		// Initialize the bloackboard
		BlackboardComp->InitializeBlackboard(*(ControlledPawn->AIBehavior->BlackboardAsset));

		// Get the keys to the blackboard values, use the key ID's when setting or getting a value
		TargetKeyID = BlackboardComp->GetKeyID("Target");
		LastKnownLocKeyID = BlackboardComp->GetKeyID("LastKnownLoc");
		CanSeeTargetKeyID = BlackboardComp->GetKeyID("CanSeeTarget");
		ShouldChaseTargetKeyID = BlackboardComp->GetKeyID("ShouldChaseTarget");
		ShouldStalkKeyID = BlackboardComp->GetKeyID("ShouldStalk");
		PlayerDeadKeyID = BlackboardComp->GetKeyID("PlayerDead");
		
		SelfActorKeyID = BlackboardComp->GetKeyID("SelfActor");
		AggroPercentageKeyID = BlackboardComp->GetKeyID("AggroPercentage");
		PreventAggroReductionKeyID = BlackboardComp->GetKeyID("PreventAggroReduction");
		TargetWaypointKeyID = BlackboardComp->GetKeyID("TargetWaypoint");
		TargetLocationKeyID = BlackboardComp->GetKeyID("TargetLocation");
		ForcedInterestLocKeyID = BlackboardComp->GetKeyID("ForcedInterestLoc");
		InterestingNoiseLocKeyID = BlackboardComp->GetKeyID("InterestingNoiseLoc");
		StunnedKeyID = BlackboardComp->GetKeyID("Stunned");

		ShouldFleeKeyID = BlackboardComp->GetKeyID("ShouldFlee");
		ScaryLocationKeyID = BlackboardComp->GetKeyID("ScaryLocation");

		ClosedDoorKeyID = BlackboardComp->GetKeyID("ClosedDoor");
		DoorAILookLocKeyID = BlackboardComp->GetKeyID("DoorAILookLoc");

		HidingSpotKeyID = BlackboardComp->GetKeyID("HidingSpot");
		HidingSpotLocKeyID = BlackboardComp->GetKeyID("HidingSpotLoc");
		KnowPlayerHidingKeyID = BlackboardComp->GetKeyID("KnowPlayerHiding");

		SoundObjectKeyID = BlackboardComp->GetKeyID("SoundObject");
		SoundObjectLocKeyID = BlackboardComp->GetKeyID("SoundObjectLoc");

		DayTimeKeyID = BlackboardComp->GetKeyID("DayTime");
		FlashlightOnSelfKeyID = BlackboardComp->GetKeyID("FlashlightOnSelf");

		OverrideDayKeyID = BlackboardComp->GetKeyID("OverrideDay");

		InitializedAIKeyID = BlackboardComp->GetKeyID("InitializedAI");

		// Start the behavior tree
		BehaviorComp->StartTree(*ControlledPawn->AIBehavior);

		// Set the sight config of the AI perception
		PerceptionComp->SetUpSight(ControlledPawn->SightRange, ControlledPawn->LoseSightRange, ControlledPawn->PeripheralVisionAngle);
		PerceptionComp->SetUpHearing(ControlledPawn->HearingRange);

		// Set the self actor
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(SelfActorKeyID, ControlledPawn);

		// Trace collision parameters for LoS hearing
		TraceParams = FCollisionQueryParams(FName(TEXT("LoS Trace")), false, nullptr);
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = false;
		TraceParams.AddIgnoredActor(ControlledPawn);

		// Current game mode of the level
		CurrentGameMode = Cast<AHomeInvasionGameMode>(GetWorld()->GetAuthGameMode());
	}
}

// Called every frame
void AMurdererController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Debug draw the current location to travel to.
	if (ShouldDebugDraw && BlackboardComp->GetValue<UBlackboardKeyType_Vector>(TargetLocationKeyID) != FVector(0.0f, 0.0f, 0.0f))
	{
		FVector ArrowStart = BlackboardComp->GetValue<UBlackboardKeyType_Vector>(TargetLocationKeyID);
		ArrowStart = FVector(ArrowStart.X, ArrowStart.Y, ArrowStart.Z + 150.0f);

		FVector ArrowEnd = ArrowStart + (-ArrowStart.UpVector * 100.0f);

		DrawDebugDirectionalArrow(GetWorld(), ArrowStart, ArrowEnd, 90.0f, FColor::Red);
	}

	// Check the hearing aggro value, decrease after couple of seconds of no change
	if (LastHearingChange >= 3.0f && HearingAggro > 0.0f)
	{
		HearingAggro -= DeltaTime * 5.0f;
		
		if (HearingAggro < 0.0f)
			HearingAggro = 0.0f;
	}
	else
	{
		LastHearingChange += DeltaTime;
	}

	// Manage the HidingSpotNoise structs, Iterate backwards to allow for removal of elements
	for (int32 i = HeardHidingSpots.Num() - 1; i >= 0; --i)
	{
		// If it has been 3 or more seconds since the last noise was heard from this hidingspot
		if (HeardHidingSpots[i].TimeSinceLastNoise >= 3.0f)
		{
			if (HeardHidingSpots[i].NoiseHeard > 0.0f)
			{
				// Start reducing the noise heard from this spot
				HeardHidingSpots[i].NoiseHeard -= DeltaTime * 5.0f;
			}

			// If the noise reaches 0, remove it from the array as it is no longer interesting
			if (HeardHidingSpots[i].NoiseHeard <= 0.0f)
			{
				HeardHidingSpots.RemoveAt(i);
			}
		}
		else
		{
			// Start counting up time since last noise
			HeardHidingSpots[i].TimeSinceLastNoise += DeltaTime;
		}
	}

	//for (int32 i = 0; i < HeardHidingSpots.Num(); i++)
	//{
	//	PrintText("Noise: " + FString::SanitizeFloat(HeardHidingSpots[i].NoiseHeard) + "Time Since last: " + FString::SanitizeFloat(HeardHidingSpots[i].TimeSinceLastNoise), -1, FColor::Green, 0.0f);
	//}

	// Check visibility of all currently sight sensed actors.
	// The AI Perception component can currently see through certain object types that are not WorldStatic.
	// Therefore there needs to be additional checks for LoS for cases where the player is behind a object with an object type that is not WorldStatic.
	int32 VisibleActors = 0;
	CanSeeFlashlight = false;
	for (int32 i = 0; i < SightSensedActors.Num(); i++)
	{
		if (SightSensedActors[i].SensedActor == nullptr)
			break;

		// Trace between the stimuli and the AI to see if there is something blocking the view of the stimuli
		FHitResult HitResult(ForceInit);
		FVector StartFVector = ControlledPawn->GetMesh()->GetSocketLocation("headSocket");
		FVector EndFVector = SightSensedActors[i].SensedActor->GetActorLocation();

		// If the trace hits a object
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
		{
			// Actor is not visible
			SightSensedActors[i].Visible = false;

			// Remove point of interests from the array if not visible
			if (SightSensedActors[i].SensedActor->ActorHasTag("PointOfInterest"))
			{
				AMurdererPointOfInterest* TempCast = Cast<AMurdererPointOfInterest>(SightSensedActors[i].SensedActor);
				if (VisiblePoIs.Contains(TempCast))
					VisiblePoIs.Remove(TempCast);
			}
		}
		else
		{
			// Actor is visible
			SightSensedActors[i].Visible = true;

			// Add point of interests to the array if the actor is one and there is not one currently in the array
			if (SightSensedActors[i].SensedActor->ActorHasTag("PointOfInterest"))
			{
				AMurdererPointOfInterest* TempCast = Cast<AMurdererPointOfInterest>(SightSensedActors[i].SensedActor);
				if (!VisiblePoIs.Contains(TempCast))
					VisiblePoIs.Add(TempCast);
			}
		}

		// Count them to print out
		if (SightSensedActors[i].Visible == true)
		{
			VisibleActors++;

			// Check if the flashlight can be seen
			if (SightSensedActors[i].SensedActor->ActorHasTag("FlashlightPoint"))
			{
				CanSeeFlashlight = true;
			}
		}
	}

	// Count how long since the last time the player was seen.
	if (!BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID))
	{
		TimeSincePlayerSeen += DeltaTime;
	}
	else
	{
		TimeSincePlayerSeen = 0.0f;
	}

	// Count down after the AI has been set to rotate
	if (FaceSoundFaceCountdown > 0.0f)
	{
		FaceSoundFaceCountdown -= DeltaTime;
	}

	// Count down after the AI has been set to rotate
	if (PreventAggroIncrease > 0.0f)
	{
		PreventAggroIncrease -= DeltaTime;
	}

	// Count down after the flashlight has been on the murderer character
	if (FlashlightCountdown > 0.0f)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(FlashlightOnSelfKeyID, true);
		FlashlightCountdown -= DeltaTime;
	}
	else
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(FlashlightOnSelfKeyID, false);
	}

	//PrintText(FString::SanitizeFloat(TimeSincePlayerSeen), -1, FColor::Green, 0.0f);

	if (ShouldPrintInfo)
	{
		if (CanSeeFlashlight)
		{
			PrintText("Can see light!", -1, FColor::Cyan, 0.0f);
		}

		if(BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
			PrintText("Aggro Percentage: " + FString::SanitizeFloat(BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID)) + ", Chasing", 1, FColor::Red);
		else if(BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID))
			PrintText("Aggro Percentage: " + FString::SanitizeFloat(BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID)) + ", Fleeing", 1, FColor::Red);
		else
			PrintText("Aggro Percentage: " + FString::SanitizeFloat(BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID)), 1, FColor::Red);

		PrintText("Hearing Aggro: " + FString::SanitizeFloat(HearingAggro), 2, FColor::Red);
		//PrintText("Look at Rot: " + LookAtRotation.ToString(), 3, FColor::White);
		PrintText("Visible actors: " + FString::FromInt(VisibleActors) + "/" + FString::FromInt(SightSensedActors.Num()), 4, FColor::Cyan);
		PrintText("Visible Player Points: " + FString::FromInt(VisiblePlayerPoints), 5, FColor::Cyan);
		PrintText("Visible Point of Interest: " + FString::FromInt(VisiblePoIs.Num()), 6, FColor::Cyan);
	}
}

// Used to check if the location provided is infront of the pawn
bool AMurdererController::LocationInFront(FVector inLocation)
{
	if (ControlledPawn == nullptr)
		return false;

	FVector Dir = ControlledPawn->GetActorForwardVector();
	FVector Offset = inLocation - ControlledPawn->GetActorLocation();
	Offset = Offset.GetSafeNormal();
	float FrontBack = FVector::DotProduct(Dir, Offset);

	return (FrontBack > FMath::Cos(70.0f) / 2.0f);
}

bool AMurdererController::IsChasing() const
{
	if (BlackboardComp != nullptr)
	{
		return BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID);
	}
	else
	{
		return false;
	}
}

bool AMurdererController::IsFleeing()
{
	if (BlackboardComp != nullptr)
	{
		return BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID);
	}
	else
	{
		return false;
	}
}

// Stores the player when spawned
void AMurdererController::FindPlayer()
{
	if (ControlledPawn == nullptr || BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID) != nullptr)
	{
		return;
	}
	
	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; ++it)
	{
		AFPSCharacter* TestPawn = Cast<AFPSCharacter>(*it);
		if (TestPawn != nullptr)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Object>(TargetKeyID, TestPawn);

			if (ShouldPrintInfo)
			{
				PrintText("Found and stored player", -1, FColor::Green, 2.0f);
			}

			// Ignore player when tracing for hearing LoS
			if (BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID) != nullptr)
				TraceParams.AddIgnoredActor(Cast<AActor>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID)));
		}
	}
}

void AMurdererController::CanSeeTarget()
{
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));

	if (PlayerCharacter == nullptr)
		return;

	// Check how many of the player points are in view
	VisiblePlayerPoints = 0;
	for (int32 i = 0; i < SightSensedActors.Num(); i++)
	{
		if (SightSensedActors[i].Visible && SightSensedActors[i].SensedActor->ActorHasTag("VisibilityPoint"))
		{
			// Check if the point is active
			APlayerVisiblityPoints* TempPoint = Cast<APlayerVisiblityPoints>(SightSensedActors[i].SensedActor);
			if (TempPoint != nullptr && TempPoint->Active)
			{
				VisiblePlayerPoints++;
			}
		}
	}

	// Tell the AI that it is seeing the player or not
	if (VisiblePlayerPoints > 0)
	{
		if (!PlayerCharacter->ShouldBeIgnored)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID, true);
		}
	}
	else if (VisiblePlayerPoints <= 0)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID, false);
	}

	// Check wether the AI knows the player is hiding while the AI is chasing it
	if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID) && BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID) && PlayerCharacter->InsideHidingSpot)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(KnowPlayerHidingKeyID, true);
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(PreventAggroReductionKeyID, true);
		
		if (PlayerCharacter->CurrentHidingSpot != nullptr)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Object>(HidingSpotKeyID, PlayerCharacter->CurrentHidingSpot);
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(HidingSpotLocKeyID, PlayerCharacter->CurrentHidingSpot->AICheckLocation);
		}
	}
	// If the AI did not see the player enter the hidingspot but it was within a couple of seconds of losing sight of the player
	else if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID) && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID) && PlayerCharacter->InsideHidingSpot && TimeSincePlayerSeen < HidingSpotTimeLimit)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(KnowPlayerHidingKeyID, true);
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(PreventAggroReductionKeyID, true);

		if (PlayerCharacter->CurrentHidingSpot != nullptr)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Object>(HidingSpotKeyID, PlayerCharacter->CurrentHidingSpot);
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(HidingSpotLocKeyID, PlayerCharacter->CurrentHidingSpot->AICheckLocation);
		}
	}
	// If the AI did not see the player enter the hidingspot and it was after 3 seconds of lost vision of the player
	else if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID) && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID) && PlayerCharacter->InsideHidingSpot && TimeSincePlayerSeen >= HidingSpotTimeLimit)
	{
		if (!BlackboardComp->GetValue<UBlackboardKeyType_Bool>(KnowPlayerHidingKeyID))
		{
			// Last known location
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(LastKnownLocKeyID, PlayerCharacter->GetActorLocation());

			// Empty the array so there are not any leftover locations.
			RandomTargetLocations.Empty();

			// Add the location and radius to be used to find a random location.
			FRandomTargetLocation RandomTargetLoc;
			RandomTargetLoc.Location = PlayerCharacter->GetActorLocation();
			RandomTargetLoc.Radius = 500.0f;
			RandomTargetLocations.Add(RandomTargetLoc);

			// Stop chasing the player
			BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, false);
			BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, 40.0f);
		}
	}
	else if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID) && BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID) && !PlayerCharacter->InsideHidingSpot)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(KnowPlayerHidingKeyID, false);
	}
}

void AMurdererController::CanSeeTargetNoHiding()
{
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));

	if (PlayerCharacter == nullptr)
		return;

	// Check how many of the player points are in view
	VisiblePlayerPoints = 0;
	for (int32 i = 0; i < SightSensedActors.Num(); i++)
	{
		if (SightSensedActors[i].Visible && SightSensedActors[i].SensedActor->ActorHasTag("VisibilityPoint"))
		{
			// Check if the point is active
			APlayerVisiblityPoints* TempPoint = Cast<APlayerVisiblityPoints>(SightSensedActors[i].SensedActor);
			if (TempPoint != nullptr && TempPoint->Active)
			{
				VisiblePlayerPoints++;
			}
		}
	}

	// Tell the AI that it is seeing the player or not
	if (VisiblePlayerPoints > 0)
	{
		if (!PlayerCharacter->ShouldBeIgnored)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID, true);
		}
	}
	else if (VisiblePlayerPoints <= 0)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID, false);
	}
}

bool AMurdererController::CanAttackPlayer()
{
	AFPSCharacter* PlayerPawn = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
	
	if (PlayerPawn == nullptr)
		return false;

	// Check to see if the AI can attack the Player
	float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
	if (Distance <= ControlledPawn->AttackRange && ControlledPawn->CanAttack && PlayerPawn->CanBeKilled && !PlayerPawn->IsDead && BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID)/* && !PlayerPawn->BloodDashing*/)
	{
		return true;
	}

	return false;
}

void AMurdererController::SelectNewWaypoint(bool UseZones, float CheckDuration)
{
	if (CurrentGameMode == nullptr)
	{
		return;
	}

	AMurdererWaypoint* TempPoint = nullptr;
	TempPoint = CurrentGameMode->SelectNewWaypoint(Cast<AMurdererWaypoint>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetWaypointKeyID)), UseZones, CheckDuration, UsableWaypointID);

	if (TempPoint != nullptr)
	{
		// Set the waypoint BB variable
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(TargetWaypointKeyID, TempPoint);

		// Empty the array so there are not any leftover locations.
		RandomTargetLocations.Empty();

		// Add the location and radius of the base waypoint
		FRandomTargetLocation RandomTargetLoc;
		RandomTargetLoc.Location = TempPoint->GetActorLocation();
		RandomTargetLoc.Radius = TempPoint->WaypointRadius;
		RandomTargetLocations.Add(RandomTargetLoc);

		// Then look if the waypoint has child locations that should also be considered
		// Only check if there is atleast one child location and if there is also the right amount of radiuses
		if (TempPoint->ChildLocations.Num() > 0 && (TempPoint->ChildLocations.Num() == TempPoint->ChildRadiuses.Num()))
		{
			for (int32 i = 0; i < TempPoint->ChildLocations.Num(); i++)
			{
				if (TempPoint->ChildLocations[i] != FVector::ZeroVector)
				{
					RandomTargetLoc.Location = TempPoint->ChildLocations[i];
					
					if (TempPoint->ChildRadiuses[i] != 0.0f)
					{
						RandomTargetLoc.Radius = TempPoint->ChildRadiuses[i];
					}
					else
					{
						RandomTargetLoc.Radius = 300.0f;
					}

					RandomTargetLocations.Add(RandomTargetLoc);
				}
			}
		}
		else if (TempPoint->ChildLocations.Num() != TempPoint->ChildRadiuses.Num())
		{
			PrintText("Uneven Location and Radius amount!", -1, FColor::Red);
		}
	}
	else
	{
		// If the new waypoint is not valid, reset the values.
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(TargetWaypointKeyID, nullptr);

		// Empty the array so there are not any leftover locations.
		RandomTargetLocations.Empty();
	}
}

// Called by the controlled pawn to set the variables used by forced interest in the BB
void AMurdererController::ForceInterestInLoc(FVector Location)
{
	if (!BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID) && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID) && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID))
	{
		// Set the vector that is checked to know if the AI has a forced interest location
		BlackboardComp->SetValue<UBlackboardKeyType_Vector>(ForcedInterestLocKeyID, Location);

		if (ControlledPawn->SoundCheckSphere != nullptr)
		{
			// Move the AI's sphere collision into place of the location of the sound
			ControlledPawn->SoundCheckSphere->SetWorldLocation(Location);

			// Get all actors that are overlapping the sphere
			TArray<AActor*> OverlappingActors;
			ControlledPawn->SoundCheckSphere->GetOverlappingActors(OverlappingActors);

			// Check all the overlapping actors to see if there is one that is important
			for (int32 i = 0; i < OverlappingActors.Num(); i++)
			{
				// If the location is within a hidingspot, store the actor on the blackboard.
				if (OverlappingActors[i]->ActorHasTag("HidingSpot") && IsAggressive)
				{
					//PrintText("Hidingspot!", -1, FColor::White, 2.0f);
					
					APlayerHidingSpot* TempSpot = Cast<APlayerHidingSpot>(OverlappingActors[i]);
					if (TempSpot != nullptr)
					{
						BlackboardComp->SetValue<UBlackboardKeyType_Object>(HidingSpotKeyID, TempSpot);
						BlackboardComp->SetValue<UBlackboardKeyType_Vector>(HidingSpotLocKeyID, TempSpot->AICheckLocation);
					}
				}
				// If the forced interest is inside a sound object
				else if (OverlappingActors[i]->ActorHasTag("SoundObject") && IsAggressive)
				{
					//PrintText("SoundObject", -1, FColor::Red, 1.0f);
					AIO_SoundObject* TempObject = Cast<AIO_SoundObject>(OverlappingActors[i]);
					if (TempObject != nullptr)
					{
						BlackboardComp->SetValue<UBlackboardKeyType_Object>(SoundObjectKeyID, TempObject);
						BlackboardComp->SetValue<UBlackboardKeyType_Vector>(SoundObjectLocKeyID, TempObject->AICheckLocation);
					}
				}
			}
		}
	}
}

void AMurdererController::FleeFromLocation(FVector Location)
{
	//PrintText("FLEE FROM LOCATION C++", -1, FColor::Red, 3.0f);
	// Sets the location to flee from and the boolean that the AI should flee from it.
	BlackboardComp->SetValue<UBlackboardKeyType_Vector>(ScaryLocationKeyID, Location);
	//BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID, true);
}

void AMurdererController::MakeAwareOfDoor(AIO_Door* Door)
{
	// Called by nav link proxies to set blackboard values that has with doors.
	BlackboardComp->SetValue<UBlackboardKeyType_Object>(ClosedDoorKeyID, Door);
	BlackboardComp->SetValue<UBlackboardKeyType_Vector>(DoorAILookLocKeyID, Door->AILookAtLoc);
}

void AMurdererController::SetDayOverride(bool Inside)
{
	// If it is day and the AI leaves a override day zone, it should stop chasing the player.
	if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(DayTimeKeyID) && !Inside)
	{
		StopChasing();
	}

	// Set the blackboard value of OverrideDay, called from the character
	BlackboardComp->SetValue<UBlackboardKeyType_Bool>(OverrideDayKeyID, Inside);
}

// Main function that check if any of it's senses sense anything. Hearing or sight.
void AMurdererController::AISense(TArray<AActor*> SensedActors)
{
	for (int32 i = 0; i < SensedActors.Num(); i++)
	{
		if (SensedActors[i] != nullptr)
		{
			// Get information regarding the current sensed actor
			const FActorPerceptionInfo* PerceptionInfo = PerceptionComp->GetActorInfo(*SensedActors[i]);

			if (PerceptionInfo)
			{
				for (int32 j = 0; j < PerceptionInfo->LastSensedStimuli.Num(); j++)
				{
					// --- Sight
					// If the actor's stimuli was sight, in view of AI
					if (PerceptionInfo->LastSensedStimuli[j].Type == UAISense::GetSenseID<UAISense_Sight>())
					{
						// Check if the stimuli was successfully sensed
						if (PerceptionInfo->LastSensedStimuli[j].WasSuccessfullySensed())
						{
							// Check if the actor is in the array
							bool ContainsActor = false;
							for (int32 k = 0; k < SightSensedActors.Num(); k++)
							{
								if (SightSensedActors[k].SensedActor == SensedActors[i])
								{
									ContainsActor = true;
								}
							}

							//PrintText(SensedActors[i]->GetName(), -1, FColor::White, 0.5f);

							// If not, add a new struct
							if (!ContainsActor)
							{
								FSightSensedActor NewSensedActor;
								NewSensedActor.SensedActor = SensedActors[i];
								NewSensedActor.Visible = false;
								SightSensedActors.Add(NewSensedActor);
							}
						}
						// If the stimuli moved out of view
						else if (!PerceptionInfo->LastSensedStimuli[j].WasSuccessfullySensed())
						{
							// Check if the actor is in the array
							/*for (int32 k = 0; k < SightSensedActors.Num(); k++)*/
							for (int32 k = SightSensedActors.Num() - 1; k >= 0; --k)
							{
								if (SightSensedActors[k].SensedActor == SensedActors[i])
								{
									SightSensedActors.RemoveAt(k);
								}
							}
						}
					}

					// --- Hearing
					// If the actor's stimuli was hearing, made noise inside hearing range of AI
					if (PerceptionInfo->LastSensedStimuli[j].Type == UAISense::GetSenseID<UAISense_Hearing>())
					{
						AFPSCharacter* PlayerPawn = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
						
						// Check if the stimuli is new, IsExpired does not seem to work atm
						if (PerceptionInfo->LastSensedStimuli[j].GetAge() == 0.0f && (PlayerPawn == nullptr || (PlayerPawn != nullptr && !PlayerPawn->ShouldBeIgnored)))
						{
							// Call the function that handles heard noises
							HeardNoise(PerceptionInfo->LastSensedStimuli[j]);
						}
					}
				}
			}
		}
	}
}

void AMurdererController::HeardNoise(FAIStimulus Stimuli)
{
	if (PreventAggroIncrease > 0.0f /*|| (IsHuman && BlackboardComp->GetValue<UBlackboardKeyType_Bool>(DayTimeKeyID))*/)
		return;
	
	float StimStrength = FCString::Atof(*Stimuli.Tag.ToString());
	bool StimInFront = LocationInFront(Stimuli.StimulusLocation);
	//float StimStrength = Stimuli.Strength;

	//PrintText("Stim Strength: " + FString::SanitizeFloat(StimStrength), -1, FColor::Green, 1.0f);

	// If it is a large sound, force direct interest in that location
	if (StimStrength >= 100.0f)
	{
		if (IsAggressive && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID))
		{
			ForceInterestInLoc(Stimuli.StimulusLocation);
		}
		else if (!IsAggressive)
		{
			FleeFromLocation(Stimuli.StimulusLocation);
			HearingAggro = 0.0f;
		}
	}
	// If it is a lower strength sound add that to the HearingAggro float
	else if ((StimStrength < 100.0f && StimStrength > 0.0f) && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
	{
		// Trace between the AI and the sound to see if there is something blocking the sound
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams HearingTraceParams = TraceParams;
		FVector StartFVector = ControlledPawn->GetMesh()->GetSocketLocation("headSocket");
		FVector EndFVector = Stimuli.StimulusLocation;
		bool SoundBlocked = false;

		bool CompletedTrace = false;
		while (!CompletedTrace)
		{
			// Check if there was a blocking hit
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, HearingTraceParams))
			{
				// If the actor is valid to be used
				if (HitResult.GetActor() != nullptr)
				{
					// If the actor has certain tags that should be ignored by the trace, add the actor to the param and trace again
					if (HitResult.GetActor()->ActorHasTag("PickupObject") || HitResult.GetActor()->ActorHasTag("PlayerPhone"))
					{
						// Add the actor to params to be ignored
						HearingTraceParams.AddIgnoredActor(HitResult.GetActor());
						//PrintText("Special Object!", -1, FColor::Green, 1.0f);
					}
					else
					{
						// All other actors that are not PickupObjects will block the noise.
						SoundBlocked = true;
						CompletedTrace = true;
						//DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 20.0f, 0, 1.0f);
					}
				}
				// If there is not actor that can be checked, the noise is blocked. (Only certain actors that we create should be ignored by sound blocking.)
				else
				{
					SoundBlocked = true;
					CompletedTrace = true;
					//DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 20.0f, 0, 1.0f);
				}
			}
			else
			{
				// The sound is not blocked if the line trace returns false, finds no blocking hits
				SoundBlocked = false;
				CompletedTrace = true;
				//DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Green, false, 20.0f, 0, 1.0f);
			}
		}

		// If the location is in front of the AI, look at the location
		if (StimInFront && !SoundBlocked)
		{
			HearingLookAtLoc = Stimuli.StimulusLocation;
		}

		// Add to the aggro value
		// If the sound is blocked, add half of the values strength
		if (SoundBlocked)
		{
			HearingAggro += (StimStrength * 0.5f);
		}
		else
		{
			HearingAggro += StimStrength;
		}

		LastHearingChange = 0.0f;

		// If it has reached the max value, force interest in the location.
		if (HearingAggro >= 100.0f)
		{
			if (IsAggressive && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID))
			{
				HearingAggro = 50.0f;
				ForceInterestInLoc(Stimuli.StimulusLocation);
			}
			else if (!IsAggressive)
			{
				HearingAggro = 0.0f;
				FleeFromLocation(Stimuli.StimulusLocation);
			}
		}
		// If the hearing aggro has reached the value used to check if the AI should rotate to face the sound loc
		else if (HearingAggro >= FaceSoundLimit && FaceSoundFaceCountdown <= 0.0f && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID))
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(InterestingNoiseLocKeyID, Stimuli.StimulusLocation);
			FaceSoundFaceCountdown = FaceSoundCooldown;
		}
		// If it has not reached the max value, check if the sound came from a hidingspot and if so, store that hidingspot
		// No need to check if it comes from a hidingspot if the hearing aggro already is >= 100
		else if(IsAggressive && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldFleeKeyID))
		{
			// Move the AI's sphere collision into place of the location of the sound
			ControlledPawn->SoundCheckSphere->SetWorldLocation(Stimuli.StimulusLocation);

			// Get all actors that are overlapping the sphere
			TArray<AActor*> OverlappingActors;
			ControlledPawn->SoundCheckSphere->GetOverlappingActors(OverlappingActors);

			// Check all the overlapping actors to see if there is one that is important
			for (int32 i = 0; i < OverlappingActors.Num(); i++)
			{
				// If the location is within a hidingspot, make sure to store it for coming noises
				if (OverlappingActors[i]->ActorHasTag("HidingSpot"))
				{
					APlayerHidingSpot* TempSpot = Cast<APlayerHidingSpot>(OverlappingActors[i]);
					if (TempSpot != nullptr)
					{
						// Check if the HidingSpot is already stored from a previous noise heard
						for (int32 j = 0; j < HeardHidingSpots.Num(); j++)
						{
							if (HeardHidingSpots[j].HidingSpot == TempSpot)
							{
								// Add to the noise heard with stimstrength
								if (SoundBlocked)
								{
									HeardHidingSpots[j].NoiseHeard += (StimStrength * 0.5f);
									HeardHidingSpots[j].TimeSinceLastNoise = 0.0f;
								}
								else
								{
									HeardHidingSpots[j].NoiseHeard += StimStrength;
									HeardHidingSpots[j].TimeSinceLastNoise = 0.0f;
								}
							}
						}

						// If the AI is currently not storing any hidingspots, directly add this hidingspot
						if (HeardHidingSpots.Num() == 0)
						{
							FHidingSpotNoise NewHidingSpotNoise;
							NewHidingSpotNoise.HidingSpot = TempSpot;
							NewHidingSpotNoise.TimeSinceLastNoise = 0.0f;

							if (SoundBlocked)
							{
								NewHidingSpotNoise.NoiseHeard += (StimStrength * 0.5f);
							}
							else
							{
								NewHidingSpotNoise.NoiseHeard += StimStrength;
							}

							HeardHidingSpots.Add(NewHidingSpotNoise);
						}
					}
				}
			}

			// Check if there is any hidingspots that has reached the limit
			for (int32 i = 0; i < HeardHidingSpots.Num(); i++)
			{
				if (HeardHidingSpots[i].NoiseHeard >= HearingHidingSpotLimit)
				{
					ForceInterestInLoc(Stimuli.StimulusLocation);
				}
			}
		}
	}

	//PrintText("Noise heard, " + FString::SanitizeFloat(Stimuli.Strength), -1, FColor::Yellow);
	if (ShouldDebugDraw)
	{
		DrawDebugSphere(GetWorld(), Stimuli.StimulusLocation, 5.0f, 10.0f, FColor::Yellow, false, 3.0f);
	}
}

void AMurdererController::Stun()
{
	// Set the boolean
	BlackboardComp->SetValue<UBlackboardKeyType_Bool>(StunnedKeyID, true);
}

void AMurdererController::AddToAggro(float Value)
{
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
	if (PlayerCharacter == nullptr || PlayerCharacter->ShouldBeIgnored || BlackboardComp->GetValue<UBlackboardKeyType_Bool>(PlayerDeadKeyID) || PreventAggroIncrease > 0.0f /*|| (IsHuman && BlackboardComp->GetValue<UBlackboardKeyType_Bool>(DayTimeKeyID))*/)
	{
		return;
	}

	if (!IsAggressive)
	{
		FleeFromLocation(ControlledPawn->GetActorLocation());
		return;
	}

	// Stop reductions
	if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(PreventAggroReductionKeyID) && Value < 0.0f)
	{
		return;
	}

	// Current value of the aggro BB value
	float TempAggro = BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID);

	TempAggro += Value;
	TempAggro = FMath::Clamp(TempAggro, 0.0f, 100.0f);

	// Set the BB value to the new increased value
	BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);

	// Set if the AI should chase or not. Same as AggroCheck, but does not have to wait for it to be called
	if (BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID) >= 100.0f)
	{
		if (IsAggressive && MurdererType != EMurdererType::EStalker)
		{
			// Chase the player
			BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, true);

			// Reset hearing variables
			HearingAggro = 0.0f;
			LastHearingChange = 0.0f;
		}
	}
	else if (IsAggressive && (BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID) <= StopChasingValue) && MurdererType != EMurdererType::EStalker)
	{
		// AI's aggro drops to 50 or below, move around in that area for a bit then move on.
		if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(LastKnownLocKeyID, PlayerCharacter->GetActorLocation());

			// Empty the array so there are not any leftover locations.
			RandomTargetLocations.Empty();

			// Add the location and radius to be used to find a random location.
			FRandomTargetLocation RandomTargetLoc;
			RandomTargetLoc.Location = PlayerCharacter->GetActorLocation();
			RandomTargetLoc.Radius = 500.0f;
			RandomTargetLocations.Add(RandomTargetLoc);
		}

		// Stop chasing the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, false);
	}
}

void AMurdererController::AggroCheck(float DeltaTime)
{
	// Increases or decreases the Aggro float depending on if the player is in view or not.
	// If the aggro reaches a certain point, the AI will start chasing the player
	// If it drops below another point the AI will stop chasing the player.

	// Three places where the aggro increases.
	//	1. Player can be seen.
	//	2. Player cannot be seen but is within the min distance
	//	3. Player cannot be seen and is not within range but flashlight can be seen

	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
	
	// Have the BB keep track wether the player is dead or not.
	if (PlayerCharacter != nullptr)
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(PlayerDeadKeyID, PlayerCharacter->IsDead);

	// Don't continue if there is no player, if it should be ignore or if the player is dead
	if (PlayerCharacter == nullptr || PlayerCharacter->ShouldBeIgnored || BlackboardComp->GetValue<UBlackboardKeyType_Bool>(PlayerDeadKeyID) || PreventAggroIncrease > 0.0f/* || (IsHuman && BlackboardComp->GetValue<UBlackboardKeyType_Bool>(DayTimeKeyID))*/)
	{
		// Set the BB aggro value to 0
		BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, 0.0f);

		// Stop chasing the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, false);

		return;
	}
	
	// Used in all three increases for when the flashlight is seen.
	float FlashlightValue = 0.0f;

	// Checks if the flashlight can be seen
	if (CanSeeFlashlight)
	{
		FlashlightValue = FlashlightIncrease;
	}

	// Trace between the AI and the player to see if there something blocking, used by within distance multi
	FHitResult HitResult(ForceInit);
	FVector StartFVector = ControlledPawn->GetMesh()->GetSocketLocation("headSocket");
	FVector EndFVector = PlayerCharacter->GetActorLocation();
	bool DistanceTraceBlocking = false;

	// If the trace hits a object
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
	{
		DistanceTraceBlocking = true;
		//PrintText("Player Blocked!", -1, FColor::Cyan, 0.0f);
	}

	// Increase the aggro when the player is in view
	if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID))
	{
		// Current value of the aggro BB value
		float TempAggro = BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID);

		// Set base values for the calculation, if none of these are set, they wont affect anything,
		float MovementModeMulti = 1.0f;
		float WithinDist = 1.0f;
		float VisibilityMultiplier = 0;

		if (PlayerCharacter->PlayerCrouched())
		{
			VisibilityMultiplier = (float)VisiblePlayerPoints / 3.0f;
		}
		else
		{
			VisibilityMultiplier = (float)VisiblePlayerPoints / (float)PlayerCharacter->VisibilityPointsNum;
		}

		//PrintText(FString::SanitizeFloat(VisibilityMultiplier), -1, FColor::Cyan, 1.0f);
		
		// If the player is fully visible, change the multiplier
		float FullyVisMulti = 1.0f;
		if (VisibilityMultiplier == 1.0f)
		{
			FullyVisMulti = FullyVisibleMultiplier;
		}

		// Set the value to the respective multiplier value if he argument is true
		switch (PlayerCharacter->CurrentMovementState())
		{
			case EPlayerMovementState::EStandingStill:
				MovementModeMulti = 2.0f;
				break;

			case EPlayerMovementState::EWalking:
				MovementModeMulti = PlayerWalkingIncrease;
				break;

			case EPlayerMovementState::EWalkingSlow:
				MovementModeMulti = PlayerWalkingSlowIncrease;
				break;

			case EPlayerMovementState::ESprinting:
				MovementModeMulti = PlayerSprintingIncrease;
				break;

			case EPlayerMovementState::EWalkingCrouched:
				MovementModeMulti = PlayerCrouchMovingIncrease;
				break;

			case EPlayerMovementState::EFalling:
				MovementModeMulti = PlayerFallingIncrease;
				break;
		};

		// Check distance
		if (FVector::Dist(PlayerCharacter->GetActorLocation(), ControlledPawn->GetActorLocation()) < MinDistance)
			WithinDist = DistanceMultiplier;

		// Increase the value
		TempAggro += ((AggroIncrease * MovementModeMulti * WithinDist * FullyVisMulti) * VisibilityMultiplier) * DeltaTime;
		TempAggro += FlashlightValue * DeltaTime;

		//PrintText("Increase: " + FString::SanitizeFloat((AggroIncrease * MovementModeMulti * WithinDist * FullyVisMulti) * VisibilityMultiplier), -1, FColor::Cyan, 1.0f);

		// Clamp
		TempAggro = FMath::Clamp(TempAggro, 0.0f, 100.0f);

		// Set the BB value to the new increased value
		BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);
	}
	// Decrease the aggro when the player cannot be seen (Exception if the player is moving within the min distance or has player flashlight on character)
	else
	{
		// Current value of the aggro BB value
		float TempAggro = BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID);

		// If the player is within min distance, nothing blocking and the AI is not chasing it, increase the aggro
		if ((FVector::Dist(PlayerCharacter->GetActorLocation(), ControlledPawn->GetActorLocation()) < MinDistance) && !DistanceTraceBlocking && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
		{
			TempAggro += (10 + FlashlightValue) * DeltaTime;
		}
		else
		{
			// If the player cannot be seen and the AI is chasing the target, decrease quicker.
			if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
			{
				TempAggro -= (AggroDecrease * LostViewChaseDecrease) * DeltaTime;
			}
			// Player cannot be seen and is not chasing
			else
			{
				// If the flashlight cannot be seen, decrease the aggro
				if (!CanSeeFlashlight)
				{
					TempAggro -= AggroDecrease * DeltaTime;
				}
				else
				{
					// Increase aggro using the flashlight value.
					TempAggro += FlashlightValue * DeltaTime;
				}
			}
		}

		// Clamp
		TempAggro = FMath::Clamp(TempAggro, 0.0f, 100.0f);

		// Prevent reduction if the bool is true
		if (TempAggro < BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID))
		{
			if (!BlackboardComp->GetValue<UBlackboardKeyType_Bool>(PreventAggroReductionKeyID))
			{
				// Set the BB value to the new decreased value
				BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);
			}
		}
		else
		{
			// Set the BB value to the new decreased value
			BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);
		}
	}

	// Set if the AI should chase or not.
	if (BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID) >= 100.0f)
	{
		// Chase the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, true);

		// Reset hearing variables
		HearingAggro = 0.0f;
		LastHearingChange = 0.0f;
	}
	else if (BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID) <= StopChasingValue)
	{
		// AI's aggro drops to 50 or below, move around in that area for a bit then move on.
		if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(LastKnownLocKeyID, PlayerCharacter->GetActorLocation());

			// Empty the array so there are not any leftover locations.
			RandomTargetLocations.Empty();

			// Add the location and radius to be used to find a random location.
			FRandomTargetLocation RandomTargetLoc;
			RandomTargetLoc.Location = PlayerCharacter->GetActorLocation();
			RandomTargetLoc.Radius = 500.0f;
			RandomTargetLocations.Add(RandomTargetLoc);
		}
		
		// Stop chasing the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, false);
	}
}

void AMurdererController::AggroCheckStalker(float DeltaTime)
{
	// Increases or decreases the Aggro float depending on if the player is in view or not.
	// If the aggro reaches a certain point, the AI will start stalking the player
	// If it drops below another point the AI will stop stalking the player.
	// Function used by the Stalker enemy type.

	// Three places where the aggro increases.
	//	1. Player can be seen.
	//	2. Player cannot be seen but is within the min distance
	//	3. Player cannot be seen and is not within range but flashlight can be seen

	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetKeyID));

	// Have the BB keep track wether the player is dead or not.
	if (PlayerCharacter != nullptr)
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(PlayerDeadKeyID, PlayerCharacter->IsDead);

	// Don't continue if there is no player, if it should be ignore or if the player is dead
	if (PlayerCharacter == nullptr || PlayerCharacter->ShouldBeIgnored || BlackboardComp->GetValue<UBlackboardKeyType_Bool>(PlayerDeadKeyID) || PreventAggroIncrease > 0.0f)
	{
		// Set the BB aggro value to 0
		BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, 0.0f);

		// Stop chasing the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID, false);

		return;
	}

	// Used in all three increases for when the flashlight is seen.
	float FlashlightValue = 0.0f;

	// Checks if the flashlight can be seen
	if (CanSeeFlashlight)
	{
		FlashlightValue = FlashlightIncrease;
	}

	// Trace between the AI and the player to see if there something blocking, used by within distance multi
	FHitResult HitResult(ForceInit);
	FVector StartFVector = ControlledPawn->GetMesh()->GetSocketLocation("headSocket");
	FVector EndFVector = PlayerCharacter->GetActorLocation();
	bool DistanceTraceBlocking = false;

	// If the trace hits a object
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
	{
		DistanceTraceBlocking = true;
		//PrintText("Player Blocked!", -1, FColor::Cyan, 0.0f);
	}

	// Increase the aggro when the player is in view
	if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(CanSeeTargetKeyID))
	{
		// Current value of the aggro BB value
		float TempAggro = BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID);

		// Set base values for the calculation, if none of these are set, they wont affect anything,
		float MovementModeMulti = 1.0f;
		float WithinDist = 1.0f;
		float VisibilityMultiplier = 0;

		if (PlayerCharacter->PlayerCrouched())
		{
			VisibilityMultiplier = (float)VisiblePlayerPoints / 3.0f;
		}
		else
		{
			VisibilityMultiplier = (float)VisiblePlayerPoints / (float)PlayerCharacter->VisibilityPointsNum;
		}

		//PrintText(FString::SanitizeFloat(VisibilityMultiplier), -1, FColor::Cyan, 1.0f);

		// If the player is fully visible, change the multiplier
		float FullyVisMulti = 1.0f;
		if (VisibilityMultiplier == 1.0f)
		{
			FullyVisMulti = FullyVisibleMultiplier;
		}

		// Set the value to the respective multiplier value if he argument is true
		switch (PlayerCharacter->CurrentMovementState())
		{
			case EPlayerMovementState::EStandingStill:
				MovementModeMulti = 2.0f;
				break;

			case EPlayerMovementState::EWalking:
				MovementModeMulti = PlayerWalkingIncrease;
				break;

			case EPlayerMovementState::EWalkingSlow:
				MovementModeMulti = PlayerWalkingSlowIncrease;
				break;

			case EPlayerMovementState::ESprinting:
				MovementModeMulti = PlayerSprintingIncrease;
				break;

			case EPlayerMovementState::EWalkingCrouched:
				MovementModeMulti = PlayerCrouchMovingIncrease;
				break;

			case EPlayerMovementState::EFalling:
				MovementModeMulti = PlayerFallingIncrease;
				break;
		};

		// Check distance
		if (FVector::Dist(PlayerCharacter->GetActorLocation(), ControlledPawn->GetActorLocation()) < MinDistance)
			WithinDist = DistanceMultiplier;

		// Increase the value
		TempAggro += ((AggroIncrease * MovementModeMulti * WithinDist * FullyVisMulti) * VisibilityMultiplier) * DeltaTime;
		TempAggro += FlashlightValue * DeltaTime;

		//PrintText("Increase: " + FString::SanitizeFloat((AggroIncrease * MovementModeMulti * WithinDist * FullyVisMulti) * VisibilityMultiplier), -1, FColor::Cyan, 1.0f);

		// Clamp
		TempAggro = FMath::Clamp(TempAggro, 0.0f, 100.0f);

		// Set the BB value to the new increased value
		BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);
	}
	// Decrease the aggro when the player cannot be seen (Exception if the player is moving within the min distance or has player flashlight on character)
	else
	{
		// Current value of the aggro BB value
		float TempAggro = BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID);

		// If the player is within min distance, nothing blocking and the AI is not stalking it, increase the aggro
		if ((FVector::Dist(PlayerCharacter->GetActorLocation(), ControlledPawn->GetActorLocation()) < MinDistance) && !DistanceTraceBlocking && !BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID))
		{
			TempAggro += (10 + FlashlightValue) * DeltaTime;
		}
		else
		{
			// If the player cannot be seen and the AI is stalking the target, decrease quicker.
			if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID))
			{
				TempAggro -= (AggroDecrease * LostViewChaseDecrease) * DeltaTime;
			}
			// Player cannot be seen and is not chasing
			else
			{
				// If the flashlight cannot be seen, decrease the aggro
				if (!CanSeeFlashlight)
				{
					TempAggro -= AggroDecrease * DeltaTime;
				}
				else
				{
					// Increase aggro using the flashlight value.
					TempAggro += FlashlightValue * DeltaTime;
				}
			}
		}

		// Clamp
		TempAggro = FMath::Clamp(TempAggro, 0.0f, 100.0f);

		// Prevent reduction if the bool is true
		if (TempAggro < BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID))
		{
			if (!BlackboardComp->GetValue<UBlackboardKeyType_Bool>(PreventAggroReductionKeyID))
			{
				// Set the BB value to the new decreased value
				BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);
			}
		}
		else
		{
			// Set the BB value to the new decreased value
			BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, TempAggro);
		}
	}

	// Set if the AI should stalk or not.
	if (BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID) >= 100.0f)
	{
		// Stalk the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID, true);

		// Reset hearing variables
		HearingAggro = 0.0f;
		LastHearingChange = 0.0f;
	}
	else if (BlackboardComp->GetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID) <= StopChasingValue)
	{
		// AI's aggro drops to 0, return to the patrol path.
		//if (BlackboardComp->GetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID))
		//{
		//	BlackboardComp->SetValue<UBlackboardKeyType_Vector>(LastKnownLocKeyID, PlayerCharacter->GetActorLocation());

		//	// Empty the array so there are not any leftover locations.
		//	RandomTargetLocations.Empty();

		//	// Add the location and radius to be used to find a random location.
		//	FRandomTargetLocation RandomTargetLoc;
		//	RandomTargetLoc.Location = PlayerCharacter->GetActorLocation();
		//	RandomTargetLoc.Radius = 500.0f;
		//	RandomTargetLocations.Add(RandomTargetLoc);
		//}

		// Stop stalking the player
		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID, false);
	}
}

void AMurdererController::LookAtPlayer()
{
	// TODO: If player is infront & has LoS, look at player

	MostInterestingPoint = FVector::ZeroVector;
	HearingLookAtLoc = FVector::ZeroVector;
	LookAtRotation = ControlledPawn->GetActorRotation();
	LookAtPointDelay = 0.0f;
}

void AMurdererController::StopChasing()
{
	// Set the aggro value to 0
	BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, 0.0f);

	// Tell it to stop chasing
	BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldChaseTargetKeyID, false);

	// Reduce the hearing aggro to 0, to prevent the AI from looking at sounds etc..
	HearingAggro = 0.0f;

	// Prevent the Ai from gaining aggro for this amount of time, also stops hearing
	PreventAggroIncrease = 2.0f;

	//PrintText("StopChasing", -1, FColor::Red, 2.0f);
}

void AMurdererController::StopStalking()
{
	// Set the aggro value to 0
	BlackboardComp->SetValue<UBlackboardKeyType_Float>(AggroPercentageKeyID, 0.0f);

	// Tell it to stop chasing
	BlackboardComp->SetValue<UBlackboardKeyType_Bool>(ShouldStalkKeyID, false);

	// Reduce the hearing aggro to 0, to prevent the AI from looking at sounds etc..
	HearingAggro = 0.0f;

	// Prevent the Ai from gaining aggro for this amount of time, also stops hearing
	//PreventAggroIncrease = 2.0f;

	//PrintText("StopChasing", -1, FColor::Red, 2.0f);
}

void AMurdererController::FlashlightOnSelf()
{
	// Set the countdown so that it can start counting down. This also sets the bb value to true in tick()
	FlashlightCountdown = 1.0f;
}

UBlackboardComponent* AMurdererController::GetBlackboard()
{
	return BlackboardComp;
}

void AMurdererController::ManageLookAround(float DeltaTime)
{
	// Select what location the AI should look at
	// Able to look at Point of Interests and Sound sense locations
	// Add all locations into an TArray and select random loc from that array to look at.

	// Stores all locations that the AI can look at.
	TArray<FVector> Locations;

	//Add all PoIs locations into the location array if there is no hearing locaiton
	if (VisiblePoIs.Num() > 0 && HearingLookAtLoc.IsZero())
	{
		for (int i = 0; i < VisiblePoIs.Num(); i++)
		{
			// Only add it if it's infront of the character
			if (LocationInFront(VisiblePoIs[i]->GetActorLocation()))
			{
				Locations.Add(VisiblePoIs[i]->GetActorLocation());
			}
		}
	}

	// Draw the locations
	if (ShouldDebugDraw)
	{
		for (int i = 0; i < Locations.Num(); i++)
		{
			DrawDebugSphere(GetWorld(), Locations[i], 25.0f, 5, FColor::Red, false, 1.0f);
		}
	}

	// Check to see if the hearing location is in front of the AI, if not, reset the vector
	if (!HearingLookAtLoc.IsZero() && !LocationInFront(HearingLookAtLoc))
	{
		HearingLookAtLoc = FVector::ZeroVector;
		MostInterestingPoint = FVector::ZeroVector;
	}

	// If the AI is not currently looking at something, if the delay has run out, if the point is no longer infront of the actor
	// Select a new random location from the array and set the new location value to the MostInterestingPoint
	// Hearing locations have priority over the other locations
	if (Locations.Num() > 0 || !HearingLookAtLoc.IsZero())
	{
		if (LookAtPointDelay <= 0.0f || MostInterestingPoint.IsZero() || !LocationInFront(MostInterestingPoint) || (!HearingLookAtLoc.IsZero() && MostInterestingPoint != HearingLookAtLoc))
		{
			// Choose from locations array
			if (HearingLookAtLoc.IsZero())
			{
				int32 RandomInt = FMath::RandRange(0, (Locations.Num() - 1));

				// Location to look at
				MostInterestingPoint = Locations[RandomInt];

				LookAtPointDelay = LookAtDuration;
			}
			// Look at the hearing location
			else
			{
				if (ShouldDebugDraw)
				{
					DrawDebugSphere(GetWorld(), HearingLookAtLoc, 25.0f, 5, FColor::Red, false, 1.0f);
				}
				MostInterestingPoint = HearingLookAtLoc;
				LookAtPointDelay = LookAtDuration;
			}
		}
		else
		{
			LookAtPointDelay -= DeltaTime;

			// Set the vector to zero
			if (LookAtPointDelay <= 0.0f)
			{
				HearingLookAtLoc = FVector::ZeroVector;
			}
		}
	}
	else
	{
		// Set the point and rotation to zero when there is noting to look at
		MostInterestingPoint = FVector::ZeroVector;
		HearingLookAtLoc = FVector::ZeroVector;
		//LookAtRotation = ControlledPawn->GetActorRotation();/*FRotator::ZeroRotator;*/
	}

	// Rotation to look at the most interesting point
	if (!MostInterestingPoint.IsZero())
	{
		FVector TempVec = MostInterestingPoint - ControlledPawn->GetMesh()->GetSocketLocation("headSocket");
		LookAtRotation = FRotationMatrix::MakeFromX(TempVec).Rotator();
	}
	// If there is no point to look at, reset where to look
	else
	{
		LookAtRotation = ControlledPawn->GetActorRotation();
	}

}