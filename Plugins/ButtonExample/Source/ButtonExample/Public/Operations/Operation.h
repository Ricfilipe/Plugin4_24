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
	Sphere,Cube,Cylinder,Cone,RightCuboid,Pyramid,PyramidFrustum,Slab,Delete,PlaceMesh, LoadMat, LoadRes, Panel, Chair,Addition,Subtraction, PyramidFrustumWall
};

class BUTTONEXAMPLE_API Operation 
{


	struct Face {
		int v1;
		int v2;
		int v3;
		short materialID;
		FVector2D uvCoords1;
		FVector2D uvCoords2;
		FVector2D uvCoords3;
		FVector UDirection1;
		FVector UDirection2;
		FVector UDirection3;
		FVector VDirection;

	};


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
	UMaterialInterface* mat = NULL;
	UStaticMesh* mesh = NULL;
	UPackage* pk;
	Response execute(UPackage* Package );
	
private:
	
	AActor* CreateSphere();
	AActor* CreateCylinder();
	AActor* CreateCone();
	AActor* CreateCube();
	AActor* CreateRightCuboid();
	AActor* CreatePyramid();
	AActor* CreatePyramidFrustum();
	AActor* CreatePyramidFrustumWall();
	AActor* CreateSlab();
	AActor* CreatePanel();
	AActor* CreateChair();
	AActor* CreateEmptyActor();
	void DeleteSelected();
	AActor* PlaceStaticMesh(UStaticMesh* mesh);
	UMaterialInterface* LoadMaterial();
	UStaticMesh* LoadResources();
	AActor* CreateSubtration();
	AActor* CreateAddition();
	UStaticMesh* CreateMesh(FString name, TArray<FVector> vertices, TArray<Face> faces, int size);
};
