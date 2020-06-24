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

	// Auxiliars for Converting fields

	UMaterialInterface* intToUMaterial(int index);
	UStaticMesh* intToUStaticMesh(int index);
	AActor* intToAActor(int index);

	//Queue

	bool checkQueue(float delta,int SpF);


	//Creating StaticMesh From Brushes

	AActor* ConvertToStaticMesh();

	AActor* ConvertToStaticMesh(FString name);
	AActor* ConvertToStaticMesh(const TArray<AActor*> bs, FString name);
	AActor* ConvertToStaticMesh( const TArray<AActor*> bs);

	void cleanBrushes();

	AActor* addBrushToConversion(ABrush * brush);

	TArray<AActor*> getArray();
	


	//API KHEPRI

	//Static Mesh

	int  Box(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz);

	int  RightCuboid(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz, float angle);

	int Cylinder(FVector top, float radius, FVector bot);

	int BeamRectSection(FVector position, FVector vx, FVector vy, float dx, float dy, float dz, float angle, UMaterialInterface* material);

	int BeamCircSection(FVector bot, float radius, FVector top, UMaterialInterface* material);

	int  Pyramid(TArray<FVector> ps, FVector q);

	int  PyramidFrustum(TArray<FVector> ps, TArray<FVector> q);

	int Slab(TArray<FVector> contour, TArray<TArray<FVector>> holes, float h, UMaterialInterface* material);

	 int Panel(TArray<FVector>  pts, FVector n, UMaterialInterface* material);

	int InstantiateBIMElement(UStaticMesh* family, FVector pos, float angle);

	int CreateBlockInstance(UStaticMesh* mesh, FVector position, FVector vx, FVector vy, float scale);

	// Actor Properties

	int CurrentMaterial();

	int SetCurrentMaterial(int material);

	int CurrentParent();

	int SetCurrentParent(int newParent);


	//LIGHT

	int PointLight(FVector position, FLinearColor color, float range, float intensity);

	int Spotlight(FVector position, FVector dir, FLinearColor color, float range, float intensity, float hotspot, float falloff);

	//Boolean Operations

	int Subtract(AActor* ac1, AActor* ac2);

	int Unite(AActor* ac1, AActor* ac2);


	//Delete

	int DeleteMany(TArray<AActor*> acs);

	int DeleteAll();

	//LOAD

	int LoadMaterial(std::string path);

	int LoadResource(std::string path);




	//RENDER

	int SetView(FVector position, FVector target, float lens, float aperture);
   
	FVector ViewCamera();

	FVector ViewTarget();

	float ViewLens();

	int RenderView(int width, int height, std::string name, std::string path, int frame);

}




