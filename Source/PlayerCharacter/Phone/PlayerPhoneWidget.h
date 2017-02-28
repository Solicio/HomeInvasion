// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerPhoneWidget.generated.h"

class APlayerPhone;

/** Used to keep track of what the current screen is on the phone.
*	Used to determine what to show on the UMG
*/
UENUM(BlueprintType)
enum class EPhoneScreenState
{
	EHome,
	EPhone,
	ENotes,
	EMap
};

/**
 * 
 */
UCLASS()
class HOMEINVASION_API UPlayerPhoneWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
		
		/** Constructor */
		UPlayerPhoneWidget(const FObjectInitializer& ObjectInitializer);
	
		// The phone this widget is attached to
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		APlayerPhone* PlayerPhone;

		// How long the player has been inside this Zone
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		TEnumAsByte<EPhoneScreenState> ScreenState;


		/** Used to activate the currently selected element on the phone */
		UFUNCTION(BlueprintImplementableEvent, Category = "General")
		void UseApp();

		/** Used to handle input for scrolling */
		UFUNCTION(BlueprintImplementableEvent, Category = "General")
		void ScrollUp();

		/** Used to handle input for scrolling */
		UFUNCTION(BlueprintImplementableEvent, Category = "General")
		void ScrollDown();

		/** Called when the player either takes out or puts the phone back */
		UFUNCTION(BlueprintImplementableEvent, Category = "General")
		void TakeOutPhone(bool TakingItOut);


	protected:

		/** Called every frame */
		virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

};
