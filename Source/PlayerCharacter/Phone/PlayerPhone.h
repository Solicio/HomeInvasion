// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "PlayerPhoneWidget.h"

#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "../../Murderer/MurdererCharacter.h"

#include "PhoneReceptionZone.h"

#include "GameFramework/Actor.h"
#include "PlayerPhone.generated.h"

UCLASS()
class HOMEINVASION_API APlayerPhone : public AActor
{
	GENERATED_BODY()
	
	public:	
	
		/** Sets default values for this actor's properties */
		APlayerPhone(const FObjectInitializer& ObjectInitializer);

		// Root, attach onto this
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		USceneComponent* RootComp;

		// Main mesh of the phone
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		UStaticMeshComponent* PhoneMesh;

		// Used to register for senses and to report noise events.
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		UAIPerceptionStimuliSourceComponent* StimuliSourceComp;

		// Cube component for trace detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
		UBoxComponent* OverlapBox;


		// If the player has the phone out infront of him
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool IsActive;

		/** Used to activate the currently selected element on the phone */
		UFUNCTION(BlueprintCallable, Category = "General")
		void Use();

		/** Used to select the apps on the phone */
		UFUNCTION(BlueprintCallable, Category = "General")
		void ScrollUp();

		/** Used to select the apps on the phone */
		UFUNCTION(BlueprintCallable, Category = "General")
		void ScrollDown();

		/** Called when the player either takes out or puts the phone back */
		UFUNCTION(BlueprintCallable, Category = "General")
		void TakeOutPhone(bool TakingItOut);


		//--- Battery ---//

		// How much battery the phone has left.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
		float BatteryPercentage;

		// How much battery the phone has left.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
		bool PhoneDead;

		// How much base battery level is reduced every second. 
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
		float BatteryDecrease;

		// Multiplier used when flashlight is on, decreases battery quicker. This value is multiplied with the base decrease.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
		float FlashlightMultiplier;

		/** Sets the battery percentage of the phone. */
		UFUNCTION(BlueprintCallable, Category = "Battery")
		void SetBatteryPercentage(float Percentage);


		//--- Flashlight ---//

		// The flashlight that is attached to the phone
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
		USpotLightComponent* Flashlight;

		// Used to check if the flashlight is hitting an AI and to place the visibility points for the AI to see.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
		float TraceLength;

		// Location used to place out the visibility points that the AI will see, at the end or at collision
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
		FVector EndPointLocation;

		// Location used to place out the visibility points that the AI will see, halfway to the end
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
		FVector HalfPointLocation;

		// All the murderers that are in the flashlight cone
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
		TArray<AMurdererCharacter*> MurdererCharacters;


		//--- Reception ---//

		// The current reception the phone has
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		int32 CurrentReception;

		/** Used to check the current overlapping zones to see what the highest reception is */
		UFUNCTION(BlueprintCallable, Category = "General")
		void CheckReceptionZones();


		//--- Widget ---//

		// The 3D widget component that is used for the screen of the phone
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
		UWidgetComponent* PhoneScreenWidget;

		// The instance of the players HUD Widget
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
		UPlayerPhoneWidget* PhoneWidget;


		//-- Noise --//

		//// Strength of the noise the phone will give of
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
		//float NoiseStrength;

		/** Used to activate the currently selected element on the phone */
		UFUNCTION(BlueprintCallable, Category = "Noise")
		void CreateNoise(FVector Location, float Loudness);

		// Player
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Noise")
		ACharacter* Player;


	protected:

		// Trace parameters for the flashlight line trace
		FCollisionQueryParams TraceParams;

		// Handle to manage the time
		FTimerHandle ReceptionCheckTimer;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
