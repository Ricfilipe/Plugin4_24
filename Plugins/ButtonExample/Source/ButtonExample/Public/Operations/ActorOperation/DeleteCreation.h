// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorsOperationCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API DeleteCreation : public ActorsOperationCreation
{
public:
    DeleteCreation(TArray<AActor*> acs);
    Response execute() override;
};
