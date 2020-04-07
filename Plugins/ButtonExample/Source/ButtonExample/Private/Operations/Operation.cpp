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
#include "CustomBrushes/KhepriPyramid.h"
#include "CustomBrushes/KhepriPyramidFrustum.h"
#include "CustomBrushes/VertixBuilder.h"
#include "Engine/Polys.h"


 int count_Pyramid;

 UStaticMesh* lookForMesh(FString mesh) {
	 return LoadObject<UStaticMesh>(nullptr, *(mesh));
 }


Response Operation::execute()
{
	UStaticMesh* loaded_mesh = NULL;
	switch (op) {
	case Sphere:
		return Response(CreateSphere());
		break;
	case Cube:
		loaded_mesh = lookForMesh(FString("/Game/Box" +
			FString::SanitizeFloat(scale.X) + ":" +
			FString::SanitizeFloat(scale.Y) + ":" +
			FString::SanitizeFloat(scale.Z)));
		if (loaded_mesh){
			return Response(PlaceStaticMesh(loaded_mesh));
		}
		return Response(CreateCube());
		break;
	case Cone:
		return Response(CreateCone());
		break;
	case Cylinder:
		loaded_mesh = lookForMesh(FString("/Game/Cylinder" +
			FString::SanitizeFloat(height) + ":" +
			FString::SanitizeFloat(radius) + ":" +
			FString::SanitizeFloat(50)));
		if (loaded_mesh) {
			return Response(PlaceStaticMesh(loaded_mesh));
		}
		return Response(CreateCylinder());
		break;
	case RightCuboid:
		loaded_mesh = lookForMesh(FString("/Game/RightCuboid" +
					FString::SanitizeFloat(scale.X) + ":" +
					FString::SanitizeFloat(scale.Y) + ":" +
					FString::SanitizeFloat(scale.Z)));
		if (loaded_mesh){
			return Response(PlaceStaticMesh(mesh));
		}
		return Response(CreateRightCuboid());
		break;
	case Pyramid:
		return Response(CreatePyramid());
		break;
	case PyramidFrustum:
		return Response(CreatePyramidFrustum());
		break;
	case Slab:
		return Response(CreateSlab());
		break;
	case LoadMat:
		return Response(LoadMaterial());
		break;
	case LoadRes:
		return Response(LoadResources());
		break;
	case PlaceMesh:
		return Response(PlaceStaticMesh(mesh));
		break;
	case Panel:
		return Response(CreatePanel());
		break;
	case Delete:
		DeleteSelected();
		return Response();
		break;
	case Chair:
		return Response(CreateChair());
		break;
	}

	return Response();
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
	builder->Height =  height;
	builder->Radius = radius*rescale;
	builder->Sides = 50;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Cylinder" +
		FString::SanitizeFloat(height) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(builder->Sides)
	));

	realActor->SetActorRotation(rot);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
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
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Cone" +
		FString::SanitizeFloat(height) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(builder->Sides)
	));

	realActor->SetActorRotation(rot);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
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
	if (add) {
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
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Box" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	));

	realActor->SetActorRotation(rot);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	return realActor;
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
	if (add) {
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
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/RightCuboid" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	));


	realActor->SetActorRotation(rot);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	return realActor;
}

AActor* Operation::CreatePyramid()
{
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriPyramid::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	if (add) {
		NewBrush->BrushType = EBrushType::Brush_Add;
	}
	else {
		NewBrush->BrushType = EBrushType::Brush_Subtract;
	}
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());

	UKhepriPyramid* builder = (UKhepriPyramid*)NewBrush->BrushBuilder;
	builder->top = topPoint;
	builder->base = base;
	builder->Size = base.Num();
	builder->Build(World, NewBrush);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Pyramid" +
		FString::SanitizeFloat(count_Pyramid++)
	));

	realActor->SetActorRotation(rot);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
}

AActor* Operation::CreatePyramidFrustum()
{
	FTransform objectTrasform(rot, pos, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriPyramidFrustum::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	if (add) {
		NewBrush->BrushType = EBrushType::Brush_Add;
	}
	else {
		NewBrush->BrushType = EBrushType::Brush_Subtract;
	}
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());

	UKhepriPyramidFrustum* builder = (UKhepriPyramidFrustum*)NewBrush->BrushBuilder;
	builder->top = top;
	builder->base = base;
	builder->Size = base.Num();
	builder->Build(World, NewBrush);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/PyramidFrustum" +
		FString::SanitizeFloat(count_Pyramid++)
	));

	realActor->SetActorRotation(rot);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
}


AActor* spawnCustom(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, int size, bool addictive) {
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UVertixBuilder::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	NewBrush->BrushType = EBrushType::Brush_Add;
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());
	if (addictive) {
		NewBrush->BrushType = EBrushType::Brush_Add;
	}
	else {
		NewBrush->BrushType = EBrushType::Brush_Subtract;
	}
	UVertixBuilder* builder = (UVertixBuilder*)NewBrush->BrushBuilder;
	builder->DrawVertices = vertices;
	builder->Size = size;
	builder->Height = height;
	builder->Build(World, NewBrush);

	//Before Optimization Triangulate

	TArray<FPoly> triangles;
	FPoly cap1 = NewBrush->Brush->Polys->Element.Pop();

	FPoly cap2 = NewBrush->Brush->Polys->Element.Pop();

	FPoly cap3 = NewBrush->Brush->Polys->Element.Pop();

	FPoly cap4 = NewBrush->Brush->Polys->Element.Pop();

	cap1.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		NewBrush->Brush->Polys->Element.Add(p);
	}
	triangles.Empty();
	cap2.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		NewBrush->Brush->Polys->Element.Add(p);
	}

	cap3.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		NewBrush->Brush->Polys->Element.Add(p);
	}
	triangles.Empty();
	cap4.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		NewBrush->Brush->Polys->Element.Add(p);
	}

	//Optimize 
  //  FPoly::OptimizeIntoConvexPolys(NewBrush, NewBrush->Brush->Polys->Element);

	GEditor->RebuildAlteredBSP();
	return NewBrush;

}


AActor* Operation::CreateSlab()
{
	TArray<AActor*> bs;
	bs.Add(spawnCustom(FVector(0,0,0), FRotator(0,0,0), height, base, base.Num(), true));

	for (int i = 0; i < holes.Num(); i++) {
		bs.Add(spawnCustom(FVector(0, 0, 0), FRotator(0, 0, 0), height, holes[i], holes[i].Num(), false));
	}


	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/PyramidFrustum" +
		FString::SanitizeFloat(count_Pyramid++)
	));
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
}

AActor* Operation::CreatePanel()
{
	FTransform objectTrasform(FRotator(0, 0, 0), FVector(0,0,0), FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UVertixBuilder::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	NewBrush->BrushType = EBrushType::Brush_Add;
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());
	UVertixBuilder* builder = (UVertixBuilder*)NewBrush->BrushBuilder;
	builder->DrawVertices = base;
	builder->Size = base.Num();
	builder->NormalVector = topPoint;
	builder->Height = topPoint.Size();
	builder->Build(World, NewBrush);

	//Before Optimization Triangulate

	TArray<FPoly> triangles;
	FPoly cap1 = NewBrush->Brush->Polys->Element.Pop();

	FPoly cap2 = NewBrush->Brush->Polys->Element.Pop();

	cap1.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		NewBrush->Brush->Polys->Element.Add(p);
	}
	triangles.Empty();
	cap2.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		NewBrush->Brush->Polys->Element.Add(p);
	}

	//Optimize 
  //  FPoly::OptimizeIntoConvexPolys(NewBrush, NewBrush->Brush->Polys->Element);

	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Panel" +
		FString::SanitizeFloat(count_Pyramid++)
	));

	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
}

AActor* Operation::CreateChair()
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot, pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AStaticMeshActor::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	AStaticMeshActor* realActor = Cast<AStaticMeshActor>(newActorCreated);
	realActor->SetActorLabel("StaticMesh");
	realActor->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Game/FreeFurniturePack/Meshes/SM_Old_Chair.SM_Old_Chair")));
	realActor->SetActorScale3D(objectScale);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);





	return realActor;
}


AActor* Operation::CreateEmptyActor()
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot, pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AActor::StaticClass();

	AActor* realActor = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);
	realActor->SetActorLabel("StaticMesh");
	realActor->SetActorScale3D(objectScale);
	return realActor;
}

void Operation::DeleteSelected()
{
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	for (int i = 0; i < selectedActors.Num(); i++) {
		
		currentWorld->EditorDestroyActor(selectedActors[i],true);
	}
	selectedActors.Empty();
	
	currentWorld->CleanupActors();
	GEditor->EditorUpdateComponents();
	currentWorld->UpdateWorldComponents(true, false);
	currentWorld->ForceGarbageCollection(true);
	GLevelEditorModeTools().MapChangeNotify();
}

AActor* Operation::PlaceStaticMesh(UStaticMesh* mesh)
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot,pos , objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel * currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AStaticMeshActor::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	AStaticMeshActor* realActor = Cast<AStaticMeshActor>(newActorCreated);
	realActor->SetActorLabel("StaticMesh");
	realActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
	realActor->SetActorScale3D(objectScale); 
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	
	
	

	return realActor;
}

UMaterial* Operation::LoadMaterial()
{
	return LoadObject<UMaterial>(nullptr, *path);
}

UStaticMesh* Operation::LoadResources()
{
	return LoadObject<UStaticMesh>(nullptr, *path);
}




