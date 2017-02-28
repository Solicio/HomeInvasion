// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_InLight.generated.h"

class UEnvQueryContext;

/**
 * 
 */
UCLASS()
class HOMEINVASION_API UEnvQueryTest_InLight : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

};
