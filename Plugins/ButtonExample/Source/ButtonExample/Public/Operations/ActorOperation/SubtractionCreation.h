// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "ActorsOperationCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API SubtractionCreation : public ActorsOperationCreation
{
public:
    SubtractionCreation(TArray<AActor*> acs, AActor* par, UMaterialInterface* mat);
    Response execute() override;

protected:
    UMaterialInterface* material;
    AActor* parent;
};
