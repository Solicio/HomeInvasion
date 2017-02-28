// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Murderer/MurdererCharacter.h"

#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "Blueprint/UserWidget.h"

#include "Engine/GameInstance.h"
#include "HomeInvasionGameInstance.generated.h"

/** Base class of the GameInstance used by the game.
 *  This class will store things such as what murderer to use for each level.
 */
UCLASS()
class HOMEINVASION_API UHomeInvasionGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	public:
		
		UHomeInvasionGameInstance(const FObjectInitializer& ObjectInitializer);

		// Checked by each level to see what type of murderer to spawn. Usually set in the MainMenu when selecting level.
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Murderer")
		TSubclassOf <AMurdererCharacter> MurdererToSpawn;
	

		//--- Loading Screen ---//

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
		float MinLoadTime;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
		TSubclassOf <class UWidget> LoadingScreenWidgetClass;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
		UWidget* LoadScreenWidgetRef;

		virtual void Init() override;

		UFUNCTION()
		virtual void BeginLoadingScreen(const FString& MapName);
		
		UFUNCTION()
		virtual void EndLoadingScreen(/*const FString& MapName*/);

};
