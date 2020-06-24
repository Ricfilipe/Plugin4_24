// Fill out your copyright notice in the Description page of Project Settings.


#include "PyramidCreation.h"
#include "CustomBrushes/KhepriPyramid.h"
#include "Primitive.h"

int numPyramid;

PyramidCreation::PyramidCreation(FVector pos, TArray<FVector> bs, FVector q, AActor* par, UMaterialInterface* mat) :
	ActorStaticMeshCreation(pos, FRotator(0,0,0), FVector(1, 1, 1), par, mat)
{
	base = bs;
	top = q;
}

Response PyramidCreation::execute()
{
	FTransform objectTrasform(FRotator(0, 0, 0), pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriPyramid::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
		NewBrush->BrushType = EBrushType::Brush_Add;

	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());

	UKhepriPyramid* builder = (UKhepriPyramid*)NewBrush->BrushBuilder;
	builder->top = top;
	builder->base = base;
	builder->Size = base.Num();
	builder->Build(World, NewBrush);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Pyramid" +
		FString::SanitizeFloat(numPyramid++)
	));

	realActor->SetActorRotation(rot);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
}


