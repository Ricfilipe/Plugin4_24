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
	Sphere,Cube,Cylinder,Cone,RightCuboid,Pyramid,PyramidFrustum,Slab,Delete,PlaceMesh, LoadMat, LoadRes, Panel, Chair,Addition,Subtraction, PyramidFrustumWall,PointLight,Camera,Render, Spotlight
};



class BUTTONEXAMPLE_API Operation
{

public:
	Operation();
	TypeOP op;

	//Transform 
	FVector pos;
	FVector scale;
	FRotator rot;
	AActor* parent = NULL;

	//Auxiliar Parameters for object creation



	//Loaded Files



	//Path
	FString path;








	//render


	FString name;
	int param[3];


	virtual  Response execute();

	void OnLevelSequenceFinished();
	void OnPIEViewportStarted();
protected:
	

	/*Create Actors*/


	//Static Mesh




	//Light


	/*Delete Actors*/


	/*Render*/
	AActor* CreateUpdateCamera();



	/*Load Files*/
	UMaterialInterface* LoadMaterial();
	UStaticMesh* LoadResources();


	/*Boolean Operations*/


	/*Auxiliar Funcions*/







};
