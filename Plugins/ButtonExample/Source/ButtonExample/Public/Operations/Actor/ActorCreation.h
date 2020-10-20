// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Operation.h"


/**
 * 
 */

class BUTTONEXAMPLE_API ActorCreation : public Operation
{
public:
	ActorCreation();
	ActorCreation(FVector position, FRotator rotation, FVector scl, AActor* par);
	Response execute() override;

protected:
	FVector pos;
	FVector scale;
	FRotator rot;
	AActor* parent = NULL;
};
