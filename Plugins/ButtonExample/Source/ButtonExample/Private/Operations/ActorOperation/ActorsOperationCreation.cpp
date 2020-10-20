// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/ActorOperation/ActorsOperationCreation.h"
#include "CustomBrushes/KhepriMesh.h"
#include "Engine/Polys.h"

ActorsOperationCreation::ActorsOperationCreation(TArray<AActor*> acs)
{
	actors = acs;
}

Response ActorsOperationCreation::execute()
{
	return Response();
}

AActor* AuxiliarFunctions::convertMeshtoBrush(UStaticMesh* m, FRotator rot, FVector pos, bool addictive) {
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriMesh::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	if (addictive) {
		NewBrush->BrushType = EBrushType::Brush_Add;
	}
	else {
		NewBrush->BrushType = EBrushType::Brush_Subtract;
	}
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());
	UKhepriMesh* builder = (UKhepriMesh*)NewBrush->BrushBuilder;
	builder->mesh = m;
	builder->Build(World, NewBrush);

	//Optimize 
	FPoly::OptimizeIntoConvexPolys(NewBrush, NewBrush->Brush->Polys->Element);

	GEditor->RebuildAlteredBSP();
	return NewBrush;
}
