// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "IO_PickupObject.h"

// Sets default values
AIO_PickupObject::AIO_PickupObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set the needed mesh physics variables
	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(0.1f);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	Mesh->SetNotifyRigidBodyCollision(true);
	//Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);

	// Create the stimuli source component and make sure it's auto registering
	StimuliSourceComp = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(this, TEXT("StimuliSourceComp"));

	Tags.Add("PickupObject");

	Name = "Throwable Object";

	SoundCooldown = 0.2f;
	NoiseCooldown = 1.0f;
	ThrownNoiseStrength = 75.0f;
	CollisionNoiseStrength = 20.0f;
	PlayerCameraDownZ = 50.0f;

	OnActorHit.AddDynamic(this, &AIO_PickupObject::HitOtherActor);
}

// Called when the game starts or when spawned
void AIO_PickupObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AIO_PickupObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Speed at which the door is moving at
	ObjectSpeed = Mesh->GetComponentVelocity().Size();

	//if (Stationary)
	//	PrintText("Stationary Object", -1, FColor::Green, 0.0f);

	//if (Thrown)
	//	PrintText("Thrown Object", -1, FColor::Cyan, 0.0f);

	// When the object has stopped moving for .3 seconds, turn of physics
	if (ObjectSpeed == 0.0f)
	{
		DurationNotMoving += DeltaTime;

		// Turn of physics and set it to a stationary object
		if (!PlayerHeld && DurationNotMoving >= 0.3f)
		{
			Mesh->SetSimulatePhysics(false);
			Stationary = true;
		}
	}
	else
	{
		DurationNotMoving = 0.0f;
		Stationary = false;
		HasBeenMoved = true;
	}

	if (SoundCDCountdown > 0.0f)
	{
		SoundCDCountdown -= DeltaTime;
	}

	if (NoiseCDCountdown > 0.0f)
	{
		NoiseCDCountdown -= DeltaTime;
	}
}

void AIO_PickupObject::Interact(AActor* InteractingActor)
{
	Super::Interact(InteractingActor);

}

// Create noise that the AI can hear
void AIO_PickupObject::CreateNoise(FVector Location, float Loudness)
{
	FName StimStrength = FName(*FString::SanitizeFloat(Loudness));
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0, this, 0.0f, StimStrength);

	//UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Loudness, this, 20000.0f);
}

void AIO_PickupObject::HitOtherActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	// If the object collides with an object that is not the murderer, turn thrown to false
	if (OtherActor == nullptr || (OtherActor != nullptr && !OtherActor->ActorHasTag("Murderer")))
	{
		if (Thrown)
		{
			// If the object was thrown before collision, use the thrown noise strength
			Thrown = false;
			if (NoiseCDCountdown <= 0.0f)
			{
				CreateNoise(FVector(Mesh->GetComponentLocation().X, Mesh->GetComponentLocation().Y, Mesh->GetComponentLocation().Z + 50.0f), ThrownNoiseStrength);
				NoiseCDCountdown = NoiseCooldown;

				Collision(true, ObjectSpeed, false);
			}
		}
		else
		{
			// Else use the normal collision strength
			if (ObjectSpeed >= 100.0f && NoiseCDCountdown <= 0.0f)
			{
				CreateNoise(FVector(Mesh->GetComponentLocation().X, Mesh->GetComponentLocation().Y, Mesh->GetComponentLocation().Z + 50.0f), CollisionNoiseStrength);
				NoiseCDCountdown = NoiseCooldown;
				
				Collision(false, ObjectSpeed, false);
			}
		}
	}
	// Hit murderer
	else if (OtherActor != nullptr && OtherActor->ActorHasTag("Murderer"))
	{
		Collision(false, ObjectSpeed, true);
	}

	if (SoundCDCountdown <= 0.0f)
	{
		SoundCDCountdown = SoundCooldown;
		PlaySound(ObjectSpeed);
	}
}