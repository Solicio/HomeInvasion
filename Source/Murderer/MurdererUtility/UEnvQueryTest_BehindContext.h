// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "UEnvQueryTest_BehindContext.generated.h"

class UEnvQueryContext;

/**
 * 
 */
UCLASS()
class HOMEINVASION_API UUEnvQueryTest_BehindContext : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
	/** context */
	UPROPERTY(EditDefaultsOnly, Category = Distance)
	TSubclassOf<UEnvQueryContext> ContextToCheckAgainst;

	UPROPERTY(EditDefaultsOnly, Category = Distance)
	float CheckValue;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
	
	
};
