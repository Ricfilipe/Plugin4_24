// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations/Actor/StaticMesh/ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API BoxCreation : public ActorStaticMeshCreation
{
public:
	BoxCreation(FVector position, FRotator rotation, FVector scale, AActor* parent, UMaterialInterface* mat);
	Response execute() override;
};
