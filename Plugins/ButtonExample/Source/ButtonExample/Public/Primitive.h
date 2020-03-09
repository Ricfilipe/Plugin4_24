#pragma once

#include "UObject/UObjectGlobals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "EditorModeManager.h"
#include "Containers/Queue.h"
#include "Operations/Operation.h"
#include <vector>

namespace Primitive {
	
	
	bool begin = false;

	int counter = 0;



	//place StaticMEsh

	int PlacingStaticMesh(char * label, char * myStaticMesh, FVector objectPosition, FRotator objectRotation, FVector objectScale, const char * mat);

	int PlacingStaticMesh(char* label, int32 actor, FVector objectPosition, FRotator objectRotation, FVector objectScale, const char* mat);

	bool checkQueue(float delta,int SpF);

	//Brushes



	/*
	int32 spawnCube(FVector objectPosition, FRotator objectRotation, FVector objectScale);

	int32 spawnCube(FVector objectPosition, FRotator objectRotation, FVector objectScale, bool addictive = true);

	int32 spawnCube(FVector objectPosition, FRotator objectRotation, FVector objectScale, bool addictive, bool hollow);
	*/
	void spawnBrushCubeHollow(FVector objectPosition, FRotator objectRotation, FVector objectScale, float wall, bool addictive = true);

	void spawnBrushCylinder(FVector objectPosition, FRotator objectRotation, float height, float radius, int slides, bool addictive=true);

	void spawnBrushCylinderHollow(FVector objectPosition, FRotator objectRotation, float height, float radius, float in_radius, int slides, bool addictive = true);

	void spawnBrushCone(FVector objectPosition, FRotator objectRotation, float height, float radius, int slides, bool addictive = true);

	void spawnBrushConeHollow(FVector objectPosition, FRotator objectRotation, float height, float in_height, float radius, float in_radius, int slides, bool addictive = true);

	void spawnBrushSphere(FVector objectPosition, FRotator objectRotation,  float radius, int tesselation, bool addictive = true);

	//Creating StaticMesh From Brushes

	AActor* ConvertToStaticMesh();
	void spawnCustom(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, int size, bool addictive);
	AActor* ConvertToStaticMesh(FString name);
	AActor* ConvertToStaticMesh(const TArray<AActor*> bs, FString name);
	AActor* ConvertToStaticMesh( const TArray<AActor*> bs);

	//BIM constructions
		//BEAM
	void createBeamRect(FVector objectPosition, FRotator objectRotation, FVector objectScale);
	void createBeamCircle(FVector objectPosition, FRotator objectRotation, float height, float radius, int slides);
	void createBeamCustom(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, int size);

		//Slabs
	void createSlabs(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, TArray<TArray<FVector>> holes, int size, int num_holes, int* size_holes);

	void createPanels(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, FVector Normal, int size);

	void cleanBrushes();

	AActor* addBrushToConversion(ABrush * brush);

	TArray<AActor*> getArray();
	


	//API KHEPRI

	int Sphere(float x, float y, float z, float radius);

	int StaticMesh(char* label, char* myStaticMesh, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, const char* mat);

	int Cylinder(float x, float y, float z, float rx, float ry, float rz, float height, float radius);

	int Cone(float px, float py, float pz, float rx, float ry, float rz, float height, float radius);

	int  Box(float px, float py, float pz, float rx, float ry, float rz, float tx, float ty, float tz, float sx, float sy, float sz);

	int  RightCuboid(float px, float py, float pz, float rx, float ry, float rz, float tx, float ty, float tz, float sx, float sy, float sz);

	int CopyMesh(char* label, int actor, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, const char* mat);
}




