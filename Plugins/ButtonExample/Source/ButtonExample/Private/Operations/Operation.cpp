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
#include "CustomBrushes/KhepriMesh.h"
#include "Engine/Polys.h"
#include "RawMesh.h"
#include "GeomTools.h"
#include "polypartition.h"
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"
#include "CineCameraActor.h"

#include "CineCameraComponent.h"
#include "LevelSequenceActor.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Factories/Factory.h"
#include "LevelSequence.h"
#include "Tracks/MovieSceneCameraCutTrack.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "MovieScene.h"
#include "MovieSceneToolHelpers.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Tracks/MovieSceneFloatTrack.h"
#include "LevelEditorViewport.h"


#include "Slate/SceneViewport.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/GameMode.h"
#include "Protocols/AudioCaptureProtocol.h"
#include "Protocols/ImageSequenceProtocol.h"
#include "ImageWriteTypes.h"


#define LOCTEXT_NAMESPACE "FButtonExampleModule"
 int count_Pyramid;



 FVector nextFramePosition;
 FRotator nextFramerotation;
 float nextFrameCamera[3];
 FString filmName;
 ULevelSequence* current = NULL;


 FMovieSceneFloatChannel* posX;
 FMovieSceneFloatChannel* posY;
 FMovieSceneFloatChannel* posZ;

 FMovieSceneFloatChannel* rotX;
 FMovieSceneFloatChannel* rotY;
 FMovieSceneFloatChannel* rotZ;

 FMovieSceneFloatChannel* aperture;
 FMovieSceneFloatChannel* focal;
 FMovieSceneFloatChannel* focus;

 UStaticMesh* lookForMesh(FString mesh) {
	
	 mesh = mesh.Replace(TEXT("."), TEXT("a"));
	 mesh = mesh.Replace(TEXT(":"), TEXT("b"));
	 return LoadObject<UStaticMesh>(nullptr, *(mesh ));

 }


Response Operation::execute(UPackage* Package)
{

	UStaticMesh* loaded_mesh = NULL;
	switch (op) {
	case Sphere:
		return Response(CreateSphere());
		break;
	case Cube:
		loaded_mesh = lookForMesh(FString("/Game/MyStaticMeshes/Box" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	));
		if (loaded_mesh){
			return Response(PlaceStaticMesh(loaded_mesh));
		}
		return Response(CreateCube());
		break;
	case Cone:
		return Response(CreateCone());
		break;
	case Cylinder:
		loaded_mesh = lookForMesh(FString("/Game/MyStaticMeshes/Cylinder" +
			FString::SanitizeFloat(height) + ":" +
			FString::SanitizeFloat(radius) + ":" +
			FString::SanitizeFloat(50)));
		if (loaded_mesh) {
			return Response(PlaceStaticMesh(loaded_mesh));
		}
		return Response(CreateCylinder());
		break;
	case RightCuboid:
		loaded_mesh = lookForMesh(FString("/Game/MyStaticMeshes/RightCuboid" +
					FString::SanitizeFloat(scale.X) + ":" +
					FString::SanitizeFloat(scale.Y) + ":" +
					FString::SanitizeFloat(scale.Z)));
		if (loaded_mesh){
			return Response(PlaceStaticMesh(loaded_mesh)); 
		}
		return Response(CreateRightCuboid());
		break;
	case Pyramid:
		return Response(CreatePyramid());
		break;
	case PyramidFrustum:
		return Response(CreatePyramidFrustum());
		break;
	case PyramidFrustumWall:
		return Response(CreatePyramidFrustumWall());
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
	case Subtraction:
		return Response(CreateSubtration());
		break;
	case Addition:
		return Response(CreateAddition());
		break;
	case PointLight:
		return Response(CreatePointLight());
		break;
	case Camera:
		return Response(CreateUpdateCamera());
		break;
	case Render:
		return Response(CreateRender());
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
	/*
	FTransform objectTrasform(FRotator(0,0,0), pos, FVector(1, 1, 1));
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
	*/

	TArray<FVector> Vertices;
	int InSides = 100;
	for (int32 i = InSides - 1; i >= 0; i--)
		for (int32 j = 0; j < 2; j += 1)
			Vertices.Add(FVector(radius * rescale * FMath::Sin((2 * i) * PI / InSides), radius * rescale * FMath::Cos((2 * i) * PI / InSides), j * height));


	int numberOfVertices = Vertices.Num();

	TArray<Face> Faces;
	Face oneFace;

	double dist = FVector::Distance(Vertices[0], Vertices[2]) / 100;



	for (int i = 0; i < numberOfVertices / 2; i++) {
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(dist * i,0), FVector2D(dist * i, height/100), FVector2D(dist * (i + 1), height / 100),
			Vertices[i * 2]-Vertices[(i * 2 + numberOfVertices-2) % (numberOfVertices)]
			,Vertices[i * 2]-Vertices[(i * 2 + numberOfVertices-2) % (numberOfVertices)]
			,Vertices[(i * 2 + 2) % (numberOfVertices)] - Vertices[i * 2],FVector(0,0,1) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist * (i + 1),height / 100), FVector2D(dist * (i + 1), 0), FVector2D(dist * i, 0),Vertices[(i * 2 + 2) % (numberOfVertices)] - Vertices[i * 2],Vertices[(i * 2 + 2) % (numberOfVertices)] - Vertices[i * 2],Vertices[i * 2] - Vertices[(i * 2 + numberOfVertices-2) % (numberOfVertices)],FVector(0,0,1) };
		Faces.Add(oneFace);
	}

	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(0, 0, height));



	for (int32 i = 0; i < numberOfVertices / 2; i++) {
		FVector uv1 = Vertices[i * 2 + 1] - Vertices[1];
		FVector uv2 = Vertices[((i + 1) % (numberOfVertices / 2)) * 2 + 1] - Vertices[1];
		oneFace = { i * 2 + 1 ,  numberOfVertices + 1,  ((i + 1) % (numberOfVertices / 2)) * 2 + 1,  0,  FVector2D(uv1.X / 100,uv1.Y / 100), -FVector2D(Vertices[0].X / 100,Vertices[0].Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(0,1,0) };
		Faces.Add(oneFace);
		oneFace = { ((i + 1) % (numberOfVertices / 2)) * 2  ,  numberOfVertices,  i * 2   ,  0, FVector2D(uv2.X / 100,uv2.Y / 100), -FVector2D(Vertices[0].X / 100,Vertices[0].Y / 100),  FVector2D(uv1.X / 100,uv1.Y / 100),FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(0,1,0) };
		Faces.Add(oneFace);
	}

	numberOfVertices = Vertices.Num();
	int numberOfFaces = Faces.Num();

	return PlaceStaticMesh(CreateMesh(FString("Cylinder" +
		FString::SanitizeFloat(height) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(50)
	), Vertices, Faces, Vertices.Num()));

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
	/*
	FTransform objectTrasform(FRotator(0, 0, 0), pos, FVector(1, 1, 1));
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
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);

	return realActor;
	*/

	TArray<FVector> Vertices;
	TArray<Face> Faces;

	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(0, 0, scale.Z * rescale));

	Vertices.Add(FVector(scale.X * rescale, 0, 0));
	Vertices.Add(FVector(scale.X * rescale, 0, scale.Z * rescale));

	Vertices.Add(FVector(scale.X * rescale, scale.Y * rescale, 0));
	Vertices.Add(FVector(scale.X * rescale, scale.Y * rescale, scale.Z * rescale));

	Vertices.Add(FVector(0, scale.Y * rescale, 0));
	Vertices.Add(FVector(0, scale.Y * rescale, scale.Z * rescale));

	 
	Face oneFace = { 2,4,0,  0,  FVector2D(scale.X,0), FVector2D(scale.X, scale.Y), FVector2D(0, 0) };
	Faces.Add(oneFace);
	oneFace = { 4,6,0,  0,  FVector2D(scale.X,scale.Y), FVector2D(0, scale.Y), FVector2D(0, 0) };
	Faces.Add(oneFace);

	oneFace = { 1,5,3,  0,  FVector2D(0,0), FVector2D(scale.X, scale.Y), FVector2D(scale.X, 0) };
	Faces.Add(oneFace);
	oneFace = { 1,7,5,  0,  FVector2D(0,0), FVector2D(0, scale.Y), FVector2D(scale.X, scale.Y) };
	Faces.Add(oneFace);

	int numberOfVertices = Vertices.Num();

	for (int i = 0; i < numberOfVertices / 2; i++) {
		float dist = FVector::Distance(Vertices[i * 2 + 1], Vertices[((i * 2 + 3) % (numberOfVertices))]) / 100;
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(0,0), FVector2D(0,  scale.Z), FVector2D(dist,  scale.Z) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist, scale.Z), FVector2D(dist, 0), FVector2D(0, 0) };
		Faces.Add(oneFace);
	}
	return PlaceStaticMesh(CreateMesh(FString("Box" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	),Vertices,Faces,Vertices.Num()));



	


}

AActor* Operation::CreateRightCuboid()
{
	/*
	FTransform objectTrasform(FRotator(0,0,0), pos, FVector(1, 1, 1));
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
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	return realActor;
	*/

	TArray<FVector> Vertices;
	TArray<Face> Faces;

	Vertices.Add(FVector(-scale.X * rescale / 2, -scale.Y * rescale / 2, 0));
	Vertices.Add(FVector(-scale.X * rescale / 2, -scale.Y * rescale / 2, scale.Z * rescale));

	Vertices.Add(FVector(scale.X * rescale/2, -scale.Y * rescale / 2, 0));
	Vertices.Add(FVector(scale.X * rescale / 2, -scale.Y * rescale / 2, scale.Z * rescale));

	Vertices.Add(FVector(scale.X * rescale / 2, scale.Y * rescale/2, 0));
	Vertices.Add(FVector(scale.X * rescale / 2, scale.Y * rescale/2, scale.Z * rescale));

	Vertices.Add(FVector(-scale.X * rescale / 2, scale.Y * rescale/2, 0));
	Vertices.Add(FVector(-scale.X * rescale / 2, scale.Y * rescale/2, scale.Z * rescale));


	Face oneFace = { 2,4,0,  0,  FVector2D(scale.X,0), FVector2D(scale.X, scale.Y), FVector2D(0, 0) };
	Faces.Add(oneFace);
	oneFace = { 4,6,0,  0,  FVector2D(scale.X,scale.Y), FVector2D(0, scale.Y), FVector2D(0, 0) };
	Faces.Add(oneFace);

	oneFace = { 1,5,3,  0,  FVector2D(0,0), FVector2D(scale.X, scale.Y), FVector2D(scale.X, 0) };
	Faces.Add(oneFace);
	oneFace = { 1,7,5,  0,  FVector2D(0,0), FVector2D(0, scale.Y), FVector2D(scale.X, scale.Y) };
	Faces.Add(oneFace);

	int numberOfVertices = Vertices.Num();

	for (int i = 0; i < numberOfVertices / 2; i++) {
		float dist = FVector::Distance(Vertices[i * 2 + 1], Vertices[((i * 2 + 3) % (numberOfVertices))]) / 100;
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(0,0), FVector2D(0,  scale.Z), FVector2D(dist,  scale.Z) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist, scale.Z), FVector2D(dist, 0), FVector2D(0, 0) };
		Faces.Add(oneFace);
	}

	return PlaceStaticMesh(CreateMesh(FString("RightCuboid" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	), Vertices, Faces, Vertices.Num()));


}

AActor* Operation::CreatePyramid()
{
	FTransform objectTrasform(FRotator(0, 0, 0), pos, FVector(1, 1, 1));
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
	FTransform objectTrasform(FRotator(0, 0, 0), pos, FVector(1, 1, 1));
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

AActor* Operation::CreatePyramidFrustumWall()
{
	TArray<FVector> Vertices;
	TArray<Face> Faces;

	for (int i = 0; i < top.Num(); i++) {
		Vertices.Add(base[i]);
		Vertices.Add(top[i]);
	}

	float x = FVector::Distance(Vertices[0], Vertices[2])/100;
	float y = FVector::Distance(Vertices[2], Vertices[4])/100;

	Face oneFace = { 2,4,0,  0,  FVector2D(x,0), FVector2D(x, y), FVector2D(0, 0) };
	Faces.Add(oneFace);
	oneFace = { 4,6,0,  0,  FVector2D(x,y), FVector2D(0, y), FVector2D(0, 0) };
	Faces.Add(oneFace);

	oneFace = { 1,5,3,  0,  FVector2D(0,0), FVector2D(x, y), FVector2D(x, 0) };
	Faces.Add(oneFace);
	oneFace = { 1,7,5,  0,  FVector2D(0,0), FVector2D(0, y), FVector2D(x, y) };
	Faces.Add(oneFace);

	int numberOfVertices = Vertices.Num();
	float z = FVector::Distance(Vertices[0], Vertices[1]) / 100;
	for (int i = 0; i < numberOfVertices / 2; i++) {
		float dist = FVector::Distance(Vertices[i * 2 + 1], Vertices[((i * 2 + 3) % (numberOfVertices))]) / 100;
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(0,0), FVector2D(z,  0), FVector2D(z,  dist) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(z, dist), FVector2D(0, dist), FVector2D(0, 0) };
		Faces.Add(oneFace);
	}


	return PlaceStaticMesh(CreateMesh(FString("PyramidFrustum"
	), Vertices, Faces, Vertices.Num()));
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

	cap1.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		
		p.TextureU = FVector(0, 1, 0);

		p.TextureV = FVector(1, 0, 0);
		NewBrush->Brush->Polys->Element.Add(p);
	}
	triangles.Empty();
	cap2.Triangulate(NewBrush, triangles);
	for (FPoly p : triangles) {
		p.TextureU = FVector(0, -1, 0);
		p.TextureV = FVector(-1, 0, 0);
		NewBrush->Brush->Polys->Element.Add(p);
	}




	//Optimize 
   //FPoly::OptimizeIntoConvexPolys(NewBrush, NewBrush->Brush->Polys->Element);
   
	GEditor->RebuildAlteredBSP();
	return NewBrush;

}

TArray<int> convertIndices(TArray<FVector> verts, TPPLPolyList polys, FVector delta) {
	TArray<int> result;
	std::list<TPPLPoly>::iterator iter;
	for (iter = polys.begin(); iter != polys.end(); iter++) {
		result.Add(verts.Find(FVector(iter->GetPoint(0).x, iter->GetPoint(0).y, 0) + delta));
		result.Add(verts.Find(FVector(iter->GetPoint(1).x, iter->GetPoint(1).y, 0) + delta));
		result.Add(verts.Find(FVector(iter->GetPoint(2).x, iter->GetPoint(2).y, 0) + delta));
	}
	return result;
}

FVector PolyNormal(TArray<FVector> pts) {
	FVector pt = pts[0];
	FVector sum = FVector::ZeroVector;
	for (int i = 1; i < pts.Num() - 1; i++) {
		if (pts[i] == pt || pts[i + 1] == pt) continue;
		sum += FVector::CrossProduct(pts[i] - pt, pts[i + 1] - pt);
	}
	sum.Normalize();
	return sum;
}

FVector GetUV(FVector v, FVector m[3]) {
	return FVector(v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z, v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z, 0);
}

AActor* Operation::CreateSlab()
{
	FVector n = PolyNormal(base);
	TArray<FVector> Vertices;
	Face oneFace;
	TArray<Face> Faces;
	if (n.Z > 1) {
		for (int j = 0; j < base.Num(); j++) {
			FVector v = base[j];
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(v.X + i, v.Y, v.Z + i * height));
			}
		}
	}
	else {
		for (int j = 0; j < base.Num(); j++) {
			FVector v = base[j];
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(v.X + i, v.Y, v.Z + i * height));
			}
		}
	}


	if (holes.Num() == 0) {

		FClipSMPolygon Polygon(0);

		for (int32 v = 0; v < Vertices.Num(); v = v+2)
		{
			FClipSMVertex vtx;
			vtx.Pos = Vertices[v];

			// Init other data so that VertsAreEqual won't compare garbage
			vtx.TangentX = FVector::ZeroVector;
			vtx.TangentY = FVector::ZeroVector;
			vtx.TangentZ = FVector::ZeroVector;
			vtx.Color = FColor(0, 0, 0);
			for (int32 uvIndex = 0; uvIndex < UE_ARRAY_COUNT(vtx.UVs); ++uvIndex)
			{
				vtx.UVs[uvIndex] = FVector2D(0.f, 0.f);
			}


			Polygon.Vertices.Add(vtx);
		}
	
		TArray<FClipSMTriangle> Triangles;

		int numberOfVertices = Vertices.Num();
		double dist = 0;
		for (int i = 0; i < numberOfVertices / 2; i++) {
			double old = dist;
			dist = dist + (FVector::Distance(Vertices[i * 2], Vertices[(i * 2 + 2) % (numberOfVertices)]) * 0.01f);
			oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(old ,0), FVector2D(old , height / 100.0f), FVector2D(dist , height / 100.0f) };
			Faces.Add(oneFace);
			oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist , height / 100.0f), FVector2D(dist , 0), FVector2D(old, 0) };
			Faces.Add(oneFace);
		}

		if (FGeomTools::TriangulatePoly(Triangles, Polygon)) {
			FVector matrix[3];
			matrix[2] = FVector::CrossProduct(Vertices[3] - Vertices[1], Vertices[5] - Vertices[1]).GetSafeNormal();
			matrix[0] = (Vertices[3] - Vertices[1]).GetSafeNormal();
			matrix[1] = FVector::CrossProduct(matrix[2], matrix[0]).GetSafeNormal();
			for (int32 p = 0; p < Triangles.Num(); ++p)
			{
				FClipSMTriangle* tri = &(Triangles[p]);
				int ponto1 = Vertices.Find(tri->Vertices[0].Pos);
				int ponto2 = Vertices.Find(tri->Vertices[1].Pos);
				int ponto3 = Vertices.Find(tri->Vertices[2].Pos);
				FVector uv1 = GetUV(Vertices[ponto1] - Vertices[0],matrix);
				FVector uv2 = GetUV(Vertices[ponto2] - Vertices[0],matrix);
				FVector uv3 = GetUV(Vertices[ponto3] - Vertices[0],matrix);

				oneFace = { ponto1,ponto2,ponto3,  0,  FVector2D(uv1.X/100,uv1.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv3.X / 100,uv3.Y / 100) };
				Faces.Add(oneFace);
				oneFace = { ponto3 + 1 ,ponto2+1,ponto1 + 1,  0, FVector2D(uv3.X / 100,uv3.Y / 100) ,FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv1.X / 100,uv1.Y / 100) };
				Faces.Add(oneFace);


			}


		}

		return PlaceStaticMesh(CreateMesh(FString("Slab" 
		), Vertices, Faces, Vertices.Num()));

	}
	else {
		TArray<FVector> vertices2;
		TPPLPoly poly;
		TPPLPartition pp;
		TPPLPolyList polys;
		TPPLPolyList trigs;
		FVector matrix[3];

		int numberOfVertices = Vertices.Num();
		poly.Init(Vertices.Num() / 2);
		for (int i = 0; i < Vertices.Num(); i = i + 2) {
		
			poly[i / 2].x = Vertices[i].X;
			poly[i / 2].y = Vertices[i].Y;
		}

		polys.push_back(poly);

		double dist = 0;
		for (int i = 0; i < numberOfVertices / 2; i++) {
			double old = dist;
			dist = dist + (FVector::Distance(Vertices[i * 2], Vertices[(i * 2 + 2) % (numberOfVertices)]) * 0.01f);
			oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(old ,0), FVector2D(old , height / 100.0f), FVector2D(dist , height / 100.0f) };
			Faces.Add(oneFace);
			oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist , height / 100.0f), FVector2D(dist , 0), FVector2D(old, 0) };
			Faces.Add(oneFace);
		}


		for (TArray<FVector> h : holes) {
			dist = 0;
			TPPLPoly ph;
			ph.SetHole(true);
			ph.Init(h.Num());
			for (int j = h.Num()-1; j >= 0; j--) {
				ph[h.Num() - 1 -j].x = h[j].X;
				ph[h.Num() - 1 - j].y = h[j].Y;
				for (int i = 0; i < 2; i = i + 1) {
					Vertices.Add(FVector(h[j].X + i, h[j].Y, h[j].Z + i * height));
				}


			}

			for (int i = 0; i < h.Num(); i++) {
				double old = dist;
				dist = dist + (FVector::Distance(Vertices[i * 2 + numberOfVertices], Vertices[((i * 2 + 2) % (h.Num()*2)) + numberOfVertices]) * 0.01f);
				oneFace = { i * 2 + numberOfVertices,   ((i * 2 + 1) % (h.Num()*2)) + numberOfVertices,   ((i * 2 + 3) % (h.Num()*2)) + numberOfVertices,  0,  FVector2D(old ,0), FVector2D(old , height / 100), FVector2D(dist , height / 100) };
				Faces.Add(oneFace);
				oneFace = { ((i * 2 + 3) % (h.Num()*2)) + numberOfVertices, ((i * 2 + 2) % (h.Num()*2)) + numberOfVertices,   i * 2 + numberOfVertices,  0,  FVector2D(dist , height / 100), FVector2D(dist , 0), FVector2D(old, 0) };
				Faces.Add(oneFace);
			}
			numberOfVertices = Vertices.Num();
			polys.push_back(ph);

		}

		matrix[2] = FVector::CrossProduct(Vertices[3] - Vertices[1], Vertices[5] - Vertices[1]).GetSafeNormal();
		matrix[0] = (Vertices[3] - Vertices[1]).GetSafeNormal();
		matrix[1] = FVector::CrossProduct(matrix[2], matrix[0]).GetSafeNormal();

		if (!pp.Triangulate_MONO(&polys, &trigs))
			UE_LOG(LogTemp, Log, TEXT("ERROR!!!"));
		TArray<int> result = convertIndices(Vertices, trigs, FVector(0, 0, Vertices[0].Z));

		

		
		
		FVector v0 = GetUV(Vertices[1], matrix);
		for (int32 i = 0; i < result.Num() / 3; i++) {
			FVector uv1 = GetUV(Vertices[result[i * 3] + 1] - Vertices[1], matrix);
			FVector uv2 = GetUV(Vertices[result[i * 3 + 1] + 1] - Vertices[1], matrix);
			FVector uv3 = GetUV(Vertices[result[i * 3 + 2]  + 1] - Vertices[1], matrix);
			oneFace = { result[i * 3 + 2] + 1 , result[i * 3 + 1] + 1 , result[i * 3] + 1 ,  0,  FVector2D(uv3.X / 100,uv3.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv1.X / 100,uv1.Y / 100) };
			Faces.Add(oneFace);
			oneFace = { result[i * 3]   ,  result[i * 3 + 1] ,  result[i * 3 + 2]    ,  0, FVector2D(uv1.X / 100,uv1.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100),  FVector2D(uv3.X / 100,uv3.Y / 100) };
			Faces.Add(oneFace);
		}
		result.Empty();
		return PlaceStaticMesh(CreateMesh(FString("Slab"
		), Vertices, Faces, Vertices.Num()));
	}
	/*
	TArray<AActor*> bs;
	bs.Add(spawnCustom(FVector(0,0,0), FRotator(0,0,0), height, base, base.Num(), true));

	for (int i = 0; i < holes.Num(); i++) {
		bs.Add(spawnCustom(FVector(0, 0, 0), FRotator(0, 0, 0), height, holes[i], holes[i].Num(), false));
	}

	AStaticMeshActor* realActor = NULL;
	
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	return realActor;
	*/
}

AActor* Operation::CreatePanel()
{
	/*
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
  // FPoly::OptimizeIntoConvexPolys(NewBrush, NewBrush->Brush->Polys->Element);

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
	*/

	FVector n = PolyNormal(base);


	TArray<FVector> Vertices;
	TArray<Face> Faces;
	if (n.Z > 1) {
		for (int j = 0; j < base.Num(); j++) {
			FVector v = base[j];
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(v.X + i, v.Y, v.Z + i * height));
			}
		}
	}
	else {
		n = -n;
		for (int j = base.Num() - 1; j >= 0; j--) {
			FVector v = base[j];
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(v.X + i, v.Y, v.Z + i * height));
			}
		}
	}

	FClipSMPolygon Polygon(0);

	for (int32 v = 0; v < Vertices.Num(); v = v + 2)
	{
		FClipSMVertex vtx;
		vtx.Pos = Vertices[v];

		// Init other data so that VertsAreEqual won't compare garbage
		vtx.TangentX = FVector::ZeroVector;
		vtx.TangentY = FVector::ZeroVector;
		vtx.TangentZ = FVector::ZeroVector;
		vtx.Color = FColor(0, 0, 0);
		for (int32 uvIndex = 0; uvIndex < UE_ARRAY_COUNT(vtx.UVs); ++uvIndex)
		{
			vtx.UVs[uvIndex] = FVector2D(0.f, 0.f);
		}


		Polygon.Vertices.Add(vtx);
	}
	Face oneFace;
	TArray<FClipSMTriangle> Triangles;

	int numberOfVertices = Vertices.Num();
	double dist = 0;
	FVector matrix[3];
	matrix[1] = n;
	for (int i = 0; i < numberOfVertices / 2; i++) {
		matrix[2] = FVector::CrossProduct(Vertices[i * 2 + 1] - Vertices[i * 2], Vertices[((i * 2 + 3) % (numberOfVertices))] - Vertices[i * 2]).GetSafeNormal();
		matrix[0] = FVector::CrossProduct(matrix[2], matrix[1]).GetSafeNormal();
		dist = dist + FVector::Distance(Vertices[i * 2], Vertices[(i * 2 + 2) % (numberOfVertices)]) / 100;
		FVector delta = GetUV(Vertices[i * 2 + 1] - Vertices[i * 2], matrix);
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(dist * i,0), FVector2D(dist * i + delta.X * height,  delta.Y * height / 100), FVector2D(dist * (i + 1) + delta.X * height, delta.Y * height / 100) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist * (i + 1) + delta.X * height, delta.Y * height / 100), FVector2D(dist * (i + 1), 0), FVector2D(dist * i, 0) };
		Faces.Add(oneFace);
	}

	if (FGeomTools::TriangulatePoly(Triangles, Polygon)) {
		
		matrix[2] = FVector::CrossProduct(Vertices[3] - Vertices[1], Vertices[5] - Vertices[1]).GetSafeNormal();
		matrix[0] = (Vertices[3] - Vertices[1]).GetSafeNormal();
		matrix[1] = FVector::CrossProduct(matrix[2], matrix[0]).GetSafeNormal();
		for (int32 p = 0; p < Triangles.Num(); ++p)
		{
			FClipSMTriangle* tri = &(Triangles[p]);
			int ponto1 = Vertices.Find(tri->Vertices[0].Pos);
			int ponto2 = Vertices.Find(tri->Vertices[1].Pos);
			int ponto3 = Vertices.Find(tri->Vertices[2].Pos);
			FVector uv1 = GetUV(Vertices[ponto1] - Vertices[0], matrix);
			FVector uv2 = GetUV(Vertices[ponto2] - Vertices[0], matrix);
			FVector uv3 = GetUV(Vertices[ponto3] - Vertices[0], matrix);

			oneFace = { ponto1,ponto2,ponto3,  0,  FVector2D(uv1.X / 100,uv1.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv3.X / 100,uv3.Y / 100) };
			Faces.Add(oneFace);
			oneFace = { ponto3 + 1 ,ponto2 + 1,ponto1 + 1,  0, FVector2D(uv3.X / 100,uv3.Y / 100) ,FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv1.X / 100,uv1.Y / 100) };
			Faces.Add(oneFace);


		}


	}

	return PlaceStaticMesh(CreateMesh(FString("Panel"
	), Vertices, Faces, Vertices.Num()));
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
	realActor->SetActorRotation(rot);
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	
	
	

	return realActor;
}

UMaterialInterface* Operation::LoadMaterial()
{
	return LoadObject<UMaterialInterface>(nullptr, *path);
}

UStaticMesh* Operation::LoadResources()
{


	return LoadObject<UStaticMesh>(nullptr, *path);
}





AActor* convertMeshtoBrush(UStaticMesh* m, FRotator rot,FVector pos, bool addictive) {
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

AActor* Operation::CreateSubtration()
{
	TArray<AActor*> brushes;
	AStaticMeshActor* ac = Cast<AStaticMeshActor>(selectedActors[0]);
	brushes.Add(convertMeshtoBrush(ac->GetStaticMeshComponent()->GetStaticMesh(),ac->GetActorRotation(), ac->GetActorLocation(), true));
	for (int i = 1; i < selectedActors.Num(); i++) {
		AStaticMeshActor* ac = Cast<AStaticMeshActor>(selectedActors[i]);
		brushes.Add(convertMeshtoBrush(ac->GetStaticMeshComponent()->GetStaticMesh(), ac->GetActorRotation(), ac->GetActorLocation(), false));
	}
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(brushes, FString("/Game/Subtraction" +
		FString::SanitizeFloat(count_Pyramid++)
	));
	
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);


	return realActor;
}

AActor* Operation::CreateAddition()
{
	TArray<AActor*> brushes;
	for (int i = 0; i < selectedActors.Num(); i++) {
		AStaticMeshActor* ac = Cast<AStaticMeshActor>(selectedActors[i]);
		brushes.Add(convertMeshtoBrush(ac->GetStaticMeshComponent()->GetStaticMesh(), ac->GetActorRotation(), ac->GetActorLocation(), false));
	}
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(brushes, FString("/Game/Addition" +
		FString::SanitizeFloat(count_Pyramid++)
	));

	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);

	return realActor;
}

AActor* Operation::CreatePointLight() {
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(FRotator(0,0,0), pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = APointLight::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	APointLight* realActor = Cast<APointLight>(newActorCreated);
	realActor->SetActorLabel("PointLight");
	realActor->PointLightComponent->Intensity = height;
	realActor->PointLightComponent->SetAttenuationRadius(radius*rescale);
	realActor->PointLightComponent->SetLightColor(color);
	realActor->SetActorScale3D(objectScale);
	realActor->SetActorRotation(FRotator(0, 0, 0));
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);





	return realActor;
}

UStaticMesh* Operation::CreateMesh(FString name, TArray<FVector> Vertices, TArray<Face> Faces, int size)
{
	// Object Details
	FString ObjectName = name;
	int numberOfVertices = size;

	ObjectName = ObjectName.Replace(TEXT("."), TEXT("a"));
	ObjectName = ObjectName.Replace(TEXT(":"), TEXT("b"));

	Face oneFace;
	//TODO: Varie UV based on size


	/*

	for cube and box
	oneFace = { 2,4,0,  0,  FVector2D(1,0), FVector2D(1, 1), FVector2D(0, 0) };
	Faces.Add(oneFace);
	oneFace = { 4,6,0,  0,  FVector2D(1,1), FVector2D(0, 1), FVector2D(0, 0) };
	Faces.Add(oneFace);

	oneFace = { 1,5,3,  0,  FVector2D(0,0), FVector2D(1, 1), FVector2D(1, 0) };
	Faces.Add(oneFace);
	oneFace = { 1,7,5,  0,  FVector2D(0,0), FVector2D(0, 1), FVector2D(1, 1) };
	Faces.Add(oneFace);


	*/



	int numberOfFaces = Faces.Num();

	TArray<FStaticMaterial> Materials;
	Materials.Add(FStaticMaterial());
	int numberOfMaterials = Materials.Num();

	// Create Package
	FString path = FString("/Game/MyStaticMeshes/") + ObjectName;
	UPackage* Package = CreatePackage(nullptr, *path);
	Package->FullyLoad();

	FName StaticMeshName;
	// Create Static Mesh
	if (op == Slab || op == Panel|| op == PyramidFrustumWall)	 {
		 StaticMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), FName(*ObjectName));
	}
	else {
		 StaticMeshName = FName(*ObjectName);
	}
	UStaticMesh* myStaticMesh = NewObject<UStaticMesh>(Package, StaticMeshName, RF_Public | RF_Standalone);

	if (myStaticMesh != NULL)
	{
		FRawMesh myRawMesh = FRawMesh();
		FColor WhiteVertex = FColor(255, 255, 255, 255);
		FVector EmptyVector = FVector(0, 0, 0);

		// Vertices
		for (int vertIndex = 0; vertIndex < numberOfVertices; vertIndex++) {
			myRawMesh.VertexPositions.Add(Vertices[vertIndex]);
		}
		// Faces and UV/Normals
		for (int faceIndex = 0; faceIndex < numberOfFaces; faceIndex++) {
			myRawMesh.WedgeIndices.Add(Faces[faceIndex].v1);
			myRawMesh.WedgeIndices.Add(Faces[faceIndex].v2);
			myRawMesh.WedgeIndices.Add(Faces[faceIndex].v3);

			myRawMesh.WedgeColors.Add(WhiteVertex);
			myRawMesh.WedgeColors.Add(WhiteVertex);
			myRawMesh.WedgeColors.Add(WhiteVertex);

			myRawMesh.WedgeTangentX.Add(Faces[faceIndex].UDirection1);
			myRawMesh.WedgeTangentX.Add(Faces[faceIndex].UDirection2);
			myRawMesh.WedgeTangentX.Add(Faces[faceIndex].UDirection3);

			myRawMesh.WedgeTangentY.Add(Faces[faceIndex].VDirection);
			myRawMesh.WedgeTangentY.Add(Faces[faceIndex].VDirection);
			myRawMesh.WedgeTangentY.Add(Faces[faceIndex].VDirection);
			FVector cp = FVector::CrossProduct(Vertices[Faces[faceIndex].v3] - Vertices[Faces[faceIndex].v1], Vertices[Faces[faceIndex].v2] - Vertices[Faces[faceIndex].v1]);
			myRawMesh.WedgeTangentZ.Add(cp);
			myRawMesh.WedgeTangentZ.Add(cp);
			myRawMesh.WedgeTangentZ.Add(cp);

			// Materials
			myRawMesh.FaceMaterialIndices.Add(Faces[faceIndex].materialID);

			myRawMesh.FaceSmoothingMasks.Add(0xFFFFFFFF); // Phong

			for (int UVIndex = 0; UVIndex < MAX_MESH_TEXTURE_COORDS; UVIndex++)
			{
				myRawMesh.WedgeTexCoords[UVIndex].Add(Faces[faceIndex].uvCoords1);
				myRawMesh.WedgeTexCoords[UVIndex].Add(Faces[faceIndex].uvCoords2);
				myRawMesh.WedgeTexCoords[UVIndex].Add(Faces[faceIndex].uvCoords3);
			}
		}

		
		// Saving mesh in the StaticMesh
		new(myStaticMesh->SourceModels) FStaticMeshSourceModel();
		myStaticMesh->SourceModels[0].RawMeshBulkData->SaveRawMesh(myRawMesh);

		FStaticMeshSourceModel& SrcModel = myStaticMesh->SourceModels[0];

		// Model Configuration
		myStaticMesh->SourceModels[0].BuildSettings.bRecomputeNormals = false;
		myStaticMesh->SourceModels[0].BuildSettings.bRecomputeTangents = false;
		myStaticMesh->SourceModels[0].BuildSettings.bUseMikkTSpace = false;
		myStaticMesh->SourceModels[0].BuildSettings.bGenerateLightmapUVs = true;
		myStaticMesh->SourceModels[0].BuildSettings.bBuildAdjacencyBuffer = true;
		myStaticMesh->SourceModels[0].BuildSettings.bBuildReversedIndexBuffer = false;
		myStaticMesh->SourceModels[0].BuildSettings.bUseFullPrecisionUVs = false;
		myStaticMesh->SourceModels[0].BuildSettings.bUseHighPrecisionTangentBasis = false;
	
		

		// Assign the Materials to the Slots (optional

		for (int32 MaterialID = 0; MaterialID < numberOfMaterials; MaterialID++) {
			myStaticMesh->StaticMaterials.Add(Materials[MaterialID]);
			myStaticMesh->SectionInfoMap.Set(0, MaterialID, FMeshSectionInfo(MaterialID));
		}

		// Processing the StaticMesh and Marking it as not saved
		myStaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
		myStaticMesh->CreateBodySetup();
		myStaticMesh->SetLightingGuid();
		myStaticMesh->PostEditChange();
		Package->MarkPackageDirty();
		UE_LOG(LogTemp, Log, TEXT("Static Mesh created: %s"), &ObjectName);
		return myStaticMesh;
	}
	return NULL;
}


ULevelSequence* CreateLevelSeq(const FString& AssetName, const FString& PackagePath, UObject* AssetToDuplicate) {
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UObject* NewAsset = nullptr;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* CurrentClass = *It;
		if (CurrentClass->IsChildOf(UFactory::StaticClass()) && !(CurrentClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UFactory* Factory = Cast<UFactory>(CurrentClass->GetDefaultObject());
			if (Factory->CanCreateNew() && Factory->ImportPriority >= 0 && Factory->SupportedClass == ULevelSequence::StaticClass())
			{
				if (AssetToDuplicate != nullptr)
				{
					NewAsset = AssetTools.DuplicateAsset(AssetName, PackagePath, AssetToDuplicate);
				}
				else
				{
					NewAsset = AssetTools.CreateAsset(AssetName, PackagePath, ULevelSequence::StaticClass(), Factory);
				}
				break;
			}
		}
	}
	return Cast<ULevelSequence>(NewAsset);
}



AActor* Operation::CreateUpdateCamera()
{ 
	nextFramerotation = rot;
	nextFramePosition = pos;
	nextFrameCamera[0] = radius;
	GCurrentLevelEditingViewportClient->SetViewLocation(pos);
	GCurrentLevelEditingViewportClient->SetLookAtLocation(scale);

	return NULL;
}

AActor* Operation::CreateRender()
{
	ALevelSequenceActor* realActor = NULL;
	if (current == NULL || name != filmName) {
		filmName = name;
		FTransform objectTrasform(FRotator(0,0,0), nextFramePosition, FVector(1,1,1));
		UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
		ULevel* currentLevel = currentWorld->GetCurrentLevel();
		UClass* staticMeshClass = ALevelSequenceActor::StaticClass();
		AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

		realActor = Cast<ALevelSequenceActor>(newActorCreated);
		realActor->SetActorLabel("LevelSequence");
		auto original = LoadObject<ULevelSequence>(nullptr, *FString("/Game/MyStaticMeshes/LevelSequence/ExampleSequence.ExampleSequence"));
		current = CreateLevelSeq(filmName, FString("/Game/MyStaticMeshes/LevelSequence"), original);
		realActor->SetSequence(current);
		TArray<FMovieSceneBinding> binds = current->GetMovieScene()->GetBindings();

		auto transformTrack = Cast<UMovieScene3DTransformTrack>(binds[0].GetTracks()[1]);
		posX = transformTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];
		posY = transformTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[1];
		posZ = transformTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[2];

		rotX = transformTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[3];
		rotY = transformTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[4];
		rotZ = transformTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[5];


		auto focusTrack = Cast<UMovieSceneFloatTrack>(binds[1].GetTracks()[0]);
		focus = focusTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];
		auto focalTrack = Cast<UMovieSceneFloatTrack>(binds[1].GetTracks()[1]);
		focal = focalTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];
		auto apertureTrack = Cast<UMovieSceneFloatTrack>(binds[1].GetTracks()[2]);
		aperture = apertureTrack->GetSectionToKey()->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];

	}
	current->GetMovieScene()->SetSelectionRange(TRange<FFrameNumber>(FFrameNumber(param[3])));
	posX->AddConstantKey(FFrameNumber(param[3]), nextFramePosition.X);
	posY->AddConstantKey(FFrameNumber(param[3]), nextFramePosition.Y);
	posZ->AddConstantKey(FFrameNumber(param[3]), nextFramePosition.Z);

	rotX->AddConstantKey(FFrameNumber(param[3]), nextFramerotation.Euler().X);
	rotY->AddConstantKey(FFrameNumber(param[3]), nextFramerotation.Euler().Y);
	rotZ->AddConstantKey(FFrameNumber(param[3]), nextFramerotation.Euler().Z);

	focus->AddConstantKey(FFrameNumber(param[3]), nextFrameCamera[0]);


	MovieSceneCapture = NewObject<UAutomatedLevelSequenceCapture>(GetTransientPackage(), UAutomatedLevelSequenceCapture::StaticClass(), FName("MovieCapture"), RF_Transient);
	MovieSceneCapture->ImageCaptureProtocolType = UImageSequenceProtocol_PNG::StaticClass();
	UImageSequenceProtocol_PNG* ImageCaptureProtocol = NewObject<UImageSequenceProtocol_PNG>(MovieSceneCapture, UImageSequenceProtocol_PNG::StaticClass(), FName("UUserDefinedImageCaptureProtocol"));
	MovieSceneCapture->ImageCaptureProtocol = ImageCaptureProtocol;
	MovieSceneCapture->LoadFromConfig();

	MovieSceneCapture->LevelSequenceAsset = current->GetMovieScene()->GetOuter()->GetPathName();
	ULevelEditorPlaySettings* PlayInEditorSettings = GetMutableDefault<ULevelEditorPlaySettings>();


	const FMovieSceneCaptureSettings& Settings = MovieSceneCapture->GetSettings();

	PlayInEditorSettings->NewWindowWidth = Settings.Resolution.ResX;
	PlayInEditorSettings->NewWindowHeight = Settings.Resolution.ResY;
	PlayInEditorSettings->CenterNewWindow = true;
	PlayInEditorSettings->LastExecutedPlayModeType = EPlayModeType::PlayMode_InEditorFloating;

	TSharedRef<SWindow> CustomWindow = SNew(SWindow)
		.Title(LOCTEXT("MovieRenderPreviewTitle", "Movie Render - Preview"))
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.UseOSWindowBorder(true)
		.FocusWhenFirstShown(false)
		.ActivationPolicy(EWindowActivationPolicy::Never)
		.HasCloseButton(true)
		.SupportsMaximize(false)
		.SupportsMinimize(true)
		.MaxWidth(Settings.Resolution.ResX)
		.MaxHeight(Settings.Resolution.ResY)
		.SizingRule(ESizingRule::FixedSize);

	FSlateApplication::Get().AddWindow(CustomWindow);

	PlayInEditorSettings->CustomPIEWindow = CustomWindow;

	// Reset everything else
	PlayInEditorSettings->GameGetsMouseControl = false;
	PlayInEditorSettings->ShowMouseControlLabel = false;
	PlayInEditorSettings->ViewportGetsHMDControl = false;
	PlayInEditorSettings->ShouldMinimizeEditorOnVRPIE = true;
	PlayInEditorSettings->EnableGameSound = MovieSceneCapture->AudioCaptureProtocolType != UNullAudioCaptureProtocol::StaticClass();
	PlayInEditorSettings->bOnlyLoadVisibleLevelsInPIE = false;
	PlayInEditorSettings->bPreferToStreamLevelsInPIE = false;
	PlayInEditorSettings->PIEAlwaysOnTop = false;
	PlayInEditorSettings->DisableStandaloneSound = false;
	PlayInEditorSettings->AdditionalLaunchParameters = TEXT("");
	PlayInEditorSettings->BuildGameBeforeLaunch = EPlayOnBuildMode::PlayOnBuild_Never;
	PlayInEditorSettings->LaunchConfiguration = EPlayOnLaunchConfiguration::LaunchConfig_Default;
	PlayInEditorSettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
	PlayInEditorSettings->SetRunUnderOneProcess(true);
	PlayInEditorSettings->SetPlayNetDedicated(false);
	PlayInEditorSettings->SetPlayNumberOfClients(1);


	FFrameRate DisplayRate = FFrameRate();
	MovieSceneCapture->Settings.bUseCustomFrameRate = true;

	MovieSceneCapture->Settings.FrameRate = DisplayRate;
	MovieSceneCapture->Settings.ZeroPadFrameNumbers = 0;
	MovieSceneCapture->Settings.bUseRelativeFrameNumbers = false;

	FFrameNumber StartFrame = FFrameNumber(0);
	FFrameNumber EndFrame = FFrameNumber(1);
	MovieSceneCapture->SetFrameOverrides(StartFrame, EndFrame);

	MovieSceneCapture->AddToRoot();
	MovieSceneCapture->OnCaptureFinished().AddRaw(this, &Operation::OnLevelSequenceFinished);
	MovieSceneCapture->Settings.Resolution = FCaptureResolution(param[0], param[1]);


	UGameViewportClient::OnViewportCreated().AddRaw(this, &Operation::OnPIEViewportStarted);




	GEditor->RequestPlaySession(true, nullptr, false);



	return realActor;
}

void Operation::OnLevelSequenceFinished() {


	GEditor->RequestEndPlayMap();

	FEditorDelegates::EndPIE.RemoveAll(this);
	UGameViewportClient::OnViewportCreated().RemoveAll(this);
	MovieSceneCapture->OnCaptureFinished().RemoveAll(this);

	MovieSceneCapture->Close();
	MovieSceneCapture->RemoveFromRoot();
}

void Operation::OnPIEViewportStarted() {
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.WorldType == EWorldType::PIE)
		{

			FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
			if (SlatePlayInEditorSession)
			{
				UWorld* CapturingFromWorld = Context.World();

				TSharedPtr<SWindow> Window = SlatePlayInEditorSession->SlatePlayInEditorWindow.Pin();

				const FMovieSceneCaptureSettings& Settings = MovieSceneCapture->GetSettings();


				SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.Get()->SetViewportSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

				FVector2D PreviewWindowSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

				// Keep scaling down the window size while we're bigger than half the desktop width/height
				{
					FDisplayMetrics DisplayMetrics;
					FSlateApplication::Get().GetCachedDisplayMetrics(DisplayMetrics);

					while (PreviewWindowSize.X >= DisplayMetrics.PrimaryDisplayWidth * .5f || PreviewWindowSize.Y >= DisplayMetrics.PrimaryDisplayHeight * .5f)
					{
						PreviewWindowSize *= .5f;
					}
				}

				// Resize and move the window into the desktop a bit
				FVector2D PreviewWindowPosition(50, 50);
				Window->ReshapeWindow(PreviewWindowPosition, PreviewWindowSize);

				if (MovieSceneCapture->Settings.GameModeOverride != nullptr)
				{

					CapturingFromWorld->GetWorldSettings()->DefaultGameMode = MovieSceneCapture->Settings.GameModeOverride;
				}

				auto CachedEngineShowFlags = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport->GetClient()->GetEngineShowFlags();
				if (CachedEngineShowFlags && Settings.bUsePathTracer)
				{
					auto CachedPathTracingMode = CachedEngineShowFlags->PathTracing;
					CachedEngineShowFlags->SetPathTracing(true);
				}
				MovieSceneCapture->Initialize(SlatePlayInEditorSession->SlatePlayInEditorWindowViewport, Context.PIEInstance);

			}

			FString CapturePath = MovieSceneCapture->ResolveFileFormat(MovieSceneCapture->Settings.OutputDirectory.Path, FFrameMetrics());
			UE_LOG(LogTemp, Warning, TEXT("%s"), *(CapturePath));
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Recieved PIE Creation callback but failed to find PIE World or missing FSlatePlayInEditorInfo for world."));
}

#undef LOCTEXT_NAMESPACE