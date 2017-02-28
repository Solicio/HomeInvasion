// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PlayerPhone.h"


// Sets default values
APlayerPhone::APlayerPhone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	RootComp = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	SetRootComponent(RootComp);

	// Create the main mesh for the phone
	PhoneMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PhoneMesh"));
	//PhoneMesh->AttachParent = RootComponent;
	PhoneMesh->SetupAttachment(RootComponent);

	// Create the spotlight
	Flashlight = ObjectInitializer.CreateDefaultSubobject<USpotLightComponent>(this, "Flashlight");
	//Flashlight->AttachParent = RootComponent;
	Flashlight->SetupAttachment(RootComponent);

	// Create the stimuli source component and make sure it's auto registering
	StimuliSourceComp = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(this, TEXT("StimuliSourceComp"));

	// Create a component for doing a 3d pointer to the source of the damage
	PhoneScreenWidget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("PhoneScreen"));
	//PhoneScreenWidget->AttachParent = RootComponent;
	PhoneScreenWidget->SetupAttachment(RootComponent);

	// Create box component used to check for reception
	OverlapBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Overlap Box"));
	OverlapBox->SetBoxExtent(FVector(20.0f, 20.0f, 20.0f));
	OverlapBox->bGenerateOverlapEvents = true;
	//OverlapBox->AttachParent = RootComponent;
	OverlapBox->SetupAttachment(RootComponent);

	// Trace collision parameters
	TraceParams = FCollisionQueryParams(FName(TEXT("Flashlight Trace")), false, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActor(this);

	Tags.Add("PlayerPhone");

	BatteryPercentage = 100.0f;
	BatteryDecrease = 0.5f;
	FlashlightMultiplier = 1.2f;

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerPhone::OnOverlapBegin);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &APlayerPhone::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APlayerPhone::BeginPlay()
{
	Super::BeginPlay();
	
	// Store the userwidget that is in the widget component
	PhoneWidget = Cast<UPlayerPhoneWidget>(PhoneScreenWidget->GetUserWidgetObject());

	if (PhoneWidget != nullptr)
	{
		// Set the player phone in the userwidget
		PhoneWidget->PlayerPhone = this;
		PhoneScreenWidget->SetVisibility(false);
	}

	Flashlight->SetVisibility(false);
	PhoneMesh->SetVisibility(false);
	IsActive = false;

	// Set the length of the trace to the length of the light
	TraceLength = Flashlight->AttenuationRadius;

	// Start a timer to check the reception with a specific interval
	GetWorldTimerManager().SetTimer(ReceptionCheckTimer, this, &APlayerPhone::CheckReceptionZones, 1.0f, true);

	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

// Called every frame
void APlayerPhone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsActive)
	{
		if (BatteryPercentage > 0.0f)
		{
			if (Flashlight->IsVisible())
				BatteryPercentage -= BatteryDecrease * FlashlightMultiplier * DeltaTime;
			else
				BatteryPercentage -= BatteryDecrease * DeltaTime;
		}
		else
		{
			// Set the phone to dead if there is no battery
			BatteryPercentage = 0.0f;
			PhoneDead = true;
		}
	}

	if (IsActive && Flashlight->IsVisible())
	{
		// Trace between the phone and the light length
		FHitResult HitResult(ForceInit);
		FVector StartFVector = Flashlight->GetComponentLocation();
		FVector EndFVector = Flashlight->GetComponentLocation() + (Flashlight->GetForwardVector() * TraceLength);

		// Trace straight forward for the end point and half point locations
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
		{
			// Set location used to place visibility points
			EndPointLocation = Flashlight->GetComponentLocation() + (Flashlight->GetForwardVector() * (HitResult.Distance * 0.9f));
			HalfPointLocation = Flashlight->GetComponentLocation() + (Flashlight->GetForwardVector() * (HitResult.Distance * 0.4f));
		}
		else
		{
			EndPointLocation = EndFVector;
			HalfPointLocation = Flashlight->GetComponentLocation() + (Flashlight->GetForwardVector() * (TraceLength * 0.5f));
		}

		// Loop through all the characters inside the flashlight cone to check LoS
		for (auto Itr(MurdererCharacters.CreateConstIterator()); Itr; Itr++)
		{
			if ((*Itr) != nullptr)
			{
				// End should be at the character which is checked.
				EndFVector = (*Itr)->GetMesh()->GetSocketLocation("headSocket");

				if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
				{
					// If the trace hits the character which is checked for LoS, nothing is blocking the light
					if (HitResult.GetActor() != nullptr && HitResult.GetActor() == (*Itr))
					{
						(*Itr)->FlashlightOnSelf();
					}
				}
			}
		}
		
	}
	else
	{
		EndPointLocation = FVector(0.0f, 0.0f, -2000.0f);
		HalfPointLocation = FVector(0.0f, 0.0f, -2000.0f);
	}
}

// Player input sending to the widget
void APlayerPhone::Use()
{
	if (PhoneWidget != nullptr)
		PhoneWidget->UseApp();
}

void APlayerPhone::ScrollUp()
{
	if (PhoneWidget != nullptr)
		PhoneWidget->ScrollUp();
}

void APlayerPhone::ScrollDown()
{
	if (PhoneWidget != nullptr)
		PhoneWidget->ScrollDown();
}

void APlayerPhone::TakeOutPhone(bool TakingItOut)
{
	IsActive = TakingItOut;
	PhoneMesh->SetVisibility(TakingItOut);
	PhoneScreenWidget->SetVisibility(TakingItOut);
	Flashlight->SetVisibility(false);

	if (PhoneWidget != nullptr)
		PhoneWidget->TakeOutPhone(TakingItOut);
}

void APlayerPhone::SetBatteryPercentage(float Percentage)
{
	BatteryPercentage = Percentage;

	if (BatteryPercentage > 0.0f)
		PhoneDead = false;
}

void APlayerPhone::CheckReceptionZones()
{
	if (!IsActive)
	{
		return;
	}

	int32 StrongestReception = 0;

	// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	OverlapBox->GetOverlappingActors(OverlappingActors, APhoneReceptionZone::StaticClass());

	// Look though all of them
	APhoneReceptionZone* TempZone = nullptr;
	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		TempZone = Cast<APhoneReceptionZone>(OverlappingActors[i]);
		if (TempZone != nullptr)
		{
			// If the temp reception is higher than the strongest, set the new strongest
			if (TempZone->ReceptionStrength > StrongestReception)
				StrongestReception = TempZone->ReceptionStrength;
		}
	}

	// Store the new strongest value
	StrongestReception = FMath::Clamp(StrongestReception, 0, 5);
	CurrentReception = StrongestReception;
}

// Create noise that the AI can hear
void APlayerPhone::CreateNoise(FVector Location, float Loudness)
{
	FName StimStrength = FName(*FString::SanitizeFloat(Loudness));
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0, this, 0.0f, StimStrength);
	
	//UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Loudness, this, 20000.0f);
}

void APlayerPhone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{

	}
}

void APlayerPhone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{

	}
}