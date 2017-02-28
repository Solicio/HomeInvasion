// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "EnvironmentManager.h"

// Sets default values
AEnvironmentManager::AEnvironmentManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*DayCycleOn = true;*/
	StringTime = "";
	ActualTime = 0.0f;
}

// Called when the game starts or when spawned
void AEnvironmentManager::BeginPlay()
{
	Super::BeginPlay();
	
	//OneDay = DayLengthInMinutes * 60.0f;
	//OneHour = OneDay / 24.0f;

	//// Set the starting hour and calculate the pitch for the directional light
	//CurrentHour = StartHour;
	//LightPitch = ((CurrentHour / 6.0f) * 90.0f) + 90.0f;
	//UpdateSkySphere();

	//TempCounter = 0.0f;
}

// Called every frame
void AEnvironmentManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Calculate the actual time, Hours + Minutes
	//CurrentMinute = (CurrentHour - floor(CurrentHour)) * 60.0f;
	//ActualTime = floor(CurrentHour) + (CurrentMinute / 100.0f);

	// Put together the digital time string
	//FString MinutesString = FString::SanitizeFloat(CurrentMinute);
	//MinutesString = MinutesString.Mid(0, 2);

	//if (MinutesString.EndsWith("."))
	//	MinutesString = "0" + MinutesString.Mid(0, 1);
	//
	//FString HoursString = FString::SanitizeFloat(floor(ActualTime));
	//HoursString.RemoveFromEnd(".0");
	//	
	//if (HoursString.Len() == 1)
	//	HoursString = "0" + HoursString;

	//// To be used for prints and other times such as digital clocks.
	//StringTime = HoursString + ":" + MinutesString;

	if (PrintTime)
	{
		PrintText("Current Time: " + StringTime, -1, FColor::Green, 0.0f);
	}
}

float AEnvironmentManager::GetTime()
{
	return ActualTime;
}

FString AEnvironmentManager::GetStringTime()
{
	return StringTime;
}
