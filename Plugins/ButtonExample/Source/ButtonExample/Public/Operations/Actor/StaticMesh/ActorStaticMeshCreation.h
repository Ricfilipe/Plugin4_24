// Fill out your copyright notice in the Description page of Project Settings.

#include "Operations\Actor\ActorCreation.h"
#include "polypartition.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

#pragma once
namespace AuxiliarFunctions {
	TArray<int> convertIndices(TArray<FVector> verts, TPPLPolyList polys, FVector delta);

	FVector PolyNormal(TArray<FVector> pts);

	FVector GetUV(FVector v, FVector m[3]);


}

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

/**
 * 
 */
class BUTTONEXAMPLE_API ActorStaticMeshCreation : public ActorCreation
{
public:
	ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par);
	ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor*  par, FString path);
	ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par, UMaterialInterface* mat);
	ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par , UMaterialInterface* mat, FString path);


	Response execute() override;
	
protected:
	FString path;
	UStaticMesh* lookForMesh(FString mesh);
	UMaterialInterface* mat = NULL;

	AActor* PlaceStaticMesh(UStaticMesh* mesh);
	UStaticMesh* CreateMesh(FString name, TArray<FVector> vertices, TArray<Face> faces, int size);

	
};


