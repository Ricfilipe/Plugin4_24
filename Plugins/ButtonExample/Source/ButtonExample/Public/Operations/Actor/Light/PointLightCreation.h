// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Actor\Light\LightCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API PointLightCreation : public LightCreation
{
public:
	PointLightCreation(FVector position, AActor* par, FLinearColor clr, float intnsty, float rg);
    Response execute() override;
};
