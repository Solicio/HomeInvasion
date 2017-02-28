// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "EnvQueryTest_InOverride.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "../../HomeInvasionGameMode.h"

UEnvQueryTest_InOverride::UEnvQueryTest_InOverride(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_InOverride::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* DataOwner = QueryInstance.Owner.Get();
	BoolValue.BindData(DataOwner, QueryInstance.QueryID);

	bool bToMatch = BoolValue.GetValue();

	// GameMode
	AHomeInvasionGameMode* CurrentGameMode = Cast<AHomeInvasionGameMode>(QueryInstance.World->GetAuthGameMode());
	if (CurrentGameMode == nullptr)
		return;

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());

		// Check if the location is in day override 
		const bool bIsInLight = CurrentGameMode->IsLocationInOverride(ItemLocation, true, Radius);
		It.SetScore(TestPurpose, FilterType, bIsInLight, bToMatch);
	}
}

FText UEnvQueryTest_InOverride::GetDescriptionTitle() const
{
	return FText::FromString(FString("In Override"));
}

FText UEnvQueryTest_InOverride::GetDescriptionDetails() const
{
	return FText::FromString(FString("In Override Test"));
}



