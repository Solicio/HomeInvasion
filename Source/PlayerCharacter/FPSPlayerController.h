// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "PlayerHUDWidget.h"

#include "../InteractableObjects/InteractableObject.h"
#include "../InteractableObjects/IO_Door.h"

#include "../Miscellaneous/SlowZone.h"

#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class AFPSCharacter;

/** The different abilities */
UENUM(BlueprintType)
enum class EBloodCorruptionAbilities
{
	ENone,
	EBloodRush,
	EBloodDash,
	EBeastCalling,
	EHunterVision,
	EVampiricHunger
};

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
	public:

		// Sets default values for this controllers's properties 
		AFPSPlayerController(const FObjectInitializer& ObjectInitializer);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool ShouldPrintInfo;

		// If the controlled character is dead.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Movement")
		bool CharacterDead;


		//------ Movement Variables ---------//

		// The movement speed the player will move at, received from character movement
		float NormalMovementSpeed;
		bool MovingStraight;
		bool MovingSideways;

		// The multipler that is multiplied with the max movement speed. > 1 
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float SprintMovementMultiplier;

		// The multipler that is multiplied with the max movement speed. < 1
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float WalkingMovementMultiplier;

		// The multipler that is multiplied with the normal movement speed when going backwards
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float BackingMovementMultiplier;

		// The multipler that is multiplied with the normal movement speed when strafing
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		float StrafingMovementMultiplier;

		// If mouse input should be ignored for rotation of camera 
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		bool IgnoreMouseLookInput;

		// If the key inputs should be ignored for character movement 
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		bool IgnoreMovementInput;

		// If the player is moving forward or backwards. Positive for forward and negaive for back. 0 for no movement.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Movement")
		int32 MovemementDirection;

		// The current velocity the player is moving at
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Movement")
		float CurrentSpeed;


		// The current slow zone the player is in.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
		ASlowZone* CurrentSlowZone;

		// Multiplier of the player speed while in the zone
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Movement")
		float SlowZoneSpeedMulti;


		// ------ Interacting ------ //

		// If the player currently can interact with objects
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interacting")
		bool CanInteract;

		// The range that the player can interact with objects at
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interacting")
		float RayLength;

		// If the player currently is holding onto a door
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interacting")
		bool HoldingDoor;

		// The door that the player is currently holding on to
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interacting")
		AIO_Door* HeldDoor;

		// The maximum the player can turn in either directions while holding a door
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interacting")
		float MaxTurn;

		// The distance the player can move from the door before the player lets go
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interacting")
		float MaxDistanceFromDoor;
	
		/** Function used to reset door variables */
		UFUNCTION(BlueprintCallable, Category = "Interacting")
		void LetGoOfDoor();


		// ----- Breathing ----- //

		// If the player can manually breathe to reduce stress
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		bool CanManuallyBreathe;

		// The maximum amount of time the player can inhale
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float MaxInhaleDuration;

		// The minimum inhale duration in order to reduce stress
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float MinInhaleDuration;

		// How long the player has been inhaling for
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Breathing")
		float InhaleDuration;

		// How long the player has been exhaling for. Player will exhale for the same length of inhale
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Breathing")
		float ExhaleDuration;

		//  How long the player will be coughing for if inhaling too long.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float MaxCoughDuration;

		// Keeping track of how long the player has been coughing for. Set to Max value when inhaling for too long.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breathing")
		float CoughDuration;

		// If the player has inhaled for the right time and has let go of the key
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Breathing")
		bool SuccessfulBreath;

		// Mouse keys
		bool LeftMouseDown;
		bool RightMouseDown;
		float LeftMouseDownDuration;
		float RightMouseDownDuration;


		//---- Blood Corruption ----//

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		TEnumAsByte<EBloodCorruptionAbilities> Key1Binding;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		TEnumAsByte<EBloodCorruptionAbilities> Key2Binding;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		TEnumAsByte<EBloodCorruptionAbilities> Key3Binding;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		TEnumAsByte<EBloodCorruptionAbilities> Key4Binding;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		TEnumAsByte<EBloodCorruptionAbilities> Key5Binding;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		TEnumAsByte<EBloodCorruptionAbilities> SelectedBCBinding;


		//---- HUD ----//

		// The class that will be used for the players Inventory HUD
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player HUD")
		TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

		// The instance of the players HUD Widget
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player HUD")
		UPlayerHUDWidget* PlayerHUDWidget;

		// If the player has picked up the phone
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone")
		bool CharacterHasPhone;

		// If the player has the phone out infront of the player
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone")
		bool CharacterHasPhoneOut;

		/** Used to display interaction text on the HUD for specified duration */
		UFUNCTION(BlueprintCallable, Category = "Interaction Text")
		void DisplayInteractionText(FText Text, float Duration = 2.0f);

		/** Used to display subtitle text on the HUD for specified duration */
		//UFUNCTION(BlueprintCallable, Category = "Subtitle Text")
		//void DisplaySubtitleText(FText Text, float Duration = 2.0f);


	private:

		// Used to check if the player is trying to sprint or walk slow
		bool SprintKeyDown;
		bool WalkSlowKeyDown;

		bool GamepadMoveDown;
		bool GamepadStrafeDown;

		bool NormalMoveFwdDown;
		bool NormalMoveBwdDown;

		bool NormalStrafeRDown;
		bool NormalStrafeLDown;

		//---- Interacting / Door variables -----//

		// Trace parameters for the line trace
		FCollisionQueryParams TraceParams;

		// Current handle that is held by the player
		FName HandleName;

		// Value that keeps track of how much the player has rotated while holding a door
		float PlayerTurnValue;

	protected:

		// Characcter possesed by this controller
		AFPSCharacter* PlayerCharacter;

		//--- Input functions ---//

		/** Called for forwards/backward input, used for gamepad*/
		void Move(float Value);

		void MoveFwd(float Value);
		void MoveBwd(float Value);

		/** Called for side to side input, used for gamepad*/
		void Strafe(float Value);

		void StrafeR(float Value);
		void StrafeL(float Value);

		/** Mouse Input */
		void MouseVertical(float Value);
		void MouseHorizontal(float Value);

		/** Called when the player wants to jump and override for when the character lands */
		void Jump();
		void StopJumping();

		/** Called for player pressing crouch key */
		void PlayerCrouch();

		/** Used for checking sprint key*/
		void Sprint(float Value);

		/** Used for checking walk slow key */
		void WalkSlow(float Value);

		/** Used to manage inhaling and exhaling*/
		void Inhale(float Value);

		/** Left clicking */
		void LeftClick();
		void LeftClickRelease();

		/** Right clicking */
		void RightClick();
		void RightClickRelease();

		/** Blood corruption keys */
		void AbilityWheel(float Value);
		void SelectedBCAbility();
		void BCKey1();
		void BCKey2();
		void BCKey3();
		void BCKey4();
		void BCKey5();

		/** Phone input functions */
		void TakeOutPhone();

		void UsePhone();

		void ScrollUp();
		void ScrollDown();


		//---

		/** Called to bind functionality to input */
		virtual void SetupInputComponent() override;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;
};
