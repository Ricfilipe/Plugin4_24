// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API PyramidFrustumCreation : public ActorStaticMeshCreation
{
public:
	PyramidFrustumCreation(FVector pos, TArray<FVector> bs, TArray<FVector> q, AActor* par, UMaterialInterface* mat);
	Response execute() override;

protected:
	TArray<FVector> base;
	TArray<FVector> top;
	
};
