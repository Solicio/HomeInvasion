// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "IO_SoundObject.h"


// Sets default values
AIO_SoundObject::AIO_SoundObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create arrow used to indicate and get location AI should run to
	AICheckLocationArrow = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("AI Check Location Arrow"));
	//AICheckLocationArrow->AttachParent = RootComponent;
	AICheckLocationArrow->SetupAttachment(RootComponent);
	AICheckLocationArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	// Create the stimuli source component and make sure it's auto registering
	StimuliSourceComp = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(this, TEXT("StimuliSourceComp"));

	// Create the overlap box used to check for player
	OverlapBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Overlap Box"));
	OverlapBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	OverlapBox->bGenerateOverlapEvents = true;
	OverlapBox->ComponentTags.Add("SoundObject");
	//OverlapBox->AttachParent = RootComponent;
	OverlapBox->SetupAttachment(RootComponent);

	NoiseStrength = 70.0f;
	NoiseDelay = 1.0f;
	
	Name = "Noise Object";

	Tags.Add("SoundObject");
}

// Called when the game starts or when spawned
void AIO_SoundObject::BeginPlay()
{
	Super::BeginPlay();

	AICheckLocation = AICheckLocationArrow->GetComponentLocation();

	// Start a timer to check the reception with a specific interval
	GetWorldTimerManager().SetTimer(NoiseTimerHandle, this, &AIO_SoundObject::CreateNoise, NoiseDelay, true);
	GetWorldTimerManager().PauseTimer(NoiseTimerHandle);
}

// Called every frame
void AIO_SoundObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Create noise that the AI can hear
void AIO_SoundObject::CreateNoise()
{
	FName StimStrength = FName(*FString::SanitizeFloat(NoiseStrength));
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0, this, 0.0f, StimStrength);

	//PrintText("Created Noise", -1, FColor::Green, 1.0f);
	//UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), NoiseStrength, this, 20000.0f);
}

void AIO_SoundObject::Interact(AActor* InteractingActor)
{
	Super::Interact(InteractingActor);

	// Turn Off/On
	PlayingSound = !PlayingSound;

	// Start or stop the timer.
	if (GetWorldTimerManager().IsTimerPaused(NoiseTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(NoiseTimerHandle);
	}
	else
	{
		GetWorldTimerManager().PauseTimer(NoiseTimerHandle);
	}
}