// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PlayerHidingSpot.generated.h"


class AFPSCharacter;

UENUM(BlueprintType)
enum class EHidingSpotTypes
{
	ENull,
	EWardrobe,
	ELowHeight
};

UCLASS()
class HOMEINVASION_API APlayerHidingSpot : public AActor
{
	GENERATED_BODY()
	
	public:	
	
		/** Sets default values for this actor's properties */
		APlayerHidingSpot(const FObjectInitializer& ObjectInitializer);

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* SceneRoot;

		// Cube component for trace detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UBoxComponent* OverlapBox;

		// Used to get the location where the AI should walk to before checking the hidingspot
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		UArrowComponent* AICheckLocationArrow;


		//--- General Variables ---//

		// What type this hidingspot is
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		TEnumAsByte<EHidingSpotTypes> SpotType;

		// If the player is inside the overlap box
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		bool PlayerInside;

		// How long the player has been inside this hidingspot
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		float PlayerInsideDuration;

		// Where the AI should run to before checking the hidingspot
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		FVector AICheckLocation;


	protected:

		AFPSCharacter* PlayerFPSCharacter;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

		/** called when something enters the sphere component */
		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		/** called when something leaves the sphere component */
		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
