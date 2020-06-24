// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API StaticMeshCreation : public ActorStaticMeshCreation
{
public:
	StaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par, UStaticMesh* msh);
	Response execute() override;
protected:
	UStaticMesh* mesh = NULL;
};
