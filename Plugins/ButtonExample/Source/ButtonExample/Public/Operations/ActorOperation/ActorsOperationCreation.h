// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Operation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API ActorsOperationCreation : public Operation
{
public:
	ActorsOperationCreation(TArray<AActor*> acs);
	virtual Response execute();
protected:
	TArray<AActor*> actors;
};


namespace AuxiliarFunctions {
	AActor* convertMeshtoBrush(UStaticMesh* m, FRotator rot, FVector pos, bool addictive);
}