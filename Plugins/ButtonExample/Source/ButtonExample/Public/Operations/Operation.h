// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tracks/MovieSceneCameraCutTrack.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "Operations/Response.h"
#include "AutomatedLevelSequenceCapture.h"
/**
 * 
 */
#define rescale 100



enum TypeOP
{
	Sphere,Cube,Cylinder,Cone,RightCuboid,Pyramid,PyramidFrustum,Slab,Delete,PlaceMesh, LoadMat, LoadRes, Panel, Chair,Addition,Subtraction, PyramidFrustumWall,PointLight,Camera,Render
};

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

class BUTTONEXAMPLE_API Operation
{

public:

	TypeOP op;

	//Transform 
	FVector pos;
	FVector scale;
	FRotator rot;
	AActor* parent = NULL;

	//Auxiliar Parameters for object creation

	float radius;
	float height;
	FVector topPoint;

	//Loaded Files

	UMaterialInterface* mat = NULL;
	UStaticMesh* mesh = NULL;

	//Path
	FString path;


	//Lists of Points
	TArray<FVector> base;
	TArray<FVector> top;
	TArray<TArray<FVector>> holes;

	//Color
	FLinearColor color;

	//Boolean Operation Parameter
	bool add = true;

	//Multiple selection of Actors
	TArray<AActor*> selectedActors;

	//render
	UAutomatedLevelSequenceCapture* MovieSceneCapture;
	FString name;
	int param[3];

	virtual Response execute(UPackage* Package);
protected:
	

	/*Create Actors*/
	AActor* CreateEmptyActor();

	//Static Mesh
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


	//Light
	AActor* CreatePointLight();

	/*Delete Actors*/
	void DeleteSelected();
	

	/*Render*/
	AActor* CreateUpdateCamera();
	AActor* CreateRender();


	/*Load Files*/
	UMaterialInterface* LoadMaterial();
	UStaticMesh* LoadResources();


	/*Boolean Operations*/

	AActor* CreateSubtration();
	AActor* CreateAddition();

	/*Auxiliar Funcions*/


	void OnLevelSequenceFinished();
	void OnPIEViewportStarted();

	AActor* PlaceStaticMesh(UStaticMesh* mesh);
	UStaticMesh* CreateMesh(FString name, TArray<FVector> vertices, TArray<Face> faces, int size);


};
