// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations/Actor/StaticMesh/ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API PanelCreation : public ActorStaticMeshCreation
{
public:
    PanelCreation(FVector pos, TArray<FVector> bs, FVector n , AActor* par, UMaterialInterface* mat);
    Response execute() override;

protected:
    TArray<FVector> base;
    FVector normal;
};
