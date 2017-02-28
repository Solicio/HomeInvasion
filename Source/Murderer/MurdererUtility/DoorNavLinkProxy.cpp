// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "DoorNavLinkProxy.h"

#include "AI/Navigation/NavLinkCustomComponent.h"


void ADoorNavLinkProxy::SetEnabledAreaClass(TSubclassOf<UNavArea> AreaClass)
{
	GetSmartLinkComp()->SetEnabledArea(AreaClass);
	UNavigationSystem::UpdateActorAndComponentsInNavOctree(*this);
}

bool ADoorNavLinkProxy::GetNavigationLinksArray(TArray<FNavigationLink>& OutLink, TArray<FNavigationSegmentLink>& OutSegments) const
{
	OutLink.Append(PointLinks);
	OutSegments.Append(SegmentLinks);

	return (PointLinks.Num() > 0) || (SegmentLinks.Num() > 0);
}

void ADoorNavLinkProxy::SetNavLinkPoints(FVector Start, FVector End)
{
	if (PointLinks.Num() >= 1)
	{
		PointLinks[0].Left = Start;
		PointLinks[0].Right = End;
	}
}

void ADoorNavLinkProxy::SetSmartLinkPoints(FVector Start, FVector End)
{
	if (bSmartLinkIsRelevant)
		GetSmartLinkComp()->SetLinkData(Start, End, ENavLinkDirection::BothWays);
}


bool ADoorNavLinkProxy::IsNavigationRelevant() const
{
	return (PointLinks.Num() > 0) || (SegmentLinks.Num() > 0) || bSmartLinkIsRelevant;
}