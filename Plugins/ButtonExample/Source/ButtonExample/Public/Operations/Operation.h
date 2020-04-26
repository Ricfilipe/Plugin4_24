// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Operations/Response.h"
/**
 * 
 */
#define rescale 100


enum TypeOP
{
	Sphere,Cube,Cylinder,Cone,RightCuboid,Pyramid,PyramidFrustum,Slab,Delete,PlaceMesh, LoadMat, LoadRes, Panel, Chair,Addition,Subtraction
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
	FVector topPoint;
	TArray<FVector> base;
	TArray<FVector> top;
	TArray<AActor*> selectedActors;
	bool add = true;
	TArray<TArray<FVector>> holes;
	AActor* parent = NULL;
	FString path;
	UMaterial* mat = NULL;
	UStaticMesh* mesh = NULL;
	Response execute();
	
private:
	
	AActor* CreateSphere();
	AActor* CreateCylinder();
	AActor* CreateCone();
	AActor* CreateCube();
	AActor* CreateRightCuboid();
	AActor* CreatePyramid();
	AActor* CreatePyramidFrustum();
	AActor* CreateSlab();
	AActor* CreatePanel();
	AActor* CreateChair();
	AActor* CreateEmptyActor();
	void DeleteSelected();
	AActor* PlaceStaticMesh(UStaticMesh* mesh);
	UMaterial* LoadMaterial();
	UStaticMesh* LoadResources();
	AActor* CreateSubtration();
	AActor* CreateAddition();
};
