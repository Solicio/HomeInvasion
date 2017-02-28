// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "../PlayerCharacter/FPSCharacter.h"
#include "MurdererUtility/DayOverrideZone.h"

#include "Perception/PawnSensingComponent.h"

#include "GameFramework/Character.h"
#include "MurdererCharacter.generated.h"

class AMurdererController;

UENUM(BlueprintType)
enum class EMovementState
{
	ENormal,
	EForcedInterest,
	EChasing
};

/** Base character class for all AI's.
*	All AI characters will inherit from this class.
*/

UCLASS()
class HOMEINVASION_API AMurdererCharacter : public ACharacter
{
	GENERATED_BODY()

	public:

		/** Sets default values for this character's properties */
		AMurdererCharacter(/*const FObjectInitializer& ObjectInitializer*/);

		UPROPERTY(EditAnywhere, Category = Behavior)
		class UBehaviorTree* AIBehavior;


		//-- Sensing --//

		// Range that sounds can be heard from
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Sensing")
		float HearingRange;

		// Range that actors can be seen from
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Sensing")
		float SightRange;

		// Range which the AI looses sight of actors
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Sensing")
		float LoseSightRange;

		// The angle the AI can see actors
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Sensing")
		float PeripheralVisionAngle;

		// Sphere component used to check if the sound heard is within a overlap box
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sensing")
		USphereComponent* SoundCheckSphere;

		// Used by the aggro increase function to see if the AI has the players flashlight on the character.
		// If the value is > 0 the players flashlight is or has been on the AI. Value is decreased over time.
		//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Sensing")
		//float PlayerFlashlightOnBody;

		/** Directly add a value to the Aggro value, positive or negative */
		UFUNCTION(BlueprintCallable, Category = "AI Sensing")
		void AddToAggro(float Value);

		/** Tells the controller that the players flashlight is on the character */
		UFUNCTION(BlueprintCallable, Category = "AI Sensing")
		void FlashlightOnSelf();

		//-- Attacking --//

		// The range that the AI can attack from
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		float AttackRange;

		// If the AI is allowed to attack
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool CanAttack;


		//-- Movement --//

		// Normal movementspeed the AI moves at
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float NormalMoveSpeed;

		// Normal movementspeed the AI moves at
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float ForcedInterestMoveSpeed;

		// Normal movementspeed the AI moves at
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float ChaseMoveSpeed;


		/** Function used to have the AI move to a certain location. Get AI Pawns from GameMode and call this function. */
		UFUNCTION(BlueprintCallable, Category = "AI Functions")
		void ForceInterestInLoc(FVector Location);

		/** Enable or disable the outline on the character */
		UFUNCTION(BlueprintCallable, Category = "General")
		void SetOutlineActive(bool Active, int32 StencilValue);

	private:
		
		bool ExecutedOnce;

	protected:

		AMurdererController* CurrentController;
		//AFPSCharacter* PlayerCharacter;

		UFUNCTION()
		void HitOtherActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

		/** Override to get the location from which the sense cone should be made (Where the eyes are) */
		virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
};
