// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EnvironmentManager.generated.h"

UCLASS()
class HOMEINVASION_API AEnvironmentManager : public AActor
{
	GENERATED_BODY()
	
	public:	

		// Sets default values for this actor's properties
		AEnvironmentManager();

		//// If the time should pass
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		//bool DayCycleOn;

		//// Which hour of the day the time should start at.
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		//float StartHour;

		//// How long a 24 hour long day should be in the game, specified in minutes.
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		//float DayLengthInMinutes;

		// If the actor should print out the current time
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		bool PrintTime;


		// The actual time in the level, 15.20, 24.00 etc..
		UPROPERTY(BlueprintReadWrite, Category = "Time")
		float ActualTime;

		// The actual time in the level, in readable string form, 15:20, 13:35 etc...
		UPROPERTY(BlueprintReadWrite, Category = "Time")
		FString StringTime;

		// By how much the player's cold level should increase or decrease when outside, different by weather.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weather")
		float TemperatureModifier;


		//---- Functions ----//

		/** Set the time of the level, also updates the sky */
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Time")
		void SetTime(float Hour, bool NewWeather = false);

		/** Get the current time */
		UFUNCTION(BlueprintPure, Category = "Time")
		float GetTime();

		/** Get the current time in string form */
		UFUNCTION(BlueprintPure, Category = "Time")
		FString GetStringTime();

	private:


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
};
