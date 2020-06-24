// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Actor\Light\LightCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API SpotlightCreation : public LightCreation
{
public:
	Response execute() override;
	SpotlightCreation(FVector position, FRotator rot, AActor* par, FLinearColor clr, float intnsty, float rg, float inner, float outer);
protected:
	float inner_cone;
	float outer_cone;
};
