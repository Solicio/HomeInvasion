// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class AFPSPlayerController;

UENUM(BlueprintType)
enum class EPlayerBreathingState
{
	ENormal,
	EInhaling,
	EExhaling,
	ESuccessfulBreath,
	ECoughing
};

UENUM(BlueprintType)
enum class EFadeState
{
	EFading,
	EFadedOut,
	EFadedIn
};

UENUM(BlueprintType)
enum class EFadeDirection
{
	EFadeIn,
	EFadeOut
};

UENUM(BlueprintType)
enum class EHUDState
{
	EGame,
	EPhone,
	EPaused,
	EInventory,
	EMap,
	EInspect,
	EAbilityWheel,
	EUpgrade
};

/**
 * 
 */
UCLASS()
class HOMEINVASION_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
	
		UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer);

		UPROPERTY(BlueprintReadOnly, Category = "General Variables")
		AFPSPlayerController* PlayerController;

		/** Gets the current breathing state the player is in  */
		UFUNCTION(BlueprintPure, Category = "General Functions")
		EPlayerBreathingState GetBreathingState();

		// If the interaction image should be hidden from the HUD
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool HideInteractImage;

		// The name of the object that the player is hovering.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		FString InteractionName;

		/** Get the current state of the HUD. */
		UFUNCTION(BlueprintPure, Category = "General Variables")
		EHUDState GetHUDState();

		// If the HUD should be showing the saving graphic
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool IsSaving;


		//-- Interaction Text --//

		// Text that is displayed when the player interacts with an object or something happens to the player
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Text")
		FText InteractionText;

		// Time to display the interaction text, reduced with delta time. = 0, stop showing the text.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Text")
		float InteractionTextDuration;

		/** Used to display interaction text on the HUD for specified duration */
		UFUNCTION(BlueprintCallable, Category = "Interaction Text")
		void DisplayInteractionText(FText Text, float Duration = 2.0f);

		UFUNCTION(BlueprintCallable, Category = "Interaction Text")
		void DisplayInteractionString(FString String, float Duration = 2.0f);


		//-- Pause Menu --//

		UFUNCTION(BlueprintCallable, Category = "Pause Menu")
		void TogglePause();

		UFUNCTION(BlueprintImplementableEvent, Category = "Pause Menu")
		void BPTogglePause();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pause Menu")
		bool IsPaused;


		//-- Inventory Menu --//

		UFUNCTION(BlueprintCallable, Category = "Inventory Menu")
		void ToggleInventory(bool GiveMode);

		UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Menu")
		void BPToggleInventory(bool GiveMode);

		UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Menu")
		void UpdateInventory();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Menu")
		bool InventoryActive;


		//-- Map Menu --//

		UFUNCTION(BlueprintCallable, Category = "Map GUI")
		void ToggleMap();

		UFUNCTION(BlueprintImplementableEvent, Category = "Map GUI")
		void BPToggleMap();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map GUI")
		bool MapActive;


		//-- Inspect Mode --//

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inspect GUI")
		void BPOpenInspectMode(const FName& ObjectName);

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inspect GUI")
		void BPCloseInspectMode();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect GUI")
		bool InspectModeActive;


		//-- Ability Wheel --//
		
		UFUNCTION(BlueprintCallable, Category = "Ability Wheel GUI")
		void SetWheelVisible(bool NewVisible);

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Ability Wheel GUI")
		void BPWheelVisible(bool Visible);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Wheel GUI")
		bool WheelOpen;

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Ability Wheel GUI")
		void BPRebindAbility(const FString& ActionName);


		//-- Dialogue GUI --//

		// General variable to see if the player is in a dialogue with someone.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		bool DialogueActive;

		// Used to see if the current dialogue is through the phone or not.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		bool PhoneDialogueActive;

		// If the player has to make a choice to continue the conversation.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		bool PlayerHasChoice;

		// General variable to see if the player is in a dialogue with someone.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		TArray<FName> CompletedDialogues;

		/** Used by the dialogue system to select what to say */
		UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
		void DialogueScrollUp();

		/** Used by the dialogue system to select what to say */
		UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
		void DialogueScrollDown();

		/** Used by the dialogue system to select what to say */
		UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
		void DialogueSelect();


		//-- Objectives --//

		/** Used by the dialogue system to select what to say */
		UFUNCTION(BlueprintImplementableEvent, Category = "Objectives")
		void UpdatedObjective();

		/** Used by the dialogue system to select what to say */
		UFUNCTION(BlueprintImplementableEvent, Category = "Objectives")
		void AddedObjective();
		

		//-- Blood Corruption --//

		/** Tell the player that the ability is on cooldown */
		UFUNCTION(BlueprintImplementableEvent, Category = "Blood Corruption")
		void DisplayCooldownAlert();

		/** Tell the player that the player does not have enough blood */
		UFUNCTION(BlueprintImplementableEvent, Category = "Blood Corruption")
		void DisplayCostAlert();

		/** Tell the player that the player does not have the ability unlocked */
		UFUNCTION(BlueprintImplementableEvent, Category = "Blood Corruption")
		void DisplayLockedAlert();

		/** Set if the upgrade screen should be visible or not */
		UFUNCTION(BlueprintCallable, Category = "Blood Corruption")
		void SetUpgradeScreenVisible(bool NewVisible);

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Blood Corruption")
		void BPUpgradeScreenChange(bool Visible);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood Corruption")
		bool UpgradeScreenVisible;


		//-- Objectives --//

		/** Set if the save screen should be visible or not */
		UFUNCTION(BlueprintCallable, Category = "Save Station")
		void SetSaveScreenVisible(bool NewVisible);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Station")
		bool SaveScreenVisible;

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Save Station")
		void BPSaveScreenChange(bool Visible);


		//-- Subtitles --//

		//// Text that is displayed as subtitles for audio clips of people speaking.
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Text")
		//FText SubtitleText;

		//// Time to display the interaction text, reduced with delta time. = 0, stop showing the text.
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Text")
		//float SubtitleTextDuration;

		///** Used to display subtitle text on the HUD for specified duration */
		//UFUNCTION(BlueprintCallable, Category = "Subtitle Text")
		//void DisplaySubtitleText(FText Text, float Duration = 2.0f);


		//-- Fade In/Out --//

		/** Fade from specified color to the game */
		UFUNCTION(BlueprintCallable, Category = "Fade In/Out")
		void FadeIn(FColor Color = FColor::Black, float Length = 1.0f);

		/** Fade from game to the specified color */
		UFUNCTION(BlueprintCallable, Category = "Fade In/Out")
		void FadeOut(FColor Color = FColor::Black, float Length = 1.0f);

		// Opacity of fade image
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade In/Out")
		float FadeOpacity;

		// If the fade has been completed. Fully visible game or fully blocked by the fade image
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade In/Out")
		bool FadeComplete;

		/** Get the current state of the fade. Fully faded out/in or currently not known */
		UFUNCTION(BlueprintPure, Category = "Fade In/Out")
		EFadeState GetFadeState();
		
		// Color of the fade image
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade In/Out")
		FLinearColor FadeColor;


	private:

		// Direction the image should be faded
		EFadeDirection CurrentFadeDirection;

		// The current state the fade is in.
		EFadeState CurrentFadeState;

		// Speed the image should fade at. FadeSpeed * DeltaTime.
		float FadeSpeed;


	protected:

		/** Called every frame */
		virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
