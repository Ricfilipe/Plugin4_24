// Fill out your copyright notice in the Description page of Project Settings.


#include "RightCuboidCreation.h"
#include "Operations/BoxCreation.h"
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
#include "CustomBrushes/KhepriRightCuboid.h"


Response RightCuboidCreation::execute()
{
	FTransform objectTrasform(rot, pos - FVector(1, 1, 1), FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriRightCuboid::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	if (true) {
		NewBrush->BrushType = EBrushType::Brush_Add;
	}
	else {
		NewBrush->BrushType = EBrushType::Brush_Subtract;
	}
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());

	UKhepriRightCuboid* builder = (UKhepriRightCuboid*)NewBrush->BrushBuilder;
	builder->X = scale.X * rescale;
	builder->Y = scale.Y * rescale;
	builder->Z = scale.Z * rescale;
	builder->Build(World, NewBrush);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	return Response(Primitive::ConvertToStaticMesh(bs, FString("RightCuboid" +
		FString::SanitizeFloat(scale.X) + "_" +
		FString::SanitizeFloat(scale.Y) + "_" +
		FString::SanitizeFloat(scale.Z)
	)));
}