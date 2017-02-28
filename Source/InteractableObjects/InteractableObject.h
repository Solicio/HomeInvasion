// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

UCLASS()
class HOMEINVASION_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
	public:	
		
		/** Sets default values for this actor's properties */
		AInteractableObject(const FObjectInitializer& ObjectInitializer);

		// Root component to hold all the other components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scene Comp")
		USceneComponent* SceneRoot;

		// Cube component for trace detecting
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace Collisions Comp")
		UBoxComponent* TraceCollisionBox;

		// The mesh of the object 
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Comp")
		UStaticMeshComponent* Mesh;


		// ----- Functions ----- //

		/** Interact with the object, when the player is looking at the object (Default LMB) */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		virtual void Interact(AActor* InteractingActor);

		/** This event is called by the Interact() function, can be used for blueprint events */
		UFUNCTION(BlueprintImplementableEvent, Category = "Object Functions")
		void InteractEvent(bool AlreadyInteracted, AActor* InteractingActor);

		/** Add this item to be destroyed on the save system. */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		void AddToBeDestroyed();

		// ----- Public Variables ----- //

		// Name of the object
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		FString Name;

		// An unique id for the interactable object. Used when saving and loading.
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Object Variables")
		FGuid GUID;

		// This bool is used by the save system when looking for interactable objects that should get their own guid value.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool NeedsGuidSet;

		// Original position in the level
		UPROPERTY(VisibleAnywhere, Category = "Object Variables")
		FVector OriginalPosition;

		// Original rotation in the level
		UPROPERTY(VisibleAnywhere, Category = "Object Variables")
		FRotator OriginalRotation;

		// If the object is active, if the object can be interacted with by the player
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool Active;

		// If the object has been interacted with
		UPROPERTY(VisibleAnywhere, Category = "Object Variables")
		bool InteractedWith;

		// If the item should be highlighted when the player is using rank 3 huntervision
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables")
		bool UsefulItem;

		// Which color it should use. 0-255
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Variables", meta = (EditCondition = "UsefulItem"))
		int32 UsefulItemStencil;

		/** This event is called by the Interact() function, can be used for blueprint events */
		UFUNCTION(BlueprintCallable, Category = "Object Functions")
		void SetOutlineActive(bool OutlineActive, int32 StencilValue);


	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;
	
		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;

};
