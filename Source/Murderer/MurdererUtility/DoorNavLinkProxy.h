// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Navigation/NavLinkProxy.h"
#include "DoorNavLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API ADoorNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

	public:

		// Relative to the main actor
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Link)
		FVector LinkStart;
		
		// Relative to the main actor
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Link)
		FVector LinkEnd;

		UFUNCTION(BlueprintCallable, Category = Link)
		void SetEnabledAreaClass(TSubclassOf<UNavArea> AreaClass);

		UFUNCTION(BlueprintCallable, Category = Link)
		virtual bool GetNavigationLinksArray(TArray<FNavigationLink>& OutLink, TArray<FNavigationSegmentLink>& OutSegments) const override;

		UFUNCTION(BlueprintCallable, Category = Link)
		void SetNavLinkPoints(FVector Start, FVector End);

		UFUNCTION(BlueprintCallable, Category = Link)
		void SetSmartLinkPoints(FVector Start, FVector End);

		virtual bool IsNavigationRelevant() const override;
};
