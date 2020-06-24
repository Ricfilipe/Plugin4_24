// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Actor\ActorCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API LightCreation : public ActorCreation
{
public:
	LightCreation(FVector position, FRotator rotation, AActor* par, FLinearColor clr, float intnsty, float rg);
	virtual  Response execute();
	
protected:
	FLinearColor color;
	float intensity;
	float range;
};
