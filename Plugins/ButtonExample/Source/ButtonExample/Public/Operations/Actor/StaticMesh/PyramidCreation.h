// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API PyramidCreation : public ActorStaticMeshCreation
{
public:
	PyramidCreation(FVector pos, TArray<FVector> bs, FVector q, AActor* par, UMaterialInterface* mat);
	Response execute() override;
protected:
	TArray<FVector> base;
	FVector top;
};
