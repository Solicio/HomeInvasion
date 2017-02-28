// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "FPSPlayerController.h"

#include "FPSCharacter.h"

AFPSPlayerController::AFPSPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
	// Movement speed
	SprintMovementMultiplier = 1.5f;
	WalkingMovementMultiplier = 0.5;

	// Trace length
	RayLength = 200.0f;

	// Doors and interacting
	CanInteract = true;
	HoldingDoor = false;
	MaxTurn = 20.0f;
	MaxDistanceFromDoor = 200.0f;
	HandleName = "";

	// Breathing
	CanManuallyBreathe = true;
	MaxInhaleDuration = 4.0f;
	MaxCoughDuration = 3.0f;
	MinInhaleDuration = 2.0f;

	LeftMouseDownDuration = 0.0f;
	RightMouseDownDuration = 0.0f;

	Key1Binding = EBloodCorruptionAbilities::EBloodRush;
	Key2Binding = EBloodCorruptionAbilities::EBloodDash;
	Key3Binding = EBloodCorruptionAbilities::EBeastCalling;
	Key4Binding = EBloodCorruptionAbilities::EHunterVision;
	Key5Binding = EBloodCorruptionAbilities::EVampiricHunger;
	SelectedBCBinding = EBloodCorruptionAbilities::EBloodRush;
}

// Called when the game starts or when spawned
void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacter() != nullptr)
		PlayerCharacter = Cast<AFPSCharacter>(GetCharacter());

	if (PlayerCharacter != nullptr)
	{
		// Store the original movement speed
		NormalMovementSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;

		// Trace collision parameters
		TraceParams = FCollisionQueryParams(FName(TEXT("IO Trace")), false, nullptr);
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = false;
		TraceParams.AddIgnoredActor(PlayerCharacter);

		if (PlayerHUDWidgetClass != nullptr && PlayerHUDWidget == nullptr)
		{
			// Create Widget
			PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);

			if (!PlayerHUDWidget)
				return;
			
			// Add it to the viewport so the Construct() method in the UUserWidget:: is run.
			PlayerHUDWidget->AddToViewport();
			PlayerHUDWidget->PlayerController = this;

			// Lock input
			//IgnoreMouseLookInput = true;
			//IgnoreMovementInput = true;
			DisableInput(this);

			// Fade in the game after load
			//PlayerHUDWidget->FadeIn(FColor::Black, 1.0f);
		}
	}
}

// Called to bind functionality to input
void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (InputComponent == nullptr)
	{
		return;
	}

	// Inhale
	InputComponent->BindAxis("Inhale", this, &AFPSPlayerController::Inhale);

	//Crouch
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSPlayerController::PlayerCrouch);

	// Jumping
	InputComponent->BindAction("Jump", IE_Pressed, this, &AFPSPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AFPSPlayerController::StopJumping);

	// Mouse click
	InputComponent->BindAction("LeftButton", IE_Pressed, this, &AFPSPlayerController::LeftClick);
	InputComponent->BindAction("LeftButton", IE_Released, this, &AFPSPlayerController::LeftClickRelease);
	InputComponent->BindAction("RightButton", IE_Pressed, this, &AFPSPlayerController::RightClick);
	InputComponent->BindAction("RightButton", IE_Released, this, &AFPSPlayerController::RightClickRelease);

	// Mouse look
	InputComponent->BindAxis("Turn", this, &AFPSPlayerController::MouseHorizontal);
	InputComponent->BindAxis("LookUp", this, &AFPSPlayerController::MouseVertical);

	// Movement
	InputComponent->BindAxis("MoveForward", this, &AFPSPlayerController::MoveFwd);
	InputComponent->BindAxis("MoveBackward", this, &AFPSPlayerController::MoveBwd);

	InputComponent->BindAxis("StrafeLeft", this, &AFPSPlayerController::StrafeL);
	InputComponent->BindAxis("StrafeRight", this, &AFPSPlayerController::StrafeR);

	InputComponent->BindAxis("Move", this, &AFPSPlayerController::Move);
	InputComponent->BindAxis("Strafe", this, &AFPSPlayerController::Strafe);
	InputComponent->BindAxis("Sprint", this, &AFPSPlayerController::Sprint);
	InputComponent->BindAxis("Walk Slow", this, &AFPSPlayerController::WalkSlow);

	// Phone
	InputComponent->BindAction("ScrollUp", IE_Pressed, this, &AFPSPlayerController::ScrollUp);
	InputComponent->BindAction("ScrollDown", IE_Pressed, this, &AFPSPlayerController::ScrollDown);

	InputComponent->BindAction("TakeOutPhone", IE_Pressed, this, &AFPSPlayerController::TakeOutPhone);

	InputComponent->BindAction("UsePhone", IE_Pressed, this, &AFPSPlayerController::UsePhone);

	// Blood Corruption
	InputComponent->BindAxis("AbilityWheel", this, &AFPSPlayerController::AbilityWheel);
	InputComponent->BindAction("SelectedBCAbility", IE_Pressed, this, &AFPSPlayerController::SelectedBCAbility);
	InputComponent->BindAction("BCKey1", IE_Pressed, this, &AFPSPlayerController::BCKey1);
	InputComponent->BindAction("BCKey2", IE_Pressed, this, &AFPSPlayerController::BCKey2);
	InputComponent->BindAction("BCKey3", IE_Pressed, this, &AFPSPlayerController::BCKey3);
	InputComponent->BindAction("BCKey4", IE_Pressed, this, &AFPSPlayerController::BCKey4);
	InputComponent->BindAction("BCKey5", IE_Pressed, this, &AFPSPlayerController::BCKey5);
}

// Called every tick
void AFPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter == nullptr)
	{
		return;
	}		
		
	// Trace the way the player is looking for interactable objects
	// This part will be used for GUI stuff mostly
	FHitResult HitResult(ForceInit);
	FVector StartFVector = PlayerCharacter->FirstPersonCameraComponent->GetComponentLocation();
	FVector EndFVector = StartFVector + (PlayerCharacter->FirstPersonCameraComponent->GetForwardVector() * RayLength);

	// If the trace hits a object
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
	{
		if (HitResult.Component->ComponentHasTag("InteractableObject"))
		{
			AInteractableObject* InterObject = Cast<AInteractableObject>(HitResult.GetActor());

			if (InterObject->Active && CanInteract && !PlayerCharacter->HoldingObject)
			{
				PlayerHUDWidget->InteractionName = InterObject->Name;
				PlayerHUDWidget->HideInteractImage = false;
			}
			else
			{
				PlayerHUDWidget->InteractionName = "";
				PlayerHUDWidget->HideInteractImage = true;
			}
		}
		else
		{
			PlayerHUDWidget->InteractionName = "";
			PlayerHUDWidget->HideInteractImage = true;
		}
	}
	else
	{
		PlayerHUDWidget->InteractionName = "";
		PlayerHUDWidget->HideInteractImage = true;
	}

	// Count the duration the mouse buttons are down.
	if (LeftMouseDown)
	{
		LeftMouseDownDuration += DeltaTime;
	}

	if (RightMouseDown)
	{
		RightMouseDownDuration += DeltaTime;
	}

	if (CurrentSlowZone != nullptr && CurrentSlowZone->Active)
	{
		FVector PlayerDirection;
		FVector WindDireciton;

		PlayerDirection = PlayerCharacter->GetCharacterMovement()->Velocity;
		WindDireciton = CurrentSlowZone->WindStartLocation->GetComponentLocation() - PlayerCharacter->GetActorLocation();
		
		PlayerDirection.Normalize();
		WindDireciton.Normalize();

		PlayerDirection.Z = 0.0f;
		WindDireciton.Z = 0.0f;

		//PlayerDirection.Y = 0.0f;
		//WindDireciton.Y = 0.0f;

		// If the zone is based of wind and the player is moving towards the source, slow down.
		if (CurrentSlowZone->BasedOfWind && PlayerDirection.Equals(WindDireciton, 0.7f))
		{
			//PrintText("Slow Down!", -1, FColor::Red, 0.0f);
			SlowZoneSpeedMulti = CurrentSlowZone->PlayerSpeedMulti;
		}
		else
		{
			// Based of wind, but should still have a base slow.
			if (CurrentSlowZone->BasedOfWind)
			{
				SlowZoneSpeedMulti = CurrentSlowZone->BaseSpeedMulti;
			}
			// Not based on wind, apply the slow all the time.
			else
			{
				SlowZoneSpeedMulti = CurrentSlowZone->PlayerSpeedMulti;
			}
		}

		
		//PrintText("Player Dir: " + PlayerDirection.ToString(), -1, FColor::Red, 0.0f);
		//PrintText("Wind Dir: " + WindDireciton.ToString(), -1, FColor::Red, 0.0f);
		//PrintText("Speed Multi: " + FString::SanitizeFloat(SlowZoneSpeedMulti), -1, FColor::Red, 0.0f);
	}
	else
	{
		//Reset the multiplier
		SlowZoneSpeedMulti = 1.0f;
	}
	
	// Reset movement speed when not doing anything special
	// Also reduce movement speed if strafing or going backwards
	if (!PlayerCharacter->PlayerSprinting() && !PlayerCharacter->PlayerWalkingSlow() && !HoldingDoor)
	{
		// Moving straight forward
		if (MovemementDirection > 0 && !MovingSideways)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (NormalMovementSpeed * SlowZoneSpeedMulti * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed * SlowZoneSpeedMulti;
			}
		}
		// Strafing
		else if (MovemementDirection >= 0 && MovingSideways)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (NormalMovementSpeed * StrafingMovementMultiplier * SlowZoneSpeedMulti * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (NormalMovementSpeed * SlowZoneSpeedMulti * StrafingMovementMultiplier);
			}
		}
		// Backing straight
		else if (MovemementDirection < 0)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (NormalMovementSpeed * BackingMovementMultiplier * SlowZoneSpeedMulti * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (NormalMovementSpeed * SlowZoneSpeedMulti * BackingMovementMultiplier);
			}
		}
	}

	// If below the threshold, reduce the movementspeed.
	if (PlayerCharacter->ColdPercentage <= 30.0f)
	{
		if (PlayerCharacter->BloodRushActive)
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed * PlayerCharacter->ColdMovementMulti * SlowZoneSpeedMulti * PlayerCharacter->BloodRushSpeedMulti);
		}
		else
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed * SlowZoneSpeedMulti * PlayerCharacter->ColdMovementMulti);
		}
	}
}

void AFPSPlayerController::LetGoOfDoor()
{
	if (PlayerCharacter == nullptr)
		return;

	HoldingDoor = false;
	HandleName = "";
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;

	if (HeldDoor != nullptr)
	{
		HeldDoor->PlayerHeld = false;
	}

	HeldDoor = nullptr;
}


//--- Input ---//

//--- Mouse
void AFPSPlayerController::LeftClick()
{
	LeftMouseDown = true;

	if (PlayerCharacter == nullptr)
		return;

	// Trace the way the player is looking for interactable objects
	FHitResult HitResult(ForceInit);
	FVector StartFVector = PlayerCharacter->FirstPersonCameraComponent->GetComponentLocation();
	FVector EndFVector = StartFVector + (PlayerCharacter->FirstPersonCameraComponent->GetForwardVector() * RayLength);

	// If the trace hits a object
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, ECC_WorldDynamic, TraceParams))
	{
		if (HitResult.Component->ComponentHasTag("InteractableObject"))
		{
			AInteractableObject* InterObject = Cast<AInteractableObject>(HitResult.GetActor());

			// If the object is a door
			if (InterObject != nullptr && InterObject->ActorHasTag("Door"))
			{
				// Grab the door
				if (InterObject->Active && CanInteract/* && !PlayerCharacter->PlayerCrouched()*/)
				{
					HeldDoor = Cast<AIO_Door>(HitResult.GetActor());

					if (!HoldingDoor && HeldDoor != nullptr)
					{
						// Check the distrance between the player and the two door handles
						float DistHandleIn = FVector::Dist(HeldDoor->Mesh->GetSocketLocation("Handle_In"), PlayerCharacter->GetActorLocation());
						float DistHandleOut = FVector::Dist(HeldDoor->Mesh->GetSocketLocation("Handle_Out"), PlayerCharacter->GetActorLocation());

						// Make sure the player is within range to grab a handle
						if ((DistHandleIn > MaxDistanceFromDoor) && (DistHandleOut > MaxDistanceFromDoor))
						{
							HeldDoor = nullptr;

							if (ShouldPrintInfo)
							{
								PrintText("Too far away from handle", -1, FColor::Red);
							}
						}
						else
						{
							//PlayerCharacter->PlayerLookAt(HeldDoor->LookAtSceneComp->GetComponentLocation(), 0.3, true, true);
							PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (NormalMovementSpeed * WalkingMovementMultiplier);
							HeldDoor->PlayerHeld = true;
							HoldingDoor = true;

							// Select the one that is closest to the player
							if (DistHandleIn < DistHandleOut)
							{
								HandleName = "Handle_In";
							}
							else
							{
								HandleName = "Handle_Out";
							}
						}

					}
				}
			}
			// If the object is a pickup object
			else if (InterObject != nullptr && InterObject->ActorHasTag("PickupObject"))
			{
				if (InterObject->Active && CanInteract)
				{
					// Tell the character to pick it up
					PlayerCharacter->PickUpObject(HitResult.GetActor());
				}
			}
			// else if it is just a normal interactable object
			else if (InterObject->Active && CanInteract)
			{
				InterObject->Interact(PlayerCharacter);
			}
		}
	}
}

void AFPSPlayerController::LeftClickRelease()
{
	LeftMouseDown = false;

	if (PlayerCharacter == nullptr)
		return;

	if (HoldingDoor)
	{
		// If the player clicks and releases quickly, auto open the door
		if (LeftMouseDownDuration <= 0.3f && HeldDoor != nullptr)
		{
			HeldDoor->AutoOpen(HandleName);
		}
		LetGoOfDoor();
	}

	if (PlayerCharacter->HoldingObject)
	{
		PlayerCharacter->LetGoObject(false);
	}

	LeftMouseDownDuration = 0.0f;
}

void AFPSPlayerController::RightClick()
{
	RightMouseDown = true;
}

void AFPSPlayerController::RightClickRelease()
{
	RightMouseDown = false;
	RightMouseDownDuration = 0.0f;

	if (PlayerCharacter->HoldingObject)
	{
		PlayerCharacter->LetGoObject(true);
	}
}

void AFPSPlayerController::MouseHorizontal(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (!IgnoreMouseLookInput)
	{
		float TempValue = Value;

		// Stop the player from rotating more than the MaxTurn value while holding the door
		// If the player has held down the mouse button for more then .3 seconds, allow the player to move the door
		if (HoldingDoor && LeftMouseDownDuration > 0.3f)
		{
			TempValue = Value;
			PlayerTurnValue += TempValue;

			// Check the absolute value of PlayerTurnValue against MaxTurn
			if (abs((long)PlayerTurnValue) < MaxTurn)
			{
				AddYawInput(TempValue);
			}
			else
			{
				// Clamp the value so that it wont keep adding up
				PlayerTurnValue = FMath::Clamp(PlayerTurnValue, -MaxTurn, MaxTurn);
			}
		}
		else
		{
			// If the player is not holding a door, work normally
			PlayerTurnValue = 0.0f;
			AddYawInput(TempValue);
		}
	}
}

void AFPSPlayerController::MouseVertical(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (!IgnoreMouseLookInput && !HoldingDoor)
	{
		AddPitchInput(Value);
	}
	else if (HoldingDoor && HeldDoor != nullptr)
	{
		// If the door handle is too far away, let go of the door
		float HandleDist = FVector::Dist(HeldDoor->Mesh->GetSocketLocation(HandleName), PlayerCharacter->GetActorLocation());
		if (HandleDist > MaxDistanceFromDoor)
		{
			if (ShouldPrintInfo)
			{
				PrintText("Let go of door.", -1, FColor::Red);
			}
				
			HoldingDoor = false;
			HandleName = "";
	
			if (HeldDoor != nullptr)
			{
				HeldDoor->PlayerHeld = false;
			}
	
			HeldDoor = nullptr;
	
			return;
		}
	
		// Move the door
		HeldDoor->MoveDoor(-Value, HandleName/*, MovemementDirection, CurrentSpeed*/);
	}
}

//---

//--- Movement
void AFPSPlayerController::Move(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	// Set movement direction variable
	if (Value > 0.0f)
	{
		MovemementDirection = 1;
	}
	else if (Value < 0.0f)
	{
		MovemementDirection = -1;
	}
	
	// Set the current speed
	CurrentSpeed = PlayerCharacter->GetVelocity().Size();
	
	if (Value != 0.0f && !IgnoreMovementInput)
	{
		// add movement in that direction
		MovingStraight = true;
		GamepadMoveDown = true;
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), Value);

		// Set the players movement state
		// Falling
		if (PlayerCharacter->FallingDuration > 0.0f)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
		}
		// Not sprinting or moving slow
		else if (!SprintKeyDown && !WalkSlowKeyDown)
		{
			if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingCrouched;
			}
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalking;
			}
		}
		// Pressing Sprint
		else if (SprintKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::ESprinting;
		}
		// Pressing WalkSlow
		else if (WalkSlowKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingSlow;
		}
	}
	else if (Value == 0.0f)
	{
		GamepadMoveDown = false;

		// If none of the move methods are used, not moving
		if (!NormalMoveFwdDown && !NormalMoveBwdDown && !GamepadMoveDown)
		{
			MovingStraight = false;
			MovemementDirection = 0;
		}

		if (!MovingSideways && !MovingStraight)
		{
			// Set the players movement state
			// Falling
			if (PlayerCharacter->FallingDuration > 0.0f)
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
			}
			// Crouched
			else if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EStandingStillCrouched;
			}
			// Normal standing still
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EStandingStill;
			}
		}	
	}
}

void AFPSPlayerController::MoveFwd(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (Value > 0.0f)
	{
		MovemementDirection = 1;
	}

	if (Value != 0.0f && !IgnoreMovementInput)
	{
		NormalMoveFwdDown = true;
		MovingStraight = true;

		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), Value);

		// Set the players movement state
		// Falling
		if (PlayerCharacter->FallingDuration > 0.0f)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
		}
		// Not sprinting or moving slow
		else if (!SprintKeyDown && !WalkSlowKeyDown)
		{
			if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingCrouched;
			}
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalking;
			}
		}
		// Pressing Sprint
		else if (SprintKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::ESprinting;
		}
		// Pressing WalkSlow
		else if (WalkSlowKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingSlow;
		}
	}
	else if (Value == 0.0f)
	{
		NormalMoveFwdDown = false;

		// If none of the move methods are used, not moving
		if (!NormalMoveFwdDown && !NormalMoveBwdDown && !GamepadMoveDown)
		{
			MovingStraight = false;
			MovemementDirection = 0;
		}
	}
}

void AFPSPlayerController::MoveBwd(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (Value < 0.0f)
	{
		MovemementDirection = -1;
	}

	if (Value != 0.0f && !IgnoreMovementInput)
	{
		NormalMoveBwdDown = true;
		MovingStraight = true;

		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), Value);

		// Set the players movement state
		// Falling
		if (PlayerCharacter->FallingDuration > 0.0f)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
		}
		// Not sprinting or moving slow
		else if (!SprintKeyDown && !WalkSlowKeyDown)
		{
			if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingCrouched;
			}
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalking;
			}
		}
		// Pressing Sprint
		else if (SprintKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::ESprinting;
		}
		// Pressing WalkSlow
		else if (WalkSlowKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingSlow;
		}
	}
	else if (Value == 0.0f)
	{
		NormalMoveBwdDown = false;

		// If none of the move methods are used, not moving
		if (!NormalMoveFwdDown && !NormalMoveBwdDown && !GamepadMoveDown)
		{
			MovingStraight = false;
			MovemementDirection = 0;
		}
	}
}

void AFPSPlayerController::Strafe(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (Value != 0.0f && !IgnoreMovementInput)
	{
		GamepadStrafeDown = true;

		// add movement in that direction
		MovingSideways = true;
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), Value);

		// Set the players movement state
		// Falling
		if (PlayerCharacter->FallingDuration > 0.0f)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
		}
		// Not sprinting or moving slow
		else if (!SprintKeyDown && !WalkSlowKeyDown)
		{
			if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingCrouched;
			}
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalking;
			}
		}
		// Pressing Sprint
		else if (SprintKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::ESprinting;
		}
		// Pressing WalkSlow
		else if (WalkSlowKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingSlow;
		}
	}
	else if (Value == 0.0f)
	{
		GamepadStrafeDown = false;

		// If none of the strafe methods are used, not moving strafing
		if (!NormalStrafeRDown && !NormalStrafeLDown && !GamepadStrafeDown)
		{
			MovingSideways = false;
		}

		if (!MovingSideways && !MovingStraight)
		{
			// Set the players movement state
			// Falling
			if (PlayerCharacter->FallingDuration > 0.0f)
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
			}
			// Crouch
			else if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EStandingStillCrouched;
			}
			// Normal Standstill
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EStandingStill;
			}
		}
	}
}

void AFPSPlayerController::StrafeR(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if(Value != 0.0f && !IgnoreMovementInput)
	{
		NormalStrafeRDown = true;
		MovingSideways = true;

		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), Value);

		// Set the players movement state
		// Falling
		if (PlayerCharacter->FallingDuration > 0.0f)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
		}
		// Not sprinting or moving slow
		else if (!SprintKeyDown && !WalkSlowKeyDown)
		{
			if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingCrouched;
			}
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalking;
			}
		}
		// Pressing Sprint
		else if (SprintKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::ESprinting;
		}
		// Pressing WalkSlow
		else if (WalkSlowKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingSlow;
		}
	}
	else if (Value == 0.0f)
	{
		NormalStrafeRDown = false;

		// If none of the strafe methods are used, not moving strafing
		if (!NormalStrafeRDown && !NormalStrafeLDown && !GamepadStrafeDown)
		{
			MovingSideways = false;
		}
	}
}

void AFPSPlayerController::StrafeL(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (Value != 0.0f && !IgnoreMovementInput)
	{
		NormalStrafeLDown = true;
		MovingSideways = true;

		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), Value);

		// Set the players movement state
		// Falling
		if (PlayerCharacter->FallingDuration > 0.0f)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EFalling;
		}
		// Not sprinting or moving slow
		else if (!SprintKeyDown && !WalkSlowKeyDown)
		{
			if (PlayerCharacter->PlayerCrouched())
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingCrouched;
			}
			else
			{
				PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalking;
			}
		}
		// Pressing Sprint
		else if (SprintKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::ESprinting;
		}
		// Pressing WalkSlow
		else if (WalkSlowKeyDown)
		{
			PlayerCharacter->PlayerMovementState = EPlayerMovementState::EWalkingSlow;
		}
	}
	else if (Value == 0.0f)
	{
		NormalStrafeLDown = false;

		// If none of the strafe methods are used, not moving strafing
		if (!NormalStrafeRDown && !NormalStrafeLDown && !GamepadStrafeDown)
		{
			MovingSideways = false;
		}
	}
}

void AFPSPlayerController::Jump()
{
	if (PlayerCharacter == nullptr)
		return;

	if (!IgnoreMovementInput && !HoldingDoor)
	{
		PlayerCharacter->Jump();
	}
}

void AFPSPlayerController::StopJumping()
{
	if (PlayerCharacter == nullptr)
		return;

	PlayerCharacter->StopJumping();
}

void AFPSPlayerController::PlayerCrouch()
{
	if (PlayerCharacter == nullptr)
		return;

	if (!IgnoreMovementInput && !HoldingDoor)
	{
		if (PlayerCharacter->GetCharacterMovement()->IsCrouching())
		{
			PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = false;
		}
		else if (!PlayerCharacter->GetCharacterMovement()->IsCrouching() && PlayerCharacter->CanCrouch() && !PlayerCharacter->PlayerSprinting())
		{
			PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = true;
		}
	}
}

void AFPSPlayerController::Sprint(float Value)
{
	if (PlayerCharacter == nullptr)
		return;
	
	if (Value > 0.0f && !WalkSlowKeyDown && !HoldingDoor && !CharacterHasPhoneOut && PlayerCharacter->ColdPercentage > 10.0f)
	{
		// Stop crouching
		if (PlayerCharacter->GetCharacterMovement()->IsCrouching() && PlayerCharacter->PlayerMoving())
			PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = false;

		SprintKeyDown = true;

		// Change the movementspeed depending on if the player is strafing or backing
		
		float TempSpeed = (NormalMovementSpeed * SprintMovementMultiplier);
		if (PlayerCharacter->BloodRushActive)
		{
			TempSpeed = (NormalMovementSpeed * SprintMovementMultiplier * PlayerCharacter->BloodRushSpeedMulti);
		}

		// Strafing
		if (MovemementDirection >= 0 && MovingSideways)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				TempSpeed = (TempSpeed * StrafingMovementMultiplier * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				TempSpeed = (TempSpeed * StrafingMovementMultiplier);
			}
		}
		// Backing
		else if(MovemementDirection < 0)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				TempSpeed = (TempSpeed * BackingMovementMultiplier * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				TempSpeed = (TempSpeed * BackingMovementMultiplier);
			}
		}

		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = TempSpeed * SlowZoneSpeedMulti;
	}
	else
	{
		SprintKeyDown = false;
		//PrintText("Sprint: " + FString::SanitizeFloat(Value), -1, FColor::Green, 0.0f);
	}
}

void AFPSPlayerController::WalkSlow(float Value)
{
	if (PlayerCharacter == nullptr)
		return;
	//PrintText("WalkSlow: " + FString::SanitizeFloat(Value), -1, FColor::Green, 0.0f);
	if (Value != 0.0f && !PlayerCharacter->GetCharacterMovement()->IsCrouching() && !SprintKeyDown && !HoldingDoor)
	{
		WalkSlowKeyDown = true;

		// Change the movementspeed depending on if the player is strafing or backing

		float TempSpeed = (NormalMovementSpeed * WalkingMovementMultiplier);
		if (PlayerCharacter->BloodRushActive)
		{
			TempSpeed = (NormalMovementSpeed * WalkingMovementMultiplier * PlayerCharacter->BloodRushSpeedMulti);
		}

		// Strafing
		if (MovemementDirection >= 0 && MovingSideways)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				TempSpeed = (TempSpeed * StrafingMovementMultiplier * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				TempSpeed = (TempSpeed * StrafingMovementMultiplier);
			}
		}
		// Backing
		else if (MovemementDirection < 0)
		{
			if (PlayerCharacter->BloodRushActive)
			{
				TempSpeed = (TempSpeed * BackingMovementMultiplier * PlayerCharacter->BloodRushSpeedMulti);
			}
			else
			{
				TempSpeed = (TempSpeed * BackingMovementMultiplier);
			}
		}

		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = TempSpeed * SlowZoneSpeedMulti;
	}
	else
	{
		WalkSlowKeyDown = false;
	}
}
//---

//--- General
void AFPSPlayerController::Inhale(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (!CanManuallyBreathe || PlayerCharacter->IsDead)
	{
		InhaleDuration = 0.0f;
		CoughDuration = 0.0f;
		ExhaleDuration = 0.0f;
		SuccessfulBreath = false;
	
		return;
	}
	
	// Inhaling
	if (Value != 0.0f && CoughDuration <= 0.0f && ExhaleDuration <= 0.0f)
	{
		SuccessfulBreath = false;

		// Keep track of how long the player has been inhaling
		InhaleDuration += GetWorld()->GetDeltaSeconds();
	
		// Start coughing if the player has been inhaling for too long
		if (InhaleDuration > MaxInhaleDuration)
		{
			CoughDuration = MaxCoughDuration;
			InhaleDuration = 0.0f;
			PlayerCharacter->BreathDelay = 1.0f;
	
			// Make the player more stressed and create noise for the AI to hear.
			PlayerCharacter->AddToStressLevel(30.0f);
			PlayerCharacter->CreateNoise(PlayerCharacter->GetActorLocation(), 25.0f);
		}
	
		//print("Inhaling! , " + FString::SanitizeFloat(InhaleDuration), 31, FColor::Blue);
	}
	// Exhaling
	else if (Value == 0.0f && CoughDuration <= 0.0f && ExhaleDuration <= 0.0f)
	{
		// If the player has inhaled for the minimum succesful inhale length
		// Successful breath taken
		if (InhaleDuration >= MinInhaleDuration)
		{
			ExhaleDuration = InhaleDuration;
			InhaleDuration = 0.0f;
			PlayerCharacter->BreathDelay = 1.0f;
	
			PlayerCharacter->AddToStressLevel(-20.0f);
			SuccessfulBreath = true;
			//print("Successful Breath!", -1, FColor::Blue);
		}
		// Unsuccessful breath
		else if ((InhaleDuration > 0.3f) && (InhaleDuration < MinInhaleDuration))
		{
			ExhaleDuration = InhaleDuration;
			InhaleDuration = 0.0f;
			PlayerCharacter->BreathDelay = 1.0f;
			SuccessfulBreath = false;
		}
		// Barely started inhaling
		else if (InhaleDuration <= 0.3f)
		{
			ExhaleDuration = 0.0f;
			InhaleDuration = 0.0f;
			//PlayerCharacter->BreathDelay = 0.2f;
			SuccessfulBreath = false;
		}
	}
	
	// Exhaling
	if (ExhaleDuration > 0.0f)
	{
		ExhaleDuration -= GetWorld()->GetDeltaSeconds();
		//print("Exhaling! , " + FString::SanitizeFloat(ExhaleDuration), 31, FColor::Blue);
	}
	
	// If the player is coughing
	if (CoughDuration > 0.0f)
	{
		CoughDuration -= GetWorld()->GetDeltaSeconds();
		//print("Coughing!, " + FString::SanitizeFloat(CoughDuration), 31, FColor::Blue);
	}
}

//--- Phone ---//

void AFPSPlayerController::TakeOutPhone()
{
	if (!PlayerCharacter->IsDead && !PlayerCharacter->PlayerSprinting() && PlayerCharacter->PlayerPhone != nullptr)
	{
		//PlayerCharacter->TakeOutPhone(false);
		PlayerCharacter->SetPhoneActive(!PlayerCharacter->HasPhoneOut, false);
		CharacterHasPhoneOut = PlayerCharacter->HasPhoneOut;
	}
}

void AFPSPlayerController::UsePhone()
{
	if (PlayerCharacter->PlayerPhone != nullptr && PlayerHUDWidget != nullptr && PlayerCharacter->HasPhoneOut && !PlayerHUDWidget->PlayerHasChoice && !PlayerCharacter->PlayerPhone->PhoneDead)
	{
		PlayerCharacter->PlayerPhone->Use();
	}
	else if (PlayerCharacter->PlayerPhone != nullptr && PlayerHUDWidget != nullptr && PlayerHUDWidget->PlayerHasChoice)
	{
		PlayerHUDWidget->DialogueSelect();
	}
}

void AFPSPlayerController::ScrollUp()
{
	if (PlayerCharacter->PlayerPhone != nullptr && PlayerHUDWidget != nullptr && PlayerCharacter->HasPhoneOut && !PlayerHUDWidget->PlayerHasChoice && !PlayerCharacter->PlayerPhone->PhoneDead)
	{
		PlayerCharacter->PlayerPhone->ScrollUp();
	}
	else if (PlayerCharacter->PlayerPhone != nullptr && PlayerHUDWidget != nullptr && PlayerHUDWidget->PlayerHasChoice)
	{
		PlayerHUDWidget->DialogueScrollUp();
	}
}

void AFPSPlayerController::ScrollDown()
{
	if (PlayerCharacter->PlayerPhone != nullptr && PlayerHUDWidget != nullptr && PlayerCharacter->HasPhoneOut && !PlayerHUDWidget->PlayerHasChoice && !PlayerCharacter->PlayerPhone->PhoneDead)
	{
		PlayerCharacter->PlayerPhone->ScrollDown();
	}
	else if (PlayerCharacter->PlayerPhone != nullptr && PlayerHUDWidget != nullptr && PlayerHUDWidget->PlayerHasChoice)
	{
		PlayerHUDWidget->DialogueScrollDown();
	}
}

//--- HUD ---//

void AFPSPlayerController::DisplayInteractionText(FText Text, float Duration)
{
	if (PlayerHUDWidget)
		PlayerHUDWidget->DisplayInteractionText(Text, Duration);
}

//void AFPSPlayerController::DisplaySubtitleText(FText Text, float Duration)
//{
//	if (PlayerHUDWidget)
//		PlayerHUDWidget->DisplaySubtitleText(Text, Duration);
//}

//--- Blood corruption ---//

void AFPSPlayerController::AbilityWheel(float Value)
{
	if (PlayerCharacter == nullptr)
		return;

	if (Value > 0.0f)
	{
		PlayerHUDWidget->SetWheelVisible(true);
	}
	else if(PlayerHUDWidget->WheelOpen && Value == 0.0f)
	{
		PlayerHUDWidget->SetWheelVisible(false);
	}
}

void AFPSPlayerController::SelectedBCAbility()
{
	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	if (PlayerHUDWidget->WheelOpen)
	{
		PlayerHUDWidget->BPRebindAbility("SelectedBCAbility");
	}
	else
	{
		PlayerCharacter->SelectBCAbility(SelectedBCBinding);
	}
}

void AFPSPlayerController::BCKey1()
{
	//PrintText("Pressed Key 1", -1, FColor::Red, 2.0f);

	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	if (PlayerHUDWidget->WheelOpen)
	{
		PlayerHUDWidget->BPRebindAbility("BCKey1");
	}
	else
	{
		PlayerCharacter->SelectBCAbility(Key1Binding);
	}
}

void AFPSPlayerController::BCKey2()
{
	//PrintText("Pressed Key 2", -1, FColor::Red, 2.0f);

	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	if (PlayerHUDWidget->WheelOpen)
	{
		PlayerHUDWidget->BPRebindAbility("BCKey2");
	}
	else
	{
		PlayerCharacter->SelectBCAbility(Key2Binding);
	}
}

void AFPSPlayerController::BCKey3()
{
	//PrintText("Pressed Key 3", -1, FColor::Red, 2.0f);

	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	if (PlayerHUDWidget->WheelOpen)
	{
		PlayerHUDWidget->BPRebindAbility("BCKey3");
	}
	else
	{
		PlayerCharacter->SelectBCAbility(Key3Binding);
	}
}

void AFPSPlayerController::BCKey4()
{
	//PrintText("Pressed Key 4", -1, FColor::Red, 2.0f);

	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	if (PlayerHUDWidget->WheelOpen)
	{
		PlayerHUDWidget->BPRebindAbility("BCKey4");
	}
	else
	{
		PlayerCharacter->SelectBCAbility(Key4Binding);
	}
}

void AFPSPlayerController::BCKey5()
{
	//PrintText("Pressed Key 5", -1, FColor::Red, 2.0f);

	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	if (PlayerHUDWidget->WheelOpen)
	{
		PlayerHUDWidget->BPRebindAbility("BCKey5");
	}
	else
	{
		PlayerCharacter->SelectBCAbility(Key5Binding);
	}
}

