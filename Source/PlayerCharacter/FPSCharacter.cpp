// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "FPSCharacter.h"

// Sets default values
AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	//FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	bUseControllerRotationPitch = false;

	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	//CameraBoom->AttachTo(GetCapsuleComponent());
	CameraBoom->SetupAttachment(GetCapsuleComponent());


	FirstPersonCameraComponent->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform);
	
	// Create shadow mesh and set mesh settings
	ShadowMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("ShadowMesh"));
	//ShadowMesh->AttachParent = GetCapsuleComponent();
	ShadowMesh->SetupAttachment(GetCapsuleComponent());
	ShadowMesh->bCastHiddenShadow = true;
	ShadowMesh->bOwnerNoSee = true;

	// Crate physics handle
	PickupPhysicsHandle = ObjectInitializer.CreateDefaultSubobject<UPhysicsHandleComponent>(this, TEXT("PickupHandle"));
	PickupPhysicsHandle->InterpolationSpeed = 10.0f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);

	GetMesh()->SetCastShadow(false);
	GetMesh()->bOnlyOwnerSee = true;

	// Crouching
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	// Movement noise
	WalkingNoiseStrength = 15.0f;
	SlowWalkingNoiseStrength = 10.0f;
	SprintingNoiseStrength = 25.0f;
	StepOverrideNoiseMulti = 1.0f;

	// General variables for throwable pickupobjects
	DistanceFromPlayer = 150.0f;
	MaxDistanceFromPlayer = 200.0f;
	ThrowForce = 13000.0f;
	CameraDownZ = 50.0f;

	// Falling
	FallingDeathLimit = 1.5f;

	// Breathing
	BreathDelay = 3.0f;
	MaxBreathDelay = 3.0f;
	BreathNoiseStrength = 2.0f;
	ShouldBreathe = true;

	// Cold
	ManageColdPercentage = true;
	ColdPercentage = 100.0f;
	NormalDecrease = 1.0f;
	NightDecrease = 2.0f;
	WetClothesDecrease = 2.0f;
	LowEnergyDecrease = 1.0f;
	LowEnergy = 25.0f;
	ColdMovementMulti = 0.4f;

	// Energy
	ManageEnergyPercentage = true;
	EnergyPercentage = 100.0f;
	IdleDecrease = 0.5f;
	WalkingSlowDecrease = 1.0f;
	WalkingDecrease = 1.5f;
	RunningDecrease = 2.0f;

	// Blood corruption
	Blood = 100.0f;

	BloodRushDuration = 5.0f;
	BloodRushSpeedMulti = 1.5f;
	BloodRushNoiseMulti = 1.5f;

	LockPlayerFor = 1.0f;
	DashStrength = 2500.0f;

	HunterVisionDuration = 5.0f;
	HunterVisionRange = 5000.0f;

	BeastCallingRange = 8000.0f;

	VampiricHungerEnergyGain = 50.0f;

	// Add tag to the player which the actor can be recognized as.
	Tags.Add("Player");
	ShouldBeIgnored = false;
	CanBeKilled = true;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSCharacter::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Visibility points
	APlayerVisiblityPoints* SpawnedBP = nullptr;
	for (int i = 0; i < 9; i++)
	{
		SpawnedBP = SpawnBP<APlayerVisiblityPoints>(GetWorld(), VisibilityPointBP, FVector::ZeroVector, FRotator::ZeroRotator);
		//SpawnedBP->AttachRootComponentToActor(this);
		VisibilityPoints.Add(SpawnedBP);

		switch (i)
		{
			case 0:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("middleHeadSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("middleHeadSocket"));
				SpawnedBP->SocketName = "middleHeadSocket";
				break;

			case 1:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("torsoSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("torsoSocket"));
				SpawnedBP->SocketName = "torsoSocket";
				break;

			case 2:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("pelvisSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("pelvisSocket"));
				SpawnedBP->SocketName = "pelvisSocket";
				break;

			case 3:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("hand_rSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_rSocket"));
				SpawnedBP->SocketName = "hand_rSocket";
				break;

			case 4:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("hand_lSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_lSocket"));
				SpawnedBP->SocketName = "hand_lSocket";
				break;

			case 5:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("calf_rSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("calf_rSocket"));
				SpawnedBP->SocketName = "calf_rSocket";
				break;

			case 6:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("calf_lSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("calf_lSocket"));
				SpawnedBP->SocketName = "calf_lSocket";
				break;

			case 7:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("foot_rSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("foot_rSocket"));
				SpawnedBP->SocketName = "foot_rSocket";
				break;

			case 8:
				//SpawnedBP->AttachRootComponentTo(ShadowMesh, FName("foot_lSocket"), EAttachLocation::SnapToTarget);
				SpawnedBP->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("foot_lSocket"));
				SpawnedBP->SocketName = "foot_lSocket";
				break;
		};

		VisibilityPointsNum ++;
	}

	// Game Mode
	CurrentGameMode = Cast<AHomeInvasionGameMode>(GetWorld()->GetAuthGameMode());

	// Controller
	if (Controller != nullptr)
		CurrentPlayerController = Cast<AFPSPlayerController>(Controller);

	// Phone
	if (StartWithPhone && PlayerPhone == nullptr && PhoneClassBP != nullptr)
	{
		PlayerPhone = SpawnBP<APlayerPhone>(GetWorld(), PhoneClassBP, FVector::ZeroVector, FRotator::ZeroRotator);

		if (PlayerPhone != nullptr)
		{
			HasPhone = true;
			HasPhoneOut = false;
			CurrentPlayerController->CharacterHasPhone = true;
		}
	}

	// Inventory, set the starting size
	Inventory.Init(nullptr, 20);

	// Start a timer to update the character stress level
	GetWorldTimerManager().SetTimer(StressTimer, this, &AFPSCharacter::UpdateStressLevel, 1.0f, true);

	// Start a timer to update the character cold percentage
	GetWorldTimerManager().SetTimer(ColdTimer, this, &AFPSCharacter::UpdateColdPercentage, 1.0f, true);

	// Start a timer to update the character energy percentage
	GetWorldTimerManager().SetTimer(EnergyTimer, this, &AFPSCharacter::UpdateEnergyPercentage, 1.0f, true);

	// Trace collision parameters
	TraceParams = FCollisionQueryParams(FName(TEXT("LoS Temp Trace")), false, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActor(this);
	if(PlayerPhone != nullptr)
		TraceParams.AddIgnoredActor(PlayerPhone);
}

// Called every tick
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPlayerController->ShouldPrintInfo)
	{
		if (!ShouldBeIgnored)
		{
			PrintText("Ignore Player: false", 21, FColor::White);
		}
		else
		{
			PrintText("Ignore Player: true", 21, FColor::White);
		}

		if (!IsDead)
		{
			PrintText("Player Dead: false", 22, FColor::White);
		}
		else
		{
			PrintText("Player Dead: true", 22, FColor::White);
		}

		if (!InsideHidingSpot)
		{
			PrintText("Player Hiding: false", 23, FColor::Cyan);
		}
		else
		{
			PrintText("Player Hiding: true", 23, FColor::Cyan);
		}

		//if (CurrentHidingSpot != nullptr)
		//	PrintText("Not Nullptr", -1, FColor::Cyan, 0.0f);
	}

	// Handle the cooldowns of the blood corruption abilities.
	UpdateAbilityCooldowns(DeltaTime);

	//switch (PlayerMovementState)
	//{
	//	case EPlayerMovementState::EStandingStill:
	//		PrintText("EStandingStill", -1, FColor::Cyan, 0.0f);
	//		break;

	//	case EPlayerMovementState::EStandingStillCrouched:
	//		PrintText("EStandingStillCrouched", -1, FColor::Cyan, 0.0f);
	//		break;

	//	case EPlayerMovementState::EWalking:
	//		PrintText("EWalking", -1, FColor::Cyan, 0.0f);
	//		break;

	//	case EPlayerMovementState::EWalkingSlow:
	//		PrintText("EWalkingSlow", -1, FColor::Cyan, 0.0f);
	//		break;

	//	case EPlayerMovementState::EWalkingCrouched:
	//		PrintText("EWalkingCrouched", -1, FColor::Cyan, 0.0f);
	//		break;

	//	case EPlayerMovementState::ESprinting:
	//		PrintText("ESprinting", -1, FColor::Cyan, 0.0f);
	//		break;

	//	case EPlayerMovementState::EFalling:
	//		PrintText("EFalling", -1, FColor::Cyan, 0.0f);
	//		break;
	//};
	

	// Keep track of how long it has been since last stress increase
	if(TimeSinceStressChange < 3.0f)
	{
		// Start counting up
		TimeSinceStressChange += DeltaTime;
	}
	
	// Manage the breathing
	UpdateBreathing(DeltaTime);

	// Camera and visibility points while crouching moving them to the right locations
	if (GetCharacterMovement()->IsCrouching())
	{
		GetMesh()->SetVisibility(false);
		ShadowMesh->SetCastShadow(false);

		//FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
		CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));

		// Needs to be removed when there is a crouching animation
		//VisibilityPoints[0]->AttachRootComponentToActor(this);
		VisibilityPoints[0]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		VisibilityPoints[0]->SetActorRelativeLocation(FVector(0.0f, 0.0f, 48.0f));
		
		//VisibilityPoints[1]->AttachRootComponentToActor(this);
		VisibilityPoints[1]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		VisibilityPoints[1]->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		
		//VisibilityPoints[2]->AttachRootComponentToActor(this);
		VisibilityPoints[2]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		VisibilityPoints[2]->SetActorRelativeLocation(FVector(0.0f, 0.0f, -48.0f));

		for (int i = 3; i < 9; i++)
		{
			VisibilityPoints[i]->Active = false;
		}

	}
	// If the player is not crouching, set the camera to the eyeSocket location and make sure that the visibility points are in the correct loc
	else if (!GetCharacterMovement()->IsCrouching())
	{
		GetMesh()->SetVisibility(true);
		ShadowMesh->SetCastShadow(true);

		//FirstPersonCameraComponent->SetWorldLocation(ShadowMesh->GetSocketLocation("eyeSocket"));
		CameraBoom->SetWorldLocation(ShadowMesh->GetSocketLocation("eyeSocket"));

		// Remove when there is a chrouching animation
		//VisibilityPoints[0]->AttachRootComponentTo(ShadowMesh, VisibilityPoints[0]->SocketName, EAttachLocation::SnapToTarget);
		VisibilityPoints[0]->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, VisibilityPoints[0]->SocketName);
		//VisibilityPoints[1]->AttachRootComponentTo(ShadowMesh, VisibilityPoints[1]->SocketName, EAttachLocation::SnapToTarget);
		VisibilityPoints[1]->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, VisibilityPoints[1]->SocketName);
		//VisibilityPoints[2]->AttachRootComponentTo(ShadowMesh, VisibilityPoints[2]->SocketName, EAttachLocation::SnapToTarget);
		VisibilityPoints[2]->AttachToComponent(ShadowMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, VisibilityPoints[2]->SocketName);

		for (int i = 3; i < 9; i++)
		{
			VisibilityPoints[i]->Active = true;
		}
	}
	
	// If the player is holding an object, move the handle infront of the player
	if (HoldingObject)
	{
		FVector CamerLoc = FVector(FirstPersonCameraComponent->GetComponentLocation().X, FirstPersonCameraComponent->GetComponentLocation().Y, FirstPersonCameraComponent->GetComponentLocation().Z - CameraDownZ);
		FVector HandleLocation = CamerLoc + (FirstPersonCameraComponent->GetForwardVector() * DistanceFromPlayer);
		PickupPhysicsHandle->SetTargetLocation(HandleLocation);
		PickupPhysicsHandle->SetTargetRotation(GetActorRotation());

		if (HeldObject != nullptr)
		{
			// Check the distance
			if (FVector::Dist(FirstPersonCameraComponent->GetComponentLocation(), HeldObject->Mesh->GetComponentLocation()) > MaxDistanceFromPlayer)
			{
				LetGoObject(false);
			}
		}
	}


	// Count how long the player has been falling for
	if (GetCharacterMovement()->IsFalling())
	{
		FallingDuration += DeltaTime;
	}
	else
	{
		FallingDuration = 0.0f;
	}

	// Check which murderers are in view of the player.
	for (int i = 0; i < CurrentGameMode->Murderers.Num(); i++)
	{
		if (CurrentGameMode->Murderers[i] != nullptr)
		{
			if (LocationInFront(CurrentGameMode->Murderers[i]->GetActorLocation()))
			{
				// Line trace to see if here is something blocking the view
				if (GetController()->LineOfSightTo(CurrentGameMode->Murderers[i]))
				{
					if (!VisibleMurderers.Contains(CurrentGameMode->Murderers[i]))
					{
						VisibleMurderers.Add(CurrentGameMode->Murderers[i]);
					}
				}
				else
				{
					// Remove murderer as it is not in LoS
					if (VisibleMurderers.Contains(CurrentGameMode->Murderers[i]))
					{
						VisibleMurderers.RemoveAt(VisibleMurderers.Find(CurrentGameMode->Murderers[i]));
					}
				}
			}
			else
			{
				// Remove murderer as it is not infront of the player.
				if (VisibleMurderers.Contains(CurrentGameMode->Murderers[i]))
				{
					VisibleMurderers.RemoveAt(VisibleMurderers.Find(CurrentGameMode->Murderers[i]));
				}
			}
		}
	}

	// Only run if there is a controller
	if (CurrentPlayerController != nullptr)
	{
		// Rotate the player to certain location using lerp
		if (ShouldTurn)
		{
			FVector PlayerLocation = FirstPersonCameraComponent->GetComponentLocation();
			FVector TargetLocation = LookAtLocation;
			FVector Direction = (TargetLocation - PlayerLocation);
			Direction.Normalize();

			FRotator NewRotation = FMath::Lerp(FRotator(FirstPersonCameraComponent->GetComponentRotation().Pitch, GetActorRotation().Yaw, 0.0f), Direction.Rotation(), TurnSpeed);
			GetController()->SetControlRotation(NewRotation);

			if (FRotator(FirstPersonCameraComponent->GetComponentRotation().Pitch, GetActorRotation().Yaw, 0.0f).Equals(Direction.Rotation(), 5.0f))
			{
				ShouldTurn = false;

				if (ShouldUnlock)
					CurrentPlayerController->IgnoreMouseLookInput = false;

				LookAtLocation = FVector(0.0f, 0.0f, 0.0f);
				TurnSpeed = 0.0f;
			}
		}
	}
}

bool AFPSCharacter::IsMurdererVisible(AMurdererCharacter * Murderer)
{
	if (Murderer != nullptr)
	{
		if (VisibleMurderers.Contains(Murderer))
		{
			return true;
		}
	}
	return false;
}

// Rotates the player to look at a location. Speed = 0.1 -> 1.0
void AFPSCharacter::PlayerLookAt(FVector Location, float Speed, bool LockInput, bool UnlockWhenDone)
{
	LookAtLocation = Location;
	TurnSpeed = Speed;
	ShouldTurn = true;
	ShouldUnlock = UnlockWhenDone;

	if (LockInput && CurrentPlayerController != nullptr)
		CurrentPlayerController->IgnoreMouseLookInput = true;
}

void AFPSCharacter::KillPlayer(bool ShouldFallDown, float FallOverDelay)
{
	if (CurrentPlayerController == nullptr)
		return;

	IsDead = true;
	
	CurrentPlayerController->IgnoreMouseLookInput = true;
	CurrentPlayerController->IgnoreMovementInput = true;
	CurrentPlayerController->CharacterDead = true;

	// Put the phone back if the phone is out
	if (HasPhoneOut)
		SetPhoneActive(false, false);

	if (HoldingObject)
		LetGoObject(false);

	// Close the inventory menu
	if (CurrentPlayerController->PlayerHUDWidget != nullptr && CurrentPlayerController->PlayerHUDWidget->InventoryActive)
	{
		CurrentPlayerController->PlayerHUDWidget->ToggleInventory(false);
	}

	// Have the player fall over
	if (ShouldFallDown && FallOverDelay > 0.0f)
		GetWorldTimerManager().SetTimer(DelayedDyingTimer, this, &AFPSCharacter::FallOver, FallOverDelay, false);
	else if(ShouldFallDown)
		FallOver();
}

void AFPSCharacter::FallOver()
{
	// Start simulating physics on the meshes
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	ShadowMesh->SetAllBodiesSimulatePhysics(true);
	FallingDead = true;
}

void AFPSCharacter::AddToStressLevel(float InValue)
{
	if (InValue > 0.0f)
		TimeSinceStressChange = 0.0f;
	
	StressLevel += InValue;
	StressLevel = FMath::Clamp(StressLevel, 0.0f, 100.0f);
}

void AFPSCharacter::UpdateStressLevel()
{
	// Increases or decreases the players stress level depending on certain things such as if the player can see the murderer
	
	if (CurrentGameMode == nullptr || CurrentPlayerController == nullptr)
		return;

	// The value that will be added onto the stress level variable, can be positive or negative
	float TempValue = 0;

	// If a murderer can be seen
	for (int i = 0; i < CurrentGameMode->Murderers.Num(); i++)
	{
		if (LocationInFront(CurrentGameMode->Murderers[i]->GetActorLocation()))
		{
			// Line trace to see if here is something blocking the view
			if (GetController()->LineOfSightTo(CurrentGameMode->Murderers[i]))
			{
				TempValue += 15;
			}
		}
		
		// If the player is close to a murderer
		if (FVector::Dist(GetActorLocation(), CurrentGameMode->Murderers[i]->GetActorLocation()) < 500.0f)
		{
			TempValue += 5;
		}
	}
	
	// If the player is moving or not
	switch (CurrentMovementState())
	{
	case EPlayerMovementState::EWalking:
		TempValue += 5;
		break;

	case EPlayerMovementState::EWalkingSlow:
		TempValue += 2.5f;
		break;

	case EPlayerMovementState::ESprinting:
		TempValue += 10;
		break;

	case EPlayerMovementState::EWalkingCrouched:
		TempValue += 2.5f;
		break;

	case EPlayerMovementState::EFalling:
		TempValue += 10;
		break;
	};

	//// If the player is moving or not
	//if (PlayerSprinting())
	//	TempValue += 10;
	//else if (PlayerWalking())
	//	TempValue += 5;
	//else if (PlayerWalkingSlow() || PlayerMovingCrouched())
	//	TempValue += 2.5f;
	//else if (!PlayerMoving())
	//	TempValue -= 5;

	// If the player is not moving and is crouched
	if (!PlayerMoving() && PlayerCrouched())
	{
		TempValue -= 5;
	}

	// Here the TempValue is checked to see if the stress should be increased or decreased
	// If the stress level should be increased
	if (TempValue > 0.0f)
	{
		StressLevel += TempValue;
		TimeSinceStressChange = 0.0f;
	}
	// If the stress level should be decreased
	else if (TempValue < 0.0f)
	{
		// Only decrease after 3 seconds of no increase
		if (TimeSinceStressChange >= 3.0f)
		{
			StressLevel += TempValue;
		}
	}

	StressLevel = FMath::Clamp(StressLevel, 0.0f, 100.0f);

	if (CurrentPlayerController->ShouldPrintInfo)
	{
		PrintText("Stress Increase / Sec: " + FString::SanitizeFloat(TempValue), 25, FColor::Yellow, 1.0f);
		PrintText("Player Stress: " + FString::SanitizeFloat(StressLevel), 26, FColor::Yellow, 1.0f);
	}
}

void AFPSCharacter::AddToColdPercentage(float InValue)
{
	ColdPercentage += InValue;
	ColdPercentage = FMath::Clamp(ColdPercentage, 0.0f, 100.0f);
}

void AFPSCharacter::UpdateColdPercentage()
{
	if (CurrentGameMode == nullptr || CurrentPlayerController == nullptr || !ManageColdPercentage || IsDead)
		return;

	float ValueToAdd = -NormalDecrease;

	// Check if it is night currently.
	if (CurrentGameMode->GetCurrentWorldTime() >= 18.0f || CurrentGameMode->GetCurrentWorldTime() <= 6.0f)
	{
		ValueToAdd = -NightDecrease;
	}

	// Find all overlapping temp zones
	TemperatureZoneValue = 0;
	bool Inside = false;
	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, ATemperatureZone::StaticClass());

	for (auto Itr(OverlappingActors.CreateIterator()); Itr; Itr++)
	{
		if((*Itr) != nullptr)
		{
			ATemperatureZone* TempZone = Cast<ATemperatureZone>((*Itr));

			if (TempZone != nullptr && !TempZone->RequiresLineOfSight)
			{
				TemperatureZoneValue += TempZone->Temperature;	
			}
			else if(TempZone != nullptr)
			{
				// Trace to see if there was a blocking hit
				FHitResult HitResult(ForceInit);
				FVector StartFVector = TempZone->LineOfSightCheck->GetComponentLocation();
				FVector EndFVector = GetActorLocation();
				
				if (!GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
				{
					if (TempZone->ShouldDebug)
					{
						PrintText("Hit Nothing", -1, FColor::Red, 1.0f);
						DrawDebugLine(GetWorld(), StartFVector, EndFVector, FColor::Green, false, 1.0f, (uint8)'\000', 5.0f);
					}

					TemperatureZoneValue += TempZone->Temperature;
				}
				else
				{
					if (TempZone->ShouldDebug)
					{
						PrintText("Hit Something", -1, FColor::Red, 1.0f);
						DrawDebugLine(GetWorld(), StartFVector, EndFVector, FColor::Red, false, 1.0f, (uint8)'\000', 5.0f);
					}
				}

				
			}
			
			// Check if the player is inside
			if (Cast<ATemperatureZone>((*Itr))->Indoors)
			{
				Inside = true;
			}

			// Warm the clothes if they are wet and player inside a warm zone
			if (Cast<ATemperatureZone>((*Itr))->Temperature > 0.0f)
			{
				HasWetClothes = false;
			}
		}
	}

	// After adding all the temp zones together, add it to the value
	ValueToAdd += TemperatureZoneValue;

	// If the player is not inside, the worlds modifier should take effect, this is based on the current weather.
	if (!Inside)
	{
		ValueToAdd -= CurrentGameMode->EnvironmentManager->TemperatureModifier;
		//PrintText("Not Inside", -1, FColor::Red, 1.0f);
	}

	// Check for wet clothes
	if (HasWetClothes)
	{
		ValueToAdd += -WetClothesDecrease;
	}


	// When on low energy
	if (EnergyPercentage <= LowEnergy)
	{
		ValueToAdd += -LowEnergyDecrease;
	}

	ColdPercentage += ValueToAdd;
	ColdPercentage = FMath::Clamp(ColdPercentage, 0.0f, 100.0f);

	if (ColdPercentage <= 0.0f)
	{
		//Dead
		KillPlayer(true, 0.0f);
	}
}

void AFPSCharacter::AddToEnergyPercentage(float InValue)
{
	EnergyPercentage += InValue;
	EnergyPercentage = FMath::Clamp(EnergyPercentage, -10.0f, 100.0f);
}

void AFPSCharacter::UpdateEnergyPercentage()
{
	if (CurrentGameMode == nullptr || CurrentPlayerController == nullptr || !ManageEnergyPercentage || IsDead)
		return;

	float ValueToAdd = -IdleDecrease;

	switch (CurrentMovementState())
	{
		case EPlayerMovementState::EWalking:
			ValueToAdd = -WalkingDecrease;
			break;

		case EPlayerMovementState::EWalkingSlow:
			ValueToAdd = -WalkingSlowDecrease;
			break;

		case EPlayerMovementState::ESprinting:
			ValueToAdd = -RunningDecrease;
			break;

		case EPlayerMovementState::EWalkingCrouched:
			ValueToAdd = -WalkingSlowDecrease;
			break;

		case EPlayerMovementState::EFalling:
			ValueToAdd = -RunningDecrease;
			break;
	};

	EnergyPercentage += ValueToAdd;
	EnergyPercentage = FMath::Clamp(EnergyPercentage, -10.0f, 100.0f);

	if (EnergyPercentage <= -10.0f)
	{
		//Dead
		KillPlayer(true, 0.0f);
	}
}

void AFPSCharacter::UpdateBreathing(float DeltaTime)
{
	if (CurrentPlayerController == nullptr)
		return;

	// If the player can't breathe or is manually breathing
	if (IsDead || !ShouldBreathe || CurrentPlayerController->InhaleDuration > 0.0f || CurrentPlayerController->CoughDuration > 0.0f || CurrentPlayerController->ExhaleDuration > 0.0f)
		return;

	// Calculate the current breath delay.
	float SubtractValue = 2.0f;
	float TempStressValue = (StressLevel * 0.01f);
	CurMaxBreathDelay = MaxBreathDelay - (SubtractValue * TempStressValue);

	if (BreathDelay <= 0.0f)
	{
		if (CurrentPlayerController->ShouldPrintInfo)
		{
			//PrintText("Breath taken!", 30, FColor::White);
		}

		CreateNoise(FirstPersonCameraComponent->GetComponentLocation(), BreathNoiseStrength);
		BreathDelay = CurMaxBreathDelay;
	}
	else
	{
		BreathDelay -= DeltaTime;
	}

	/*print("BreathDelay: " + FString::SanitizeFloat(BreathDelay), 27, FColor::Green);*/
	/*print("Current Breath Delay: " + FString::SanitizeFloat(CurMaxBreathDelay), 28, FColor::Green);*/
}

void AFPSCharacter::Landed(const FHitResult& Hit)
{
	CreateNoise(GetActorLocation(), 75.0f);
	
	if (FallingDuration > FallingDeathLimit)
	{
		KillPlayer(true, 0.1f);
	}
}

void AFPSCharacter::PickUpPhone()
{
	if (PlayerPhone == nullptr && PhoneClassBP != nullptr)
	{
		PlayerPhone = SpawnBP<APlayerPhone>(GetWorld(), PhoneClassBP, FVector::ZeroVector, FRotator::ZeroRotator);
		
		if (PlayerPhone != nullptr)
		{
			HasPhone = true;
			HasPhoneOut = false;
			CurrentPlayerController->CharacterHasPhone = true;
		}
	}
}

//void AFPSCharacter::TakeOutPhone(bool ForceDown)
//{
//	if (PlayerPhone == nullptr || CurrentPlayerController == nullptr)
//		return;
//
//	// Should not be able to put away phone while currently speaking
//	if (CurrentPlayerController->PlayerHUDWidget->PhoneDialogueActive && HasPhoneOut && !ForceDown)
//	{
//		return;
//	}
//
//	// Set to show the phone
//	//PlayerPhone->IsActive = !HasPhoneOut;
//	PlayerPhone->TakeOutPhone(!HasPhoneOut);
//	HasPhoneOut = !HasPhoneOut;
//	CurrentPlayerController->CharacterHasPhoneOut = HasPhoneOut;
//}

void AFPSCharacter::SetPhoneActive(bool NewActive, bool ForceDown)
{
	if (PlayerPhone == nullptr || CurrentPlayerController == nullptr || NewActive == HasPhoneOut)
		return;

	// Should not be able to put away phone while currently speaking
	if (CurrentPlayerController->PlayerHUDWidget->PhoneDialogueActive && !NewActive && !ForceDown)
	{
		return;
	}

	PlayerPhone->TakeOutPhone(NewActive);
	HasPhoneOut = NewActive;
	CurrentPlayerController->CharacterHasPhoneOut = HasPhoneOut;
}

// Tell the handle to pick the object up.
void AFPSCharacter::PickUpObject(AActor* PickupObject)
{
	if (PickupObject == nullptr)
		return;

	//PrintText("Pickup!", -1, FColor::Green, 2.0f);

	HoldingObject = true;
	HeldObject = Cast<AIO_PickupObject>(PickupObject);
	HeldObject->PlayerHeld = true;
	HeldObject->Thrown = false;
	HeldObject->NoiseCDCountdown = 0.0f;
	HeldObject->Mesh->SetSimulatePhysics(true);
	CameraDownZ = HeldObject->PlayerCameraDownZ;

	//PickupPhysicsHandle->GrabComponent(HeldObject->Mesh, NAME_None, HeldObject->Mesh->GetComponentLocation(), true);
	PickupPhysicsHandle->GrabComponentAtLocationWithRotation(HeldObject->Mesh, NAME_None, HeldObject->Mesh->GetComponentLocation(), HeldObject->Mesh->GetComponentRotation());

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
}

// Drop the object or throw it away
void AFPSCharacter::LetGoObject(bool ShouldThrow)
{
	//PrintText("Let Go!", -1, FColor::Green, 2.0f);

	PickupPhysicsHandle->ReleaseComponent();
	HoldingObject = false;

	if (HeldObject != nullptr)
	{
		// If the player is just letting go of the left button
		if (!ShouldThrow)
		{
			HeldObject->Mesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
			HeldObject->Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		}
		// If the player threw the object
		else
		{
			FVector ThrowImpulse = FirstPersonCameraComponent->GetForwardVector() * ThrowForce;
			HeldObject->Mesh->AddImpulse(ThrowImpulse);
			HeldObject->Thrown = true;
		}

		HeldObject->PlayerHeld = false;
		HeldObject = nullptr;
	}

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

void AFPSCharacter::ClearInventory()
{
	Inventory.Init(nullptr, 20);
}

// Inventory
bool AFPSCharacter::AddInventoryItem(UClass* InventoryItemBP, bool SuppressText)
{
	// Spawn the inventory item containing the objects information
	AInventoryItem* SpawnedBP = nullptr;
	SpawnedBP = SpawnBP<AInventoryItem>(GetWorld(), InventoryItemBP, FVector::ZeroVector, FRotator::ZeroRotator);

	// Find the first empty location in the array
	bool FoundEmpty = false;
	for (int i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			Inventory[i] = SpawnedBP;
			SpawnedBP->Index = i;
			FoundEmpty = true;
			break;
		}
	}

	// If there is no empty slots, do not pick up the object.
	if (!FoundEmpty)
	{
		if(!SuppressText)
			CurrentPlayerController->PlayerHUDWidget->DisplayInteractionString("Inventory Full");

		SpawnedBP->Destroy();
		return false;
	}
	else
	{
		if(!SuppressText)
			CurrentPlayerController->PlayerHUDWidget->DisplayInteractionString(SpawnedBP->PickupPrefixText + " " + SpawnedBP->Name);

		return true;
	}

	return true;
}

bool AFPSCharacter::AddInventoryItemIndex(UClass* InventoryItemBP, int32 ArrayIndex, bool SuppressText)
{
	// Spawn the inventory item containing the objects information
	AInventoryItem* SpawnedBP = nullptr;
	SpawnedBP = SpawnBP<AInventoryItem>(GetWorld(), InventoryItemBP, FVector::ZeroVector, FRotator::ZeroRotator);

	bool FoundEmpty = false;

	// Specified index is empty, add the item to that slot
	if (Inventory[ArrayIndex] == nullptr)
	{
		Inventory[ArrayIndex] = SpawnedBP;
		SpawnedBP->Index = ArrayIndex;
		FoundEmpty = true;
	}
	else
	{
		// Find the first empty location in the array
		for (int i = 0; i < Inventory.Num(); i++)
		{
			if (Inventory[i] == nullptr)
			{
				Inventory[i] = SpawnedBP;
				SpawnedBP->Index = i;
				FoundEmpty = true;
				break;
			}
		}
	}

	// If there is no empty slots, do not pick up the object.
	if (!FoundEmpty)
	{
		if(!SuppressText)
			CurrentPlayerController->PlayerHUDWidget->DisplayInteractionString("Inventory Full");
		
		SpawnedBP->Destroy();
		return false;
	}
	else
	{
		if(!SuppressText)
			CurrentPlayerController->PlayerHUDWidget->DisplayInteractionString(SpawnedBP->PickupPrefixText + " " + SpawnedBP->Name);

		return true;
	}

	return true;
}

void AFPSCharacter::RemoveInventoryItem(AInventoryItem* Item)
{
	// Set the index in the inventory array to null and then destroy the actor
	Inventory[Item->Index] = nullptr;
	Item->Destroy();
}

void AFPSCharacter::RemoveInventoryItemByName(FString Name)
{
	// Look through the inventory to see if the names match.
	for (auto Itr(Inventory.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			if ((*Itr)->Name == Name)
			{
				// If the names match, remove the item.
				RemoveInventoryItem((*Itr));
				return;
			}
		}
	}
}

bool AFPSCharacter::HasItem(FString ItemName)
{
	// Look through the inventory to see if the names match.
	for (auto Itr(Inventory.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			if ((*Itr)->Name == ItemName)
			{
				return true;
			}
		}
	}

	return false;
}

void AFPSCharacter::AddObjective(UObjectiveData * Objective, bool SuppressText)
{
	if (Objective == nullptr)
		return;

	// Check so there is no duplicates
	if (!CurrentObjectives.Contains(Objective))
	{
		int Index = CurrentObjectives.Add(Objective);

		// Set the starting segment in the objective
		if(CurrentObjectives[Index]->ObjectiveSegments.Num() > 0)
			CurrentObjectives[Index]->CurrentSegment = CurrentObjectives[Index]->ObjectiveSegments[0];

		CurrentObjectives.Sort(AFPSCharacter::ConstPredicate);
		ReCheckHighlightedObjective();

		if(!SuppressText)
			CurrentPlayerController->PlayerHUDWidget->AddedObjective();
	}
}

void AFPSCharacter::AddCompletedObjective(FName ObjectiveName)
{
	if (!CompletedObjectives.Contains(ObjectiveName))
	{
		CompletedObjectives.Add(ObjectiveName);
	}
}

void AFPSCharacter::ReCheckHighlightedObjective()
{
	// If there is no highlighted element, pick the first element in the array if there is one.
	if (HighlightedObjective == nullptr)
	{
		if (CurrentObjectives.Num() > 0)
			HighlightedObjective = CurrentObjectives[0];

		return;
	}

	// If the highlighted objective's importance is lower than the first element in the array. Change highlighted objective.
	if (CurrentObjectives.Num() > 0 && (HighlightedObjective->ObjectiveImportance < CurrentObjectives[0]->ObjectiveImportance))
	{
		HighlightedObjective = CurrentObjectives[0];
	}
}

void AFPSCharacter::SetObjectiveSegment(FString ObjectiveName, FString SegmentName, bool SuppressText)
{
	// Look through the current objectives to see if there is a match
	for (int i = 0; i < CurrentObjectives.Num(); i++)
	{
		if (CurrentObjectives[i]->ObjectiveName == ObjectiveName)
		{
			// Look through the objectives segments to see if there is a match
			for (int j = 0; j < CurrentObjectives[i]->ObjectiveSegments.Num(); j++)
			{
				if (CurrentObjectives[i]->ObjectiveSegments[j].SegmentName == SegmentName)
				{
					// Set the objectives current segment to the new segment.
					CurrentObjectives[i]->CurrentSegment = CurrentObjectives[i]->ObjectiveSegments[j];

					if(!SuppressText)
						CurrentPlayerController->PlayerHUDWidget->UpdatedObjective();
				}
			}
		}
	}
}

bool AFPSCharacter::HasObjective(FString ObjectiveName)
{
	for (auto Itr(CurrentObjectives.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			if ((*Itr)->ObjectiveName == ObjectiveName)
			{
				return true;
			}
		}
	}

	return false;
}

bool AFPSCharacter::IsOnSegment(FString ObjectiveName, FString SegmentName)
{
	for (auto Itr(CurrentObjectives.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr) != nullptr)
		{
			if ((*Itr)->ObjectiveName == ObjectiveName && (*Itr)->CurrentSegment.SegmentName == SegmentName)
			{
				return true;
			}
		}
	}
	return false;
}

void AFPSCharacter::RemoveObjectiveRef(UObjectiveData * Objective)
{
	if (Objective == nullptr)
		return;

	if (CurrentObjectives.Contains(Objective))
	{
		if (HighlightedObjective == Objective)
			HighlightedObjective = nullptr;

		CurrentObjectives.RemoveAt(CurrentObjectives.Find(Objective));
		CurrentObjectives.Sort(AFPSCharacter::ConstPredicate);
		ReCheckHighlightedObjective();
	}
}

void AFPSCharacter::RemoveObjectiveName(FString ObjectiveName)
{
	for (int i = 0; i < CurrentObjectives.Num(); i++)
	{
		if (CurrentObjectives[i]->ObjectiveName == ObjectiveName)
		{
			RemoveObjectiveRef(CurrentObjectives[i]);
		}
	}
}

void AFPSCharacter::SelectBCAbility(EBloodCorruptionAbilities Ability)
{
	switch (Ability)
	{
		case EBloodCorruptionAbilities::EBloodRush:
			BCBloodRush();
			break;

		case EBloodCorruptionAbilities::EBloodDash:
			BCBloodDash();
			break;

		case EBloodCorruptionAbilities::EBeastCalling:
			BCBeastCalling();
			break;

		case EBloodCorruptionAbilities::EHunterVision:
			BCHunterVision();
			break;

		case EBloodCorruptionAbilities::EVampiricHunger:
			BCVampiricHunger();
			break;
	};
}

FBloodCorruptionAbilityInfo AFPSCharacter::GetAbilityInfo(EBloodCorruptionAbilities Ability)
{
	switch (Ability)
	{
		case EBloodCorruptionAbilities::EBloodRush:
			return BloodRushInfo;
			break;

		case EBloodCorruptionAbilities::EBloodDash:
			return BloodDashInfo;
			break;

		case EBloodCorruptionAbilities::EBeastCalling:
			return BeastCallingInfo;
			break;

		case EBloodCorruptionAbilities::EHunterVision:
			return HunterVisionInfo;
			break;

		case EBloodCorruptionAbilities::EVampiricHunger:
			return VampiricHungerInfo;
			break;
	};

	return FBloodCorruptionAbilityInfo();
}

void AFPSCharacter::UpgradeAbility(EBloodCorruptionAbilities Ability, int32 Level, bool ShouldUnlockAbility)
{
	switch (Ability)
	{
		case EBloodCorruptionAbilities::EBloodRush:
			BloodRushInfo.AbilityLevel = Level;
			if (ShouldUnlockAbility)
				BloodRushInfo.Unlocked = true;
			break;

		case EBloodCorruptionAbilities::EBloodDash:
			BloodDashInfo.AbilityLevel = Level;
			if (ShouldUnlockAbility)
				BloodDashInfo.Unlocked = true;
			break;

		case EBloodCorruptionAbilities::EBeastCalling:
			BeastCallingInfo.AbilityLevel = Level;
			if (ShouldUnlockAbility)
				BeastCallingInfo.Unlocked = true;
			break;

		case EBloodCorruptionAbilities::EHunterVision:
			HunterVisionInfo.AbilityLevel = Level;
			if (ShouldUnlockAbility)
				HunterVisionInfo.Unlocked = true;
			break;

		case EBloodCorruptionAbilities::EVampiricHunger:
			VampiricHungerInfo.AbilityLevel = Level;
			if (ShouldUnlockAbility)
				VampiricHungerInfo.Unlocked = true;
			break;
	};
}

void AFPSCharacter::BCBloodRush()
{
	if (!BloodRushInfo.Unlocked)
	{
		//PrintText("Not Unlocked", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayLockedAlert();
		return;
	}

	if (Blood < (BloodRushInfo.Cost * BloodRushInfo.CostMultiplier))
	{
		//PrintText("Not enough blood", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCostAlert();
		return;
	}

	if (BloodRushInfo.CurrentAbilityCooldown > 0.0f)
	{
		//PrintText("Ability on cooldown", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCooldownAlert();
		return;
	}

	// Remove the cost from blood, put ability on cooldown and add corruption to the player
	Blood -= BloodRushInfo.Cost * BloodRushInfo.CostMultiplier;
	BloodRushInfo.CurrentAbilityCooldown = BloodRushInfo.AbilityCooldown;
	CorruptionLevel += BloodRushInfo.CorruptionIncrease;

	BloodRushActive = true;
	BPBloodRushActivate();

	//PrintText("Blood Rush", -1, FColor::Red, 2.0f);
}

void AFPSCharacter::BCBloodDash()
{
	if (!BloodDashInfo.Unlocked)
	{
		//PrintText("Not Unlocked", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayLockedAlert();
		return;
	}

	if (Blood < (BloodDashInfo.Cost * BloodDashInfo.CostMultiplier))
	{
		//PrintText("Not enough blood", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCostAlert();
		return;
	}

	if (BloodDashInfo.CurrentAbilityCooldown > 0.0f)
	{
		//PrintText("Ability on cooldown", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCooldownAlert();
		return;
	}

	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Walking)
	{
		return;
	}

	// Remove the cost from blood, put ability on cooldown and add corruption to the player
	Blood -= BloodDashInfo.Cost * BloodDashInfo.CostMultiplier;
	BloodDashInfo.CurrentAbilityCooldown = BloodDashInfo.AbilityCooldown;
	CorruptionLevel += BloodDashInfo.CorruptionIncrease;

	BloodDashing = true;
	BPBloodDashActivate();

	//PrintText("Blood Dash", -1, FColor::Red, 2.0f);
}

void AFPSCharacter::BCBeastCalling()
{
	if (!BeastCallingInfo.Unlocked)
	{
		//PrintText("Not Unlocked", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayLockedAlert();
		return;
	}

	if (Blood < (BeastCallingInfo.Cost * BeastCallingInfo.CostMultiplier))
	{
		//PrintText("Not enough blood", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCostAlert();
		return;
	}

	if (BeastCallingInfo.CurrentAbilityCooldown > 0.0f)
	{
		//PrintText("Ability on cooldown", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCooldownAlert();
		return;
	}

	// Remove the cost from blood, put ability on cooldown and add corruption to the player
	Blood -= BeastCallingInfo.Cost * BeastCallingInfo.CostMultiplier;
	BeastCallingInfo.CurrentAbilityCooldown = BeastCallingInfo.AbilityCooldown;
	CorruptionLevel += BeastCallingInfo.CorruptionIncrease;

	BPBeastCallingActivate();

	//PrintText("Beast Calling", -1, FColor::Red, 2.0f);
}

void AFPSCharacter::BCHunterVision()
{
	if (!HunterVisionInfo.Unlocked)
	{
		//PrintText("Not Unlocked", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayLockedAlert();
		return;
	}

	if (Blood < (HunterVisionInfo.Cost * HunterVisionInfo.CostMultiplier))
	{
		//PrintText("Not enough blood", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCostAlert();
		return;
	}

	if (HunterVisionInfo.CurrentAbilityCooldown > 0.0f)
	{
		//PrintText("Ability on cooldown", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCooldownAlert();
		return;
	}

	// Remove the cost from blood, put ability on cooldown and add corruption to the player
	Blood -= HunterVisionInfo.Cost * HunterVisionInfo.CostMultiplier;
	HunterVisionInfo.CurrentAbilityCooldown = HunterVisionInfo.AbilityCooldown;
	CorruptionLevel += HunterVisionInfo.CorruptionIncrease;

	UsingHunterVision = true;
	BPHunterVisionActivate();

	//PrintText("Hunter Vision", -1, FColor::Red, 2.0f);
}

void AFPSCharacter::BCVampiricHunger()
{
	if (!VampiricHungerInfo.Unlocked)
	{
		//PrintText("Not Unlocked", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayLockedAlert();
		return;
	}

	if (Blood < (VampiricHungerInfo.Cost * VampiricHungerInfo.CostMultiplier))
	{
		//PrintText("Not enough blood", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCostAlert();
		return;
	}

	if (VampiricHungerInfo.CurrentAbilityCooldown > 0.0f)
	{
		//PrintText("Ability on cooldown", -1, FColor::Red, 2.0f);
		CurrentPlayerController->PlayerHUDWidget->DisplayCooldownAlert();
		return;
	}

	// Remove the cost from blood, put ability on cooldown and add corruption to the player
	Blood -= VampiricHungerInfo.Cost * VampiricHungerInfo.CostMultiplier;
	VampiricHungerInfo.CurrentAbilityCooldown = VampiricHungerInfo.AbilityCooldown;
	CorruptionLevel += VampiricHungerInfo.CorruptionIncrease;

	// Gain energy
	EnergyPercentage += VampiricHungerEnergyGain;
	EnergyPercentage = FMath::Clamp(EnergyPercentage, 0.0f, 100.0f);

	BPVampHungerActivate();

	//PrintText("Vampiric Hunger", -1, FColor::Red, 2.0f);
}

void AFPSCharacter::UpdateAbilityCooldowns(float Delta)
{
	if (BloodRushInfo.CurrentAbilityCooldown > 0.0f)
	{
		BloodRushInfo.CurrentAbilityCooldown -= Delta;

		if (BloodRushInfo.CurrentAbilityCooldown < 0.0f)
			BloodRushInfo.CurrentAbilityCooldown = 0.0f;
	}

	if (BloodDashInfo.CurrentAbilityCooldown > 0.0f)
	{
		BloodDashInfo.CurrentAbilityCooldown -= Delta;

		if (BloodDashInfo.CurrentAbilityCooldown < 0.0f)
			BloodDashInfo.CurrentAbilityCooldown = 0.0f;
	}

	if (BeastCallingInfo.CurrentAbilityCooldown > 0.0f)
	{
		BeastCallingInfo.CurrentAbilityCooldown -= Delta;

		if (BeastCallingInfo.CurrentAbilityCooldown < 0.0f)
			BeastCallingInfo.CurrentAbilityCooldown = 0.0f;
	}

	if (HunterVisionInfo.CurrentAbilityCooldown > 0.0f)
	{
		HunterVisionInfo.CurrentAbilityCooldown -= Delta;

		if (HunterVisionInfo.CurrentAbilityCooldown < 0.0f)
			HunterVisionInfo.CurrentAbilityCooldown = 0.0f;
	}

	if (VampiricHungerInfo.CurrentAbilityCooldown > 0.0f)
	{
		VampiricHungerInfo.CurrentAbilityCooldown -= Delta;

		if (VampiricHungerInfo.CurrentAbilityCooldown < 0.0f)
			VampiricHungerInfo.CurrentAbilityCooldown = 0.0f;
	}
}

/**--------- Utility ------------ */
#pragma region Utility
EPlayerMovementState AFPSCharacter::CurrentMovementState()
{
	return PlayerMovementState;
}

bool AFPSCharacter::PlayerMoving()
{
	if (CurrentPlayerController == nullptr)
		return false;
	
	if (CurrentPlayerController->CurrentSpeed > 0.0f)
		return true;

	return false;
}

bool AFPSCharacter::PlayerCrouched()
{
	return GetCharacterMovement()->IsCrouching();
}

bool AFPSCharacter::PlayerMovingCrouched()
{
	if (PlayerMovementState == EPlayerMovementState::EWalkingCrouched)
	{
		return true;
	}

	return false;
}

bool AFPSCharacter::PlayerSprinting()
{
	if (PlayerMovementState == EPlayerMovementState::ESprinting)
	{
		return true;
	}

	return false;
}

bool AFPSCharacter::PlayerWalking()
{
	if (PlayerMovementState == EPlayerMovementState::EWalking)
	{
		return true;
	}

	return false;
}

bool AFPSCharacter::PlayerWalkingSlow()
{
	if (PlayerMovementState == EPlayerMovementState::EWalkingSlow)
	{
		return true;
	}
	
	return false;
}

void AFPSCharacter::CreateNoise(FVector Location, float Loudness/*, bool InfiniteRange*/)
{
	FName StimStrength = FName(*FString::SanitizeFloat(Loudness));
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0, this, 0.0f, StimStrength);

	//if (!InfiniteRange)
	//{
	//	//PrintText("MADE NOISE: " + FString::SanitizeFloat(Loudness), -1, FColor::Cyan, 2.0f);
	//	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0f, this, 20000.0f, StimStrength);
	//}
	//else
	//{
		//PrintText("MADE NOISE, infi range", -1, FColor::Cyan, 2.0f);

	//}
}

void AFPSCharacter::ShowDisplayInteractionString(FString Text)
{
	if (CurrentPlayerController != nullptr)
	{
		CurrentPlayerController->PlayerHUDWidget->DisplayInteractionString(Text);
	}
}

bool AFPSCharacter::LocationInFront(FVector inLocation)
{
	// Check if the location is in front of the player
	
	FVector Dir = GetActorForwardVector();
	FVector Offset = inLocation - GetActorLocation();
	Offset = Offset.GetSafeNormal();
	float FrontBack = FVector::DotProduct(Dir, Offset);

	return (FrontBack > 0.7f/*FMath::Cos(90.0f) / 2.0f*/);
}

#pragma endregion
/**--------- Utility ------------ */

void AFPSCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("SlowZone"))
	{
		// Set the current slow zone
		ASlowZone* SlowZone = Cast<ASlowZone>(OtherActor);
		CurrentPlayerController->CurrentSlowZone = SlowZone;
	}
}

void AFPSCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag("SlowZone"))
	{
		// Check if the current slow zone is the one that the player is leaving before removing the current slow zone.
		ASlowZone* SlowZone = Cast<ASlowZone>(OtherActor);
		if (CurrentPlayerController->CurrentSlowZone == SlowZone)
		{
			CurrentPlayerController->CurrentSlowZone = nullptr;
		}
	}
}