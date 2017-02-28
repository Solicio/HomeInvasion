// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MainMenuWidget.h"

#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:

		AMenuPlayerController(const FObjectInitializer& ObjectInitializer);

		//---- HUD ----//

		// The class that will be used for the players Inventory HUD
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu GUI")
		TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass;

		// The instance of the players HUD Widget
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu GUI")
		UMainMenuWidget* MainMenuWidget;

	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaSeconds) override;
	
	
};
