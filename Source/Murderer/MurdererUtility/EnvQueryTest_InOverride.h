// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_InOverride.generated.h"

/**
 * 
 */
UCLASS()
class HOMEINVASION_API UEnvQueryTest_InOverride : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
	
	float Radius = 50.0f;
	
};
