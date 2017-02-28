// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "LightZone.h"

#include "AI/Navigation/NavAreas/NavArea_Obstacle.h"
#include "AI/Navigation/NavAreas/NavArea_Default.h"

// Sets default values
ALightZone::ALightZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	/*TriggerVolume = CreateDefaultSubobject<UShapeComponent>(TEXT("Trigger Volume"));*/
	//TriggerVolume->AttachParent = RootComponent;

	Tags.Add("LightZone");

	CheckLightsEvery = 1.0f;
}

// Called when the game starts or when spawned
void ALightZone::BeginPlay()
{
	Super::BeginPlay();
	
	// Start a timer to update the character stress level
	GetWorldTimerManager().SetTimer(CheckLightsTimer, this, &ALightZone::CheckLights, CheckLightsEvery, true);

	TArray<UActorComponent*> Components = GetComponentsByTag(UActorComponent::StaticClass(), "TriggerVolume");
	if (Components[0] != nullptr)
		TriggerShape = Cast<UShapeComponent>(Components[0]);

}

// Called every frame
void ALightZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ALightZone::CheckLights()
{
	LightsOn = false;

	for (auto Itr(ConnectedLights.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			if ((*Itr)->GetLightComponent()->IsVisible())
			{
				LightsOn = true;
			}
		}
	}
	
	if (TriggerShape != nullptr)
	{
		if (LightsOn && TriggerShape->AreaClass == UNavArea_Default::StaticClass())
		{
			TriggerShape->AreaClass = UNavArea_Obstacle::StaticClass();

			UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(GetWorld());
			if (NavSys)
			{
				//PrintText("Lets update", -1, FColor::Green, 2.0f);
				NavSys->UpdateComponentInNavOctree(*TriggerShape);
				//NavSys->UpdateNavOctree(this, UNavigationSystem::OctreeUpdate_Modifiers);
			}
		}
		else if(!LightsOn && TriggerShape->AreaClass == UNavArea_Obstacle::StaticClass())
		{
			TriggerShape->AreaClass = UNavArea_Default::StaticClass();

			UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(GetWorld());
			if (NavSys)
			{
				//PrintText("Lets update", -1, FColor::Green, 2.0f);
				NavSys->UpdateComponentInNavOctree(*TriggerShape);
				//NavSys->UpdateNavOctree(this, UNavigationSystem::OctreeUpdate_Modifiers);
			}
		}
	}

	LightCheck();
}
