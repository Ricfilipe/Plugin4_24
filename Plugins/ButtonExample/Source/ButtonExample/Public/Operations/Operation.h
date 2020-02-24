// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Operations/Response.h"
/**
 * 
 */

enum TypeOP
{
	Sphere,Cube,Cylinder,Cone
};

class BUTTONEXAMPLE_API Operation 
{
public:
	TypeOP op;
	FVector pos;
	FVector scale;
	FRotator rot;
	float radius;
	float height;


	Response execute();
	
private:
	AActor* CreateSphere();
	AActor* CreateCylinder();
	AActor* CreateCone();
	AActor* CreateCube();
};
