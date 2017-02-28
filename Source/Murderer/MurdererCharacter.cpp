// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "MurdererCharacter.h"
#include "MurdererController.h"
#include "../InteractableObjects/IO_PickupObject.h"
#include "MurdererUtility/LightZone.h"

// Sets default values
AMurdererCharacter::AMurdererCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Setting the default controller class of Pawn
	/*AIControllerClass = AMurdererController::StaticClass();*/
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxAcceleration = 200.0f;
	bUseControllerRotationYaw = false;

	// Sensing
	HearingRange = 1000.0f;
	SightRange = 1500.0f;
	LoseSightRange = 1600.0f;
	PeripheralVisionAngle = 70.0f;

	// Attacking
	AttackRange = 200.0f;
	CanAttack = true;

	// Movement
	NormalMoveSpeed = 200.0f;
	ForcedInterestMoveSpeed = 350.0f;
	ChaseMoveSpeed = 500.0f;

	// Add tag to the AI which the actor can be recognized as.
	Tags.Add("Murderer");
	
	ExecutedOnce = false;

	SoundCheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SoundCheckSphere"));
	SoundCheckSphere->SetSphereRadius(10.0f);
	SoundCheckSphere->bGenerateOverlapEvents = true;
	/*SoundCheckBox->AttachParent = RootComponent;*/
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMurdererCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMurdererCharacter::OnOverlapEnd);
	OnActorHit.AddDynamic(this, &AMurdererCharacter::HitOtherActor);
}

// Called when the game starts or when spawned
void AMurdererCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Current controller that controls this character
	CurrentController = Cast<AMurdererController>(GetController());
	
	// Find and store the player
	//for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; ++it)
	//{
	//	AFPSCharacter* TestPawn = Cast<AFPSCharacter>(*it);
	//	if (TestPawn != nullptr)
	//	{
	//		PlayerCharacter = TestPawn;
	//	}
	//}
}

// Called every frame
void AMurdererCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//if (PlayerFlashlightOnBody > 0.0f)
	//{
	//	PlayerFlashlightOnBody -= DeltaTime;
	//}

	//if (PlayerCharacter != nullptr)
	//{
	//	// If the player is holding an object, make it so that the AI does not block that type of items.
	//	if (PlayerCharacter->HoldingObject)
	//	{
	//		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	//	}
	//	else if (!PlayerCharacter->HoldingObject)
	//	{
	//		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	//	}
	//}

	if (CurrentController != nullptr && !ExecutedOnce)
	{
		// Check to see if the character started in a day override zone
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors, ADayOverrideZone::StaticClass());
		bool InsideDayOverride = false;

		for (auto Itr(OverlappingActors.CreateIterator()); Itr; Itr++)
		{
			if ((*Itr) != nullptr && (*Itr)->ActorHasTag("DayOverride"))
			{
				if (Cast<ADayOverrideZone>((*Itr))->ShouldOverride)
				{
					InsideDayOverride = true;
				}
			}
		}

		CurrentController->SetDayOverride(InsideDayOverride);
		ExecutedOnce = true;
	}
}

void AMurdererCharacter::AddToAggro(float Value)
{
	AMurdererController* MController = Cast<AMurdererController>(GetController());
	if (MController != nullptr)
	{
		MController->AddToAggro(Value);
	}
}

void AMurdererCharacter::FlashlightOnSelf()
{
	AMurdererController* MController = Cast<AMurdererController>(GetController());
	if (MController != nullptr)
	{
		MController->FlashlightOnSelf();
	}
}

void AMurdererCharacter::GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const
{	
	out_Location = GetMesh()->GetSocketLocation("headSocket");
	out_Rotation = GetMesh()->GetSocketRotation("headSocket");
}

void AMurdererCharacter::ForceInterestInLoc(FVector Location)
{
	AMurdererController* MController = Cast<AMurdererController>(GetController());

	if (MController != nullptr)
	{
		MController->ForceInterestInLoc(Location);
	}
}

void AMurdererCharacter::SetOutlineActive(bool Active, int32 StencilValue)
{
	GetMesh()->SetRenderCustomDepth(Active);
	GetMesh()->CustomDepthStencilValue = StencilValue;
}

void AMurdererCharacter::HitOtherActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("Player"))
	{
		AMurdererController* MController = Cast<AMurdererController>(GetController());
		if (MController != nullptr)
		{
			MController->AddToAggro(100.0f);
		}
	}
	else if (OtherActor != nullptr && OtherActor->ActorHasTag("PickupObject"))
	{
		// If the AI collides with the object and it has not been thrown, start overlapping it
		AIO_PickupObject* TempObject = Cast<AIO_PickupObject>(OtherActor);
		if (!TempObject->Thrown)
		{
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
		}
		// If it was thrown, stun.
		else if (TempObject->Thrown && CurrentController != nullptr)
		{
			//TempObject->Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
			CurrentController->AddToAggro(100.0f);
			CurrentController->Stun();
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
		}

		if (CurrentController != nullptr && TempObject->PlayerHeld)
		{
			CurrentController->AddToAggro(100.0f);
		}
	}
}

void AMurdererCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("PickupObject"))
	{
		AIO_PickupObject* TempObject = Cast<AIO_PickupObject>(OtherActor);
		// The character should collide with the object if it has recently been thrown.
		if (TempObject->Thrown && CurrentController != nullptr)
		{
			TempObject->Mesh->SetPhysicsLinearVelocity(TempObject->Mesh->GetPhysicsLinearVelocity() * 0.5f, false);
			CurrentController->AddToAggro(100.0f);
			CurrentController->Stun();
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
		}

		if (CurrentController != nullptr && TempObject->PlayerHeld)
		{
			CurrentController->AddToAggro(100.0f);
		}
	}
	// Walked into a light Zone
	else if (OverlappedComponent != nullptr && /*OtherActor->ActorHasTag("LightZone")*/ OtherComp->ComponentHasTag("LightZoneTrigger") && CurrentController != nullptr)
	{
		// Needs to be a human to be affected by the Zone
		if (CurrentController->IsHumanoid && Cast<ALightZone>(OtherActor)->LightsOn)
		{
			// Flee from the current location
			CurrentController->FleeFromLocation(GetActorLocation());
		}
	}
	// Walked into a day override zone
	else if (OtherActor != nullptr && OtherActor->ActorHasTag("DayOverride") && CurrentController != nullptr)
	{
		if (Cast<ADayOverrideZone>(OtherActor)->ShouldOverride)
		{
			CurrentController->SetDayOverride(true);
		}
	}
}

void AMurdererCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("DayOverride") && CurrentController != nullptr)
	{
		// Check to see if the character is not in another zone before setting the value to false
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);
		bool InsideDayOverride = false;

		for (auto Itr(OverlappingActors.CreateIterator()); Itr; Itr++)
		{
			if ((*Itr) != nullptr && (*Itr)->ActorHasTag("DayOverride") && (*Itr) != OtherActor)
			{
				if (Cast<ADayOverrideZone>((*Itr))->ShouldOverride)
				{
					InsideDayOverride = true;
				}
			}
		}

		CurrentController->SetDayOverride(InsideDayOverride);
	}
}