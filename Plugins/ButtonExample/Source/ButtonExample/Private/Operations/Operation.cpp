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
#include "CustomBrushes/KhepriCylinder.h"
#include "Builders/ConeBuilder.h"
#include "CustomBrushes/KhepriBox.h"
#include "CustomBrushes/KhepriRightCuboid.h"

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
	case RightCuboid:
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
	builder->Radius = radius*200;
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
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriCylinder::StaticClass(), NAME_None, RF_Transactional);
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
	UKhepriCylinder* builder = (UKhepriCylinder*)NewBrush->BrushBuilder;
	builder->Height = rescale * height;
	builder->Radius = radius*rescale;
	builder->Sides = 50;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AActor*realActor = Primitive::ConvertToStaticMesh(bs, FString("/Game/Cylinder" +
		FString::SanitizeFloat(height) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(builder->Sides)
	));

	realActor->SetActorRotation(rot);
	return realActor;
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
	return Primitive::ConvertToStaticMesh(bs, FString("/Game/Cone" +
		FString::SanitizeFloat(height) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(builder->Sides)
	));
}

AActor* Operation::CreateCube()
{
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriBox::StaticClass(), NAME_None, RF_Transactional);
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

	UKhepriBox* builder = (UKhepriBox*)NewBrush->BrushBuilder;
	builder->X = scale.X * rescale;
	builder->Y = scale.Y * rescale;
	builder->Z = scale.Z * rescale;
	builder->Build(World, NewBrush);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	return Primitive::ConvertToStaticMesh(bs, FString("/Game/Box" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	));
}

AActor* Operation::CreateRightCuboid()
{
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
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
	return Primitive::ConvertToStaticMesh(bs, FString("/Game/RightCuboid" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	));
}




