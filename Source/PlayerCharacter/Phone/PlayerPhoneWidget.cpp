// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "PlayerPhoneWidget.h"

#include "PlayerPhone.h"

UPlayerPhoneWidget::UPlayerPhoneWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UPlayerPhoneWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

}