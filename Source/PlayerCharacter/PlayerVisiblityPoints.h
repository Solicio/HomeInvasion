// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PlayerVisiblityPoints.generated.h"

UCLASS()
class HOMEINVASION_API APlayerVisiblityPoints : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		APlayerVisiblityPoints(const FObjectInitializer& ObjectInitializer);

		// Root, attach onto this
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General Variables")
		USceneComponent* RootComp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		bool Active;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Variables")
		FName SocketName;

	protected:	

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

	
	
};
