// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "HomeInvasion.h"
#include "HomeInvasionGameMode.h"

AHomeInvasionGameMode::AHomeInvasionGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Set default player controller class to our blueprint
	//static ConstructorHelpers::FClassFinder<APlayerController> ControllerBPClass(TEXT("/Game/Player/Game/BP_PlayerController"));
	//if (ControllerBPClass.Class != NULL)
	//{
	//	PlayerControllerClass = ControllerBPClass.Class;
	//}
	
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/Game/BP_FPSCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	// Create root component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	//OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	//OverlapSphere->SetSphereRadius(10.0f);
	//OverlapSphere->bGenerateOverlapEvents = true;
	//OverlapSphere->AttachParent = RootComponent;
}

void AHomeInvasionGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Look through the current level and store the zones and waypoints
	FindWaypoints(GetWorld());
	FindZones(GetWorld());
}

void AHomeInvasionGameMode::FindWaypoints(UWorld* World)
{
	// Store all waypoints

	for (TActorIterator<AMurdererWaypoint> It(World); It; ++It)
	{
		// Check if the object is valid
		if (It->IsValidLowLevel())
		{
			Waypoints.Push(*It);
		}
	}
}

void AHomeInvasionGameMode::FindZones(UWorld* World)
{
	// Store all zones

	for (TActorIterator<AMurdererWaypointZone> It(World); It; ++It)
	{
		// Check if the object is valid
		if (It->IsValidLowLevel())
		{
			Zones.Push(*It);
		}
	}

	for (int i = 0; i < Zones.Num(); i++)
	{
		if (Zones[i] != nullptr)
			Zones[i]->SetID(i);
	}
}

void AHomeInvasionGameMode::AddMurderer(AMurdererCharacter* Murderer)
{
	if (!Murderers.Contains(Murderer) && Murderer != nullptr)
	{
		Murderers.Add(Murderer);
	}
}

void AHomeInvasionGameMode::RemoveMurderer(AMurdererCharacter* Murderer)
{
	if (Murderers.Contains(Murderer) && Murderer != nullptr)
	{
		Murderers.Remove(Murderer);
	}
}

AMurdererWaypoint* AHomeInvasionGameMode::SelectNewWaypoint(AMurdererWaypoint* CurrentWaypoint, bool UseZones, float ZoneDuration, int32 UsableWaypointID)
{
	if (Waypoints.Num() == 0)
	{
		return nullptr;
	}

	TArray<AMurdererWaypoint*> AvailableWaypoints;
	TArray<AMurdererWaypointZone*> AvailableZones;

	// Using Zones to find waypoint
	if (UseZones)
	{
		for (int i = 0; i < Zones.Num(); i++)
		{
			// If the player has been in the current Zone for the right amount of time
			if (Zones[i] != nullptr && Zones[i]->PlayerInsideDuration >= ZoneDuration)
			{
				// Add it into the array of Zones
				AvailableZones.Add(Zones[i]);
			}
		}

		// Look through the available Zones
		for (int i = 0; i < AvailableZones.Num(); i++)
		{
			// Look through all the available Zones waypoints
			for (int j = 0; j < AvailableZones[i]->Waypoints.Num(); j++)
			{
				// If they are active, is not the current waypoint and has matching IDs, add them into the array
				if ((AvailableZones[i]->Waypoints[j]->Active) && (AvailableZones[i]->Waypoints[j] != CurrentWaypoint) && (AvailableZones[i]->Waypoints[j]->WaypointID == UsableWaypointID))
				{
					// Make sure there aren't duplicates
					if (!AvailableWaypoints.Contains(AvailableZones[i]->Waypoints[j]))
					{
						AvailableWaypoints.Add(AvailableZones[i]->Waypoints[j]);
					}
				}
			}
		}
	}

	// If the array is empty, look through all waypoints. (If UseZones, there was no available waypoints)
	if (AvailableWaypoints.Num() == 0)
	{
		// Look through Waypoints to see if they are active, is not the current waypoint and if the IDs match
		for (int i = 0; i < Waypoints.Num(); i++)
		{
			if (Waypoints[i] != nullptr && Waypoints[i]->Active && Waypoints[i] != CurrentWaypoint && Waypoints[i]->WaypointID == UsableWaypointID)
			{
				// Make sure there aren't duplicates
				if (!AvailableWaypoints.Contains(Waypoints[i]))
				{
					AvailableWaypoints.Add(Waypoints[i]);
				}
			}
		}

		// If there is no available waypoint to travel to, return
		if (AvailableWaypoints.Num() == 0)
		{
			return nullptr;
		}
	}

	// Select a random ID from all the all the waypoints in the array
	int32 RandomInt = FMath::RandRange(0, (AvailableWaypoints.Num() - 1));

	// Get the new waypoint using the random value
	AMurdererWaypoint* NewWaypoint = nullptr;
	NewWaypoint = AvailableWaypoints[RandomInt];

	//PrintText("Av Zones: " + FString::FromInt(AvailableZones.Num()) + ", Av Waypoints: " + FString::FromInt(AvailableWaypoints.Num()), 90, FColor::Yellow, 3.0f);

	// Send it back to the AI
	return NewWaypoint;
}

float AHomeInvasionGameMode::GetCurrentWorldTime()
{
	if(EnvironmentManager == nullptr)
		return 0.0f;

	return EnvironmentManager->GetTime();
}

void AHomeInvasionGameMode::SetPhase(const FString& NewPhaseName)
{
	if (LevelPhaseData == nullptr)
		return;

	// Look through the phases array to find a name match, then set the machting phase to the current one
	for (int i = 0; i < LevelPhaseData->Phases.Num(); i++)
	{
		if (LevelPhaseData->Phases[i].PhaseName == NewPhaseName)
		{
			LevelPhaseData->CurrentPhase = LevelPhaseData->Phases[i];
		}
	}
}

FString AHomeInvasionGameMode::GetCurrentPhase()
{
	if (LevelPhaseData == nullptr)
		return "nullptr";

	return LevelPhaseData->CurrentPhase.PhaseName;
}

bool AHomeInvasionGameMode::IsLocationInLight(FVector Location, bool CheckIfOn, float Radius)
{
	if (OverlapSphere == nullptr)
		return false;

	// Set the radius to the provided one, for cases where the area being checked needs to be bigger
	OverlapSphere->SetSphereRadius(Radius);

	// Move the sphere to the desired location that is going to be checked
	OverlapSphere->SetWorldLocation(Location);

	// Get all actors that are overlapping the sphere
	TArray<AActor*> OverlappingActors;
	OverlapSphere->GetOverlappingActors(OverlappingActors);

	//TArray<UPrimitiveComponent*> OverlappingComponents;
	//OverlapSphere->GetOverlappingComponents(OverlappingComponents);

	// Loop through the array of actors and check the tags for the right one
	for (auto Itr(OverlappingActors.CreateConstIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr && (*Itr)->ActorHasTag("LightZone"))
		{
			if (CheckIfOn)
			{
				if(Cast<ALightZone>((*Itr))->LightsOn)
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}

	//for (auto Itr(OverlappingComponents.CreateConstIterator()); Itr; Itr++)
	//{
	//	if ((*Itr) != nullptr && (*Itr)->ComponentHasTag("LightZoneTrigger"))
	//	{
	//		if (CheckIfOn)
	//		{
	//			if (Cast<ALightZone>((*Itr)->GetOwner())->LightsOn)
	//			{
	//				return true;
	//			}
	//		}
	//		else
	//		{
	//			return true;
	//		}
	//	}
	//}

	// Move the component back to 0
	OverlapSphere->SetWorldLocation(FVector::ZeroVector);

	return false;
}

bool AHomeInvasionGameMode::IsLocationInOverride(FVector Location, bool CheckIfOn, float Radius)
{
	if (OverlapSphere == nullptr)
		return false;

	// Set the radius to the provided one, for cases where the area being checked needs to be bigger
	OverlapSphere->SetSphereRadius(Radius);

	// Move the sphere to the desired location that is going to be checked
	OverlapSphere->SetWorldLocation(Location);

	// Get all actors that are overlapping the sphere
	TArray<AActor*> OverlappingActors;
	OverlapSphere->GetOverlappingActors(OverlappingActors);

	// Loop through the array of actors and check the tags for the right one
	for (auto Itr(OverlappingActors.CreateConstIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr && (*Itr)->ActorHasTag("DayOverride"))
		{
			if (CheckIfOn)
			{
				if (Cast<ADayOverrideZone>((*Itr))->ShouldOverride)
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}

	// Move the component back to 0
	OverlapSphere->SetWorldLocation(FVector::ZeroVector);

	return false;
}

bool AHomeInvasionGameMode::IsLocationOverlapping(FName Tag, FVector Location, float Radius)
{
	if (OverlapSphere == nullptr)
		return false;

	// Set the radius to the provided one, for cases where the area being checked needs to be bigger
	OverlapSphere->SetSphereRadius(Radius);

	// Move the sphere to the desired location that is going to be checked
	OverlapSphere->SetWorldLocation(Location);

	// Get all actors that are overlapping the sphere
	TArray<AActor*> OverlappingActors;
	OverlapSphere->GetOverlappingActors(OverlappingActors);

	// Loop through the array of actors and check the tags for the right one
	for (auto Itr(OverlappingActors.CreateConstIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr && (*Itr)->ActorHasTag(Tag))
		{
			return true;
		}
	}
	
	// Move the component back to 0
	OverlapSphere->SetWorldLocation(FVector::ZeroVector);

	return false;
}