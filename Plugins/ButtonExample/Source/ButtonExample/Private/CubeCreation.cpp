// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeCreation.h"
#include "UObject/UObjectGlobals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "EditorModeManager.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACubeCreation::ACubeCreation()
{

	static ConstructorHelpers::FObjectFinder<UStaticMesh> myStaticMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	mesh = myStaticMesh.Object;

}

UStaticMesh* ACubeCreation::getMesh() {
	return mesh;
}



