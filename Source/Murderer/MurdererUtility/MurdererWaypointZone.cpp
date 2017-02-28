// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MurdererWaypointZone.h"


// Sets default values
AMurdererWaypointZone::AMurdererWaypointZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = SceneRoot;

	OverlapBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Overlap Box"));
	OverlapBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	OverlapBox->ComponentTags.Add("WaypointZoneOverlap");
	OverlapBox->bGenerateOverlapEvents = true;
	//OverlapBox->AttachParent = RootComponent;
	OverlapBox->SetupAttachment(RootComponent);

	Tags.Add("WaypointZone");

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AMurdererWaypointZone::OnOverlapBegin);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &AMurdererWaypointZone::OnOverlapEnd);
}

void AMurdererWaypointZone::SetID(int32 NewID)
{
	ZoneID = NewID;
}

// Called when the game starts or when spawned
void AMurdererWaypointZone::BeginPlay()
{
	Super::BeginPlay();
	
	// Find all waypoints inside of this Zone
	TArray<AActor*> OverlappingActors;
	OverlapBox->GetOverlappingActors(OverlappingActors, AMurdererWaypoint::StaticClass());

	AMurdererWaypoint* TempWaypoint;
	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		TempWaypoint = Cast<AMurdererWaypoint>(OverlappingActors[i]);
		if (TempWaypoint)
		{
			Waypoints.Add(TempWaypoint);
		}
	}
}

// Called every frame
void AMurdererWaypointZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerInside)
	{
		PlayerInsideDuration += DeltaTime;
	}
	else
	{
		PlayerInsideDuration = 0.0f;
	}

	if (ShouldPrintInfo && PlayerInside)
		PrintText("Player In Zone: " + FString::FromInt(ZoneID) + ", WPs in Zone: " + FString::FromInt(Waypoints.Num()) + ", Duration: " + FString::SanitizeFloat(PlayerInsideDuration), (120 + ZoneID), FColor::Green);
}

void AMurdererWaypointZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			PlayerInside = true;
		}
	}
}

void AMurdererWaypointZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			PlayerInside = false;
		}
	}
}