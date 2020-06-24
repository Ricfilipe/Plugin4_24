// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorsOperationCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API AdditionCreation : public ActorsOperationCreation
{
public:
    AdditionCreation(TArray<AActor*> acs, AActor* par, UMaterialInterface* mat);
    Response execute() override;

protected:
    UMaterialInterface* material;
    AActor* parent;
};
