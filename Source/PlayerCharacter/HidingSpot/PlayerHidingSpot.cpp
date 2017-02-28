// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PlayerHidingSpot.h"

#include "../FPSCharacter.h"

// Sets default values
APlayerHidingSpot::APlayerHidingSpot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = SceneRoot;

	// Create the overlap box used to check for player
	OverlapBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Overlap Box"));
	OverlapBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	OverlapBox->bGenerateOverlapEvents = true;
	OverlapBox->ComponentTags.Add("HidingSpot");
	//OverlapBox->AttachParent = RootComponent;
	OverlapBox->SetupAttachment(RootComponent);
	
	// Create arrow used to indicate and get location AI should run to
	AICheckLocationArrow = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("AI Check Location Arrow"));
	//AICheckLocationArrow->AttachParent = RootComponent;
	AICheckLocationArrow->SetupAttachment(RootComponent);
	AICheckLocationArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	Tags.Add("HidingSpot");

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerHidingSpot::OnOverlapBegin);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &APlayerHidingSpot::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APlayerHidingSpot::BeginPlay()
{
	Super::BeginPlay();

	AICheckLocation = AICheckLocationArrow->GetComponentLocation();
}

// Called every frame
void APlayerHidingSpot::Tick(float DeltaTime)
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

	//if (PlayerInside && SpotType == EHidingSpotTypes::EWardrobe)
	//	PrintText("Player Hiding, Duration:" +  FString::SanitizeFloat(PlayerInsideDuration), 250, FColor::Green);
}

void APlayerHidingSpot::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			PlayerInside = true;

			if (PlayerFPSCharacter == nullptr)
				PlayerFPSCharacter = Cast<AFPSCharacter>(OtherActor);

			PlayerFPSCharacter->InsideHidingSpot = true;
			PlayerFPSCharacter->CurrentHidingSpot = this;
		}
	}
}

void APlayerHidingSpot::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			PlayerInside = false;

			if (PlayerFPSCharacter == nullptr)
				PlayerFPSCharacter = Cast<AFPSCharacter>(OtherActor);

			PlayerFPSCharacter->InsideHidingSpot = false;
			PlayerFPSCharacter->CurrentHidingSpot = nullptr;
		}
	}
}