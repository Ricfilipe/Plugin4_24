// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorStaticMeshCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API CylinderCreation : public ActorStaticMeshCreation
{
public:

	CylinderCreation(FVector position, FRotator rotation, float rad, float hght, AActor* parent , UMaterialInterface* mat);
	Response execute() override;
protected:
	float radius;
	float height;
};
