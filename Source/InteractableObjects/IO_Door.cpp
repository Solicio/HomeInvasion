// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "IO_Door.h"

#include "../PlayerCharacter/FPSCharacter.h"

// Sets default values
AIO_Door::AIO_Door(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Creation of look at location component
	LookAtSceneComp = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("LookAt"));
	//LookAtSceneComp->AttachTo(Mesh);
	LookAtSceneComp->SetupAttachment(Mesh);

	// Initiate the various boolean variables
	Locked = false;
	RemoveKeyAfterUnlocking = false;
	KeyName = "None";

	OpensIntoNegativeValue = false;

	MovingForward = false;
	MovingBackward = false;
	MovingPastClosedRot = false;

	OpensOneWay = true;
	CanAutoOpen = true;

	PlayerHeld = false;
	AIHeld = false;
	MaximumYawRotation = 120.0f;
	AIClosedRotation = MaximumYawRotation;

	// Setting physics and collision settings for the mesh
	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(1.0f);
	Mesh->SetEnableGravity(false);

	Tags.Add("Door");

	Name = "Door";

	OnActorHit.AddDynamic(this, &AIO_Door::HitOtherActor);
}

// Called when the game starts or when spawned
void AIO_Door::BeginPlay()
{
	Super::BeginPlay();

	// Set the closed rotation to the actors original rotation.
	ClosedRotation = OriginalRotation;
}

// Called every frame
void AIO_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Check direction of movement
	float DirectionVelocity = 0.0f;
	DirectionVelocity = FVector::DotProduct(Mesh->GetForwardVector(), Mesh->GetComponentVelocity());
	
	if (DirectionVelocity > 0.1f)
	{
		Moving = true;
		MovingForward = true;
		MovingBackward = false;
	}
	else if (DirectionVelocity < -0.1f)
	{
		Moving = true;
		MovingForward = false;
		MovingBackward = true;
	}
	// Not moving or movement so small that it can't be seen
	else if (DirectionVelocity >= -0.1f && DirectionVelocity <= 0.1f)
	{
		Moving = false;
		MovingForward = false;
		MovingBackward = false;
	}

	// Speed at which the door is moving at
	Speed = Mesh->GetComponentVelocity().Size();
	
	// Check if the door is trying to move past the closed rotation while moving in the wrong direction.
	if (OpensOneWay)
	{
		// If the door opens into positive yaw values
		if (!OpensIntoNegativeValue)
		{
			// If it is, stop it and set the rotation to the closed rotation.
			if (MovingBackward && Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw - 10.0f, ClosedRotation.Roll), 10.0f))
			{
				MovingPastClosedRot = true;
				Mesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
				Mesh->SetWorldRotation(ClosedRotation);

				if (Debugging)
				{
					PrintText("Moving in wrong direction", -1, FColor::Red, 1.0f);
				}
			}
			else
			{
				MovingPastClosedRot = false;
			}
		}
		// If the door opens into negative yaw values
		else
		{
			// If it is, stop it and set the rotation to the closed rotation.
			if (MovingBackward && Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw + 10.0f, ClosedRotation.Roll), 10.0f))
			{
				MovingPastClosedRot = true;
				Mesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
				Mesh->SetWorldRotation(ClosedRotation);

				if (Debugging)
				{
					PrintText("Moving in wrong direction", -1, FColor::Red, 1.0f);
				}
			}
			else
			{
				MovingPastClosedRot = false;
			}
		}

		// Check if the door is moving past the maximum rotation
		if (MovingForward && !Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw, ClosedRotation.Roll), MaximumYawRotation))
		{
			// Stop the door and move it back
			MovingPastMaximumRot = true;
			Mesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
			
			if (!OpensIntoNegativeValue)
			{
				Mesh->SetWorldRotation(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw + MaximumYawRotation, ClosedRotation.Roll));
			}
			else
			{
				Mesh->SetWorldRotation(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw - MaximumYawRotation, ClosedRotation.Roll));
			}

			Mesh->SetSimulatePhysics(false);

			if (Debugging)
			{
				PrintText("Moving past max rot!", -1, FColor::Red, 1.0f);
			}
		}
		else
		{
			MovingPastMaximumRot = false;
		}
	}


	// Check to see if the door is closed, if the door is close enough to the closed rotation
	if (Mesh->GetComponentRotation().Equals(ClosedRotation, 3.0f))
	{
		Closed = true;
		AIClosed = true;
		
		// Turn off physics so that it wont look weird if the player walks into it while it is closed. 
		// Only turn it on if the door is not moving past the closed rotation
		if ((!PlayerHeld && !AIHeld && AutoOpened <= 0.0f && AIOpened <= 0.0f) || MovingPastClosedRot)
		{
			Mesh->SetSimulatePhysics(false);
			Mesh->SetWorldRotation(ClosedRotation);
		}
	}
	// Not closed
	else
	{
		Closed = false;

		//if (!Mesh->IsSimulatingPhysics())
		//{
		//	Mesh->SetSimulatePhysics(true);
		//}

		// Check if the door is open enough for the AI
		// If the door is within X degrees of the closed rotation, make sure to open it more for the ai to go through
		if (Mesh->GetComponentRotation().Equals(ClosedRotation, (AIClosedRotation - 0.5f)))
		{
			AIClosed = true;
		}
		else
		{
			AIClosed = false;
		}
	}

	// If the door is not simulating physics and it is being held. Turn on Physics.
	// Also check wether the door is locked and is not moving in a weird direction.
	if (!Mesh->IsSimulatingPhysics() && (PlayerHeld || AIHeld) && !Locked && !MovingPastClosedRot && !MovingPastMaximumRot)
	{
		Mesh->SetSimulatePhysics(true);
	}

	// Reduce the value after AutoOpen
	if (AutoOpened > 0.0f)
		AutoOpened -= DeltaTime;

	if (AIOpened > 0.0f)
		AIOpened -= DeltaTime;

	if (Debugging)
	{
		if (Closed)
		{
			PrintText("Closed", 41, FColor::Green);
		}
		else
		{
			PrintText("Not Closed", 41, FColor::Red);
		}

		if (AIClosed)
		{
			PrintText("AI Closed", 45, FColor::Green);
		}
		else
		{
			PrintText("Not AI Closed", 45, FColor::Red);
		}

		PrintText("Current Rot: " + Mesh->GetComponentRotation().ToString(), 42, FColor::Green);
		PrintText("Closed Rot: " + ClosedRotation.ToString(), 44, FColor::Green);
	}
}

void AIO_Door::Interact(AActor* InteractingActor)
{
	Super::Interact(InteractingActor);

}

void AIO_Door::MoveDoor(float MouseInputVal, FName HandleName)
{

	if (Locked)
	{
		ShouldUnlock();
		return;
	}

	// If the door is not simulating physics or if the AI is holding the door, return
	// The AI has priority on opening the door
	if (!Mesh->IsSimulatingPhysics() || AIHeld || AIOpened > 0.0f)
		return;

	// Make sure there is not a too high input value
	float ClampedInputVal = FMath::Clamp(MouseInputVal, -2.0f, 2.0f);

	// Stop the door if the player is holding it but not moving it
	if (ClampedInputVal == 0.0f)
	{
		FVector TempVelocity = Mesh->GetComponentVelocity() * 0.5f;
		Mesh->SetPhysicsAngularVelocity(TempVelocity);
	}

	// Decide which direction the door should be moved in depending on which handle is grabbed
	if (HandleName == "Handle_Out")
	{
		ClampedInputVal = -ClampedInputVal;
	}

	// Calculate the force direction and strength
	FVector DoorHandleLoc = Mesh->GetSocketLocation("Handle_In");
	FVector DoorHandleRot = Mesh->GetSocketRotation("Handle_In").Vector();

	// The force vector to be added onto the door
	FVector PushForce = DoorHandleRot * (ClampedInputVal * 15000.0f);

	// Add force to the door
	Mesh->AddForceAtLocation(PushForce, DoorHandleLoc);
}

void AIO_Door::AutoOpen(FName HandleName)
{
	// Check if the door is locked or held by AI
	// The AI has priority on opening the door
	if (AIHeld || !CanAutoOpen || AIOpened > 0.0f)
		return;

	if (Locked)
	{
		ShouldUnlock();
		return;
	}

	// Make sure that it can take the impulse
	if (!Mesh->IsSimulatingPhysics())
	{
		Mesh->SetSimulatePhysics(true);
	}

	// Fully stop the door before moving it, to prevent the door from flying at too high of a speed.
	Mesh->SetPhysicsAngularVelocity(FVector::ZeroVector);

	// Dircetion of the impulse, -1/1
	float Direction = 1.0f;

	// Set the direction of the impulse based on which handle is used, how open the door is and if it can only be opened one way
	if (HandleName == "Handle_Out")
	{
		// If the door can open both ways
		Direction = -1.0f;
		if (Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw - 100.0f, ClosedRotation.Roll), 50.0f))
		{	
			Direction = 1.0f;
		}

		// Can only open one way
		if (OpensOneWay)
		{
			Direction = 1.0f;
			if ((Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw - 180.0f, ClosedRotation.Roll), 90.0f)))
			{
				Direction = -1.0f;
			}
		}
	}
	else if (HandleName == "Handle_In")
	{
		// If the door can open both ways
		Direction = 1.0f;
		if (Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw + 100.0f, ClosedRotation.Roll), 50.0f))
		{
			Direction = -1.0f;
		}

		// Can only open one way
		if (OpensOneWay)
		{
			if (Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw + 180.0f, ClosedRotation.Roll), 100.0f))
			{
				Direction = -1.0f;
			}
		}
	}
	
	// Calculate the force direction and strength
	FVector DoorHandleLoc = Mesh->GetSocketLocation("Handle_Out");
	FVector DoorHandleRot = Mesh->GetSocketRotation("Handle_Out").Vector();

	// The force vector to be added onto the door
	FVector PushImpulse = DoorHandleRot * (Direction * 15000.0f);

	// Add the impulse
	Mesh->AddImpulseAtLocation(PushImpulse, DoorHandleLoc);

	AutoOpened = 0.5f;
}

void AIO_Door::SetDoorRotation(FRotator Rotation)
{
	Mesh->SetWorldRotation(Rotation);
}

FRotator AIO_Door::GetDoorRotation()
{
	return Mesh->GetComponentRotation();
}

bool AIO_Door::AIAutoOpen()
{
	// If the door is locked return false. Should make the AI look for a new path into the room or something.
	if (Locked)
	{
		ShouldUnlock();
		return false;
	}

	// Make sure that it can take the impulse
	if (!Mesh->IsSimulatingPhysics())
	{
		Mesh->SetSimulatePhysics(true);
	}

	// Fully stop the door before moving it, to prevent the door from flying at too high of a speed.
	Mesh->SetPhysicsAngularVelocity(FVector::ZeroVector);

	// Dircetion of the impulse, -1/1
	float Direction = 1.0f;

	// Calculate the force direction and strength
	FVector DoorHandleLoc = Mesh->GetSocketLocation("Handle_Out");
	FVector DoorHandleRot = Mesh->GetSocketRotation("Handle_Out").Vector();

	// Reduce power if the door is somewhat opened to make sure it does not bounce back in.
	float Power = 25000.0f;
	//if (Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw + AIClosedRotation, ClosedRotation.Roll), 30.0f) || Mesh->GetComponentRotation().Equals(FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw - AIClosedRotation, ClosedRotation.Roll), 30.0f))
	//{
	//	if (Debugging)
	//	{
	//		PrintText("Reduce Power.", -1, FColor::Cyan, 2.0f);
	//	}

	//	Power = (Power * 0.5f);
	//}

	// The force vector to be added onto the door
	FVector PushImpulse = DoorHandleRot * (Direction * Power);

	// Add the impulse
	Mesh->AddImpulseAtLocation(PushImpulse, DoorHandleLoc);

	AutoOpened = 0.5f;
	AIOpened = 3.0f;

	return true;
}

void AIO_Door::HitOtherActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr)
	{
		if (!Closed)
		{
			if (!Mesh->IsSimulatingPhysics())
			{
				Mesh->SetSimulatePhysics(true);
			}
		}
	}
}

void AIO_Door::ShouldUnlock()
{
	AFPSCharacter* Player; 
	Player = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player != nullptr)
	{
		if (Player->HasItem(KeyName))
		{
			// Key was found, unlock the door.
			Locked = false;
			Player->ShowDisplayInteractionString("Unlocked Door");

			// Remove the key if the door should consume the key.
			if (RemoveKeyAfterUnlocking)
			{
				Player->RemoveInventoryItemByName(KeyName);
			}
		}
		else
		{
			// No key, tell the player that the door is locked.
			Player->ShowDisplayInteractionString("Locked");
		}
	}


}