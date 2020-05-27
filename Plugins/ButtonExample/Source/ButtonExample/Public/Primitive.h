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
#include <string>


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

	//Creating StaticMesh From Brushes

	AActor* ConvertToStaticMesh();
	void spawnCustom(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, int size, bool addictive);
	AActor* ConvertToStaticMesh(FString name);
	AActor* ConvertToStaticMesh(const TArray<AActor*> bs, FString name);
	AActor* ConvertToStaticMesh( const TArray<AActor*> bs);

	//BIM constructions
		//BEAM


		//Slabs


	void cleanBrushes();

	AActor* addBrushToConversion(ABrush * brush);

	TArray<AActor*> getArray();
	


	//API KHEPRI

	int Sphere(float x, float y, float z, float radius);

	int StaticMesh(char* label, char* myStaticMesh, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, const char* mat);

	int Cylinder(FVector top, float radius, FVector bot);

	int Cone(float px, float py, float pz, float rx, float ry, float rz, float height, float radius);

	int  Box(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz);

	int  RightCuboid(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz, float angle);

	int  Pyramid(TArray<FVector> ps, FVector q);

	int  PyramidFrustum(TArray<FVector> ps, TArray<FVector> q);

	int  PyramidFrustumWithMaterial(TArray<FVector> ps, TArray<FVector> q, int material);

	int Slab(TArray<FVector> contour, TArray<TArray<FVector>> holes, float h, int material);

	 int CurrentParent();

	 int SetCurrentParent(int newParent);

	 int LoadMaterial(std::string path);

	 int LoadResource(std::string path);

	 int CreateBlockInstance(int mesh, FVector position, FVector vx, FVector vy, float scale);

	 int CurrentMaterial();

	 int SetCurrentMaterial(int material);

	 int Panel(TArray<FVector>  pts, FVector n, int material);

	 int BeamRectSection(FVector position, FVector vx, FVector vy, float dx, float dy, float dz, float angle, int material);

	 int BeamCircSection(FVector bot, float radius, FVector top, int material);

	int CopyMesh(char* label, int actor, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, const char* mat);

	int Chair(FVector pos, float angle);

	int DeleteAll();
	
	int InstantiateBIMElement(int family, FVector pos, float angle);

	int Subtract(int ac1, int ac2);

	int Unite(int ac1, int ac2);

	int DeleteMany(TArray<int> acs);

}




