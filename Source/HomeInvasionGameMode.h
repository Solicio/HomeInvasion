// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "HomeInvasion/Murderer/MurdererCharacter.h"
#include "HomeInvasion/Murderer/MurdererUtility/MurdererWaypoint.h"
#include "HomeInvasion/Murderer/MurdererUtility/MurdererWaypointZone.h"
#include "HomeInvasion/Murderer/MurdererUtility/LightZone.h"
#include "HomeInvasion/Murderer/MurdererUtility/DayOverrideZone.h"
#include "Miscellaneous/EnvironmentManager.h"
#include "Miscellaneous/LevelPhasesData.h"

#include "GameFramework/GameMode.h"
#include "HomeInvasionGameMode.generated.h"

UCLASS(minimalapi)
class AHomeInvasionGameMode : public AGameMode
{
	GENERATED_BODY()

	public:

		AHomeInvasionGameMode(const FObjectInitializer& ObjectInitializer);

		/** Add to the murderer array */
		UFUNCTION(BlueprintCallable, Category = "AI")
		void AddMurderer(AMurdererCharacter* Murderer);

		/** Remove from the murderer array */
		UFUNCTION(BlueprintCallable, Category = "AI")
		void RemoveMurderer(AMurdererCharacter* Murderer);

		// Stores references to all the murderers in the game
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		TArray<AMurdererCharacter*> Murderers;


		//--- Waypoints ---//

		/** Find as all the waypoints in the current level. */
		UFUNCTION(BlueprintCallable, Category = "Waypoints")
		void FindWaypoints(UWorld* World);

		/** Find as all the waypoint zones in the current level. */
		UFUNCTION(BlueprintCallable, Category = "Waypoints")
		void FindZones(UWorld* World);

		/**
		*	Select a new random waypoint from all available waypoints.
		*
		*	@param CurrentWaypoint The current waypoint of the AI
		*	@param UseZones If the function should use zones when finding which waypoint to select
		*	@param ZoneDuration	The time the player has to spend inside a Zone to make the AI select that zone
		*/
		UFUNCTION(BlueprintCallable, Category = "Waypoints")
		AMurdererWaypoint* SelectNewWaypoint(AMurdererWaypoint* CurrentWaypoint, bool UseZones, float ZoneDuration, int32 UsuableWaypointID);

		// All the waypoints in the current level.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoints")
		TArray<AMurdererWaypoint*> Waypoints;

		// All the waypoint zones in the current level.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoints")
		TArray<AMurdererWaypointZone*> Zones;


		//--- Settings ---//

		/** Applies FOV, Brightness, AA Method, Audio levels and various other settings */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Settings")
		void ApplyGeneralSettings(bool ApplySound = true, bool ApplyVideo = true, bool ApplyGeneral = true);


		//--- Environment ---//

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
		AEnvironmentManager* EnvironmentManager;

		/** Get the current time of the world, 0.0 -> 24.0 */
		UFUNCTION(BlueprintCallable, Category = "Environment")
		float GetCurrentWorldTime();

	
		//--- Phases ---//

		/** Hold the current levels data regarding to phases.
		*	Set in the blueprint begin play
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phases")
		ULevelPhasesData* LevelPhaseData;

		/** Sets the current phase of the level */
		UFUNCTION(BlueprintCallable, Category = "Phases")
		void SetPhase(const FString& NewPhaseName);

		/** Gets the name of the current phase */
		UFUNCTION(BlueprintCallable, Category = "Phases")
		FString GetCurrentPhase();


		//--- Utility ---//
		
		// Root, attach onto this
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utility")
		USceneComponent* RootComp;

		// Sphere component used for overlap checking
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Utility")
		USphereComponent* OverlapSphere;

		/** Checks if the specified location is overlapping a LightZone and checks if the zone is on. */
		UFUNCTION(BlueprintCallable, Category = "Utility")
		bool IsLocationInLight(FVector Location, bool CheckIfOn = true, float Radius = 10.0f);

		/** Checks if the specified location is overlapping a OverrideDay zone and checks if the zone is on. */
		UFUNCTION(BlueprintCallable, Category = "Utility")
		bool IsLocationInOverride(FVector Location, bool CheckIfOn = true, float Radius = 10.0f);

		/** Checks if the specified location is overlapping something that has the provided tag */
		UFUNCTION(BlueprintCallable, Category = "Utility")
		bool IsLocationOverlapping(FName Tag, FVector Location, float Radius = 10.0f);

	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
};



