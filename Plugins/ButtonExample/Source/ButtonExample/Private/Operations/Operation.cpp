#include "..\..\Public\Operations\Operation.h"
// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StaticMeshComponent.h"
#include "Builders/TetrahedronBuilder.h"
#include "UObject/UObjectGlobals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "EditorModeManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Primitive.h"




#include "LevelEditorViewport.h"


#include "Slate/SceneViewport.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/GameMode.h"



#define LOCTEXT_NAMESPACE "Operations"






 UStaticMesh* lookForMesh(FString mesh) {
	
	 mesh = mesh.Replace(TEXT("."), TEXT("a"));
	 mesh = mesh.Replace(TEXT(":"), TEXT("b"));
	 return LoadObject<UStaticMesh>(nullptr, *(mesh ));

 }


 Operation::Operation()
 {
	
 }

 Response Operation::execute()
{

	UStaticMesh* loaded_mesh = NULL;
	switch (op) {
	case LoadMat:
		return Response(LoadMaterial());
		break;
	case LoadRes:
		return Response(LoadResources());
		break;

	case Camera:
		return Response(CreateUpdateCamera());
		break;

	}



	return Response();
}




UMaterialInterface* Operation::LoadMaterial()
{
	return LoadObject<UMaterialInterface>(nullptr, *path);
}

UStaticMesh* Operation::LoadResources()
{


	return LoadObject<UStaticMesh>(nullptr, *path);
}





AActor* Operation::CreateUpdateCamera()
{ 

	GCurrentLevelEditingViewportClient->SetViewLocation(pos);
	GCurrentLevelEditingViewportClient->SetLookAtLocation(scale);

	return NULL;
}




#undef LOCTEXT_NAMESPACE