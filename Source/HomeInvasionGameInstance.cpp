// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "HomeInvasionGameInstance.h"

UHomeInvasionGameInstance::UHomeInvasionGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MinLoadTime = 5.0f;
}

void UHomeInvasionGameInstance::Init()
{
	UGameInstance::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UHomeInvasionGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UHomeInvasionGameInstance::EndLoadingScreen);

}

void UHomeInvasionGameInstance::BeginLoadingScreen(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	LoadingScreen.MinimumLoadingScreenDisplayTime = MinLoadTime;

	/*UWidget* */LoadScreenWidgetRef = CreateWidget<UWidget>(GetWorld(), LoadingScreenWidgetClass);

	//LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
	LoadingScreen.WidgetLoadingScreen = LoadScreenWidgetRef->TakeWidget();

	//PrintText("START LOADING SCREEN", -1, FColor::Red, 2.0f);

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UHomeInvasionGameInstance::EndLoadingScreen(/*const FString& MapName*/)
{
	//PrintText("END LOADING SCREEN", -1, FColor::Red, 2.0f);
}
