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
#include "Builders/CylinderBuilder.h"
#include "Builders/ConeBuilder.h"
#include "Builders/CubeBuilder.h"

Response Operation::execute()
{
	switch (op) {
	case Sphere:
		return Response(CreateSphere());
		break;
	case Cube:
		return Response(CreateCube());
		break;
	case Cone:
		return Response(CreateCone());
		break;
	case Cylinder:
		return Response(CreateCylinder());
		break;
	}
	return Response(NULL);
}



AActor* Operation::CreateSphere() {
	FTransform objectTrasform(rot,pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UTetrahedronBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UTetrahedronBuilder* builder = (UTetrahedronBuilder*)NewBrush->BrushBuilder;
	builder->Radius = radius*2;
	builder->SphereExtrapolation = 5;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	return Primitive::ConvertToStaticMesh(bs);

}

AActor* Operation::CreateCylinder() {
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UCylinderBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UCylinderBuilder* builder = (UCylinderBuilder*)NewBrush->BrushBuilder;
	builder->Z = 2 * height;
	builder->OuterRadius = radius*2;
	builder->Sides = 50;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	return Primitive::ConvertToStaticMesh(bs);
}

AActor* Operation::CreateCone()
{
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UConeBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UConeBuilder* builder = (UConeBuilder*)NewBrush->BrushBuilder;
	builder->Z = 200 * height;
	builder->OuterRadius = radius;
	builder->Sides = 50;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	return Primitive::ConvertToStaticMesh(bs);
}

AActor* Operation::CreateCube()
{
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UCubeBuilder::StaticClass(), NAME_None, RF_Transactional);
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

	UCubeBuilder* builder = (UCubeBuilder*)NewBrush->BrushBuilder;
	builder->X = scale.X;
	builder->Y = scale.Y;
	builder->Z = scale.Z;
	builder->Build(World, NewBrush);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	return Primitive::ConvertToStaticMesh(bs);
}




