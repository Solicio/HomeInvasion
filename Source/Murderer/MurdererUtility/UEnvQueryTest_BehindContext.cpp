// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "UEnvQueryTest_BehindContext.h"


UUEnvQueryTest_BehindContext::UUEnvQueryTest_BehindContext(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);
	CheckValue = 0.7f;
}

void UUEnvQueryTest_BehindContext::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	BoolValue.BindData(QueryOwner, QueryInstance.QueryID);

	bool bToMatch = BoolValue.GetValue();

	TArray<AActor*> ContextActors;
	if (!QueryInstance.PrepareContext(ContextToCheckAgainst, ContextActors))
	{
		return;
	}

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());

		// Check if the location is behind the context
		//const bool bIsBehind = false;

		for (int32 ContextIndex = 0; ContextIndex < ContextActors.Num(); ContextIndex++)
		{
			const FVector ForwardDir = ContextActors[ContextIndex]->GetActorForwardVector();
			const FVector Offset = (ItemLocation - ContextActors[ContextIndex]->GetActorLocation()).GetSafeNormal();
			const float FrontBack = FVector::DotProduct(ForwardDir, Offset);

			if (FrontBack < CheckValue)
			{
				It.SetScore(TestPurpose, FilterType, true, bToMatch);
			}
			else
			{
				It.SetScore(TestPurpose, FilterType, false, bToMatch);
			}
		}
	}
}

FText UUEnvQueryTest_BehindContext::GetDescriptionTitle() const
{
	return FText::FromString(FString("Behind Context"));
}

FText UUEnvQueryTest_BehindContext::GetDescriptionDetails() const
{
	return FText::FromString(FString("Behind Context Test"));
}

//bool AFPSCharacter::LocationInFront(FVector inLocation)
//{
//	// Check if the location is in front of the player
//
//	FVector Dir = GetActorForwardVector();
//	FVector Offset = inLocation - GetActorLocation();
//	Offset = Offset.GetSafeNormal();
//	float FrontBack = FVector::DotProduct(Dir, Offset);
//
//	return (FrontBack > 0.7f/*FMath::Cos(90.0f) / 2.0f*/);
//}
