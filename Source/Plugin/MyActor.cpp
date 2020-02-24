// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
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
AMyActor::AMyActor()
{


	PrimaryActorTick.bCanEverTick = true;

}



