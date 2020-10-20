// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Operations/Actor/StaticMesh/ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API SlabCreation : public ActorStaticMeshCreation
{
public:
    SlabCreation(FVector pos, TArray<FVector> bs, TArray<TArray<FVector>> holes, float hght, AActor* par, UMaterialInterface* mat);
    Response execute() override;
protected:
    TArray<FVector> base;
    TArray<TArray<FVector>> _holes;
    float height;
};
