// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations/Actor/StaticMesh/ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API RightCuboidCreation : public ActorStaticMeshCreation
{
public:
	RightCuboidCreation(FVector position, FRotator rotation, FVector scale, AActor* parent, UMaterialInterface* mat);
	Response execute() override;
};
