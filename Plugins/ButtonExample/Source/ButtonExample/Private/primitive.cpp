#include "Primitive.h"
#include "UObject/UObjectGlobals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "EditorModeManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraActor.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaterialExpressionIO.h"
#include "Builders/CubeBuilder.h"
#include "Builders/CylinderBuilder.h"
#include "Builders/ConeBuilder.h"
#include "CustomBrushes/VertixBuilder.h"
#include "Engine/Polys.h"
#include "Builders/TetrahedronBuilder.h"
#include "Operations/BoxCreation.h"
#include "Operations/RightCuboidCreation.h"
#include "CustomBrushes/KhepriBox.h"
#include "CustomBrushes/KhepriCylinder.h"


int cube = 0, cylinder = 0, sphere = 0, object = 0;
int N_StaticMesh = 0;
TArray< AActor* > brushes;
int parent = -1;
int current_material = -1;
TQueue<Operation>* queue = new TQueue<Operation, EQueueMode::Spsc>();
TQueue<Response>* responsequeue = new TQueue<Response, EQueueMode::Spsc>();
//--------------------------------------------------------Placing Objects---------------------------------------------------------------------------//

TArray<AActor*> listActor;
TArray<UMaterial*> listMaterial;
TArray<UStaticMesh*> listMesh;

UStaticMesh* FindMesh(FString mesh) {

	mesh = mesh.Replace(TEXT("."), TEXT("a"));
	mesh = mesh.Replace(TEXT(":"), TEXT("b"));

	return LoadObject<UStaticMesh>(nullptr, *(mesh));

}


AActor* PlaceStaticMesh(UStaticMesh* mesh,FRotator rot, FVector pos)
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot, pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AStaticMeshActor::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	AStaticMeshActor* realActor = Cast<AStaticMeshActor>(newActorCreated);
	realActor->SetActorLabel("StaticMesh");
	realActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
	realActor->SetActorScale3D(objectScale);
	realActor->SetActorRotation(rot);
	realActor->SetActorLocation(pos);
	if (parent > -1)
		realActor->AttachToActor(listActor[parent], FAttachmentTransformRules::KeepRelativeTransform);
	if (current_material > -1)
		realActor->GetStaticMeshComponent()->SetMaterial(0, listMaterial[current_material]);





	return realActor;
}

FRotator MyLookRotation(FVector lookAt, FVector upDirection)
{
	FVector forward = lookAt;
	FVector up = upDirection.GetSafeNormal();


	///////////////////////





	FVector vector = forward.GetSafeNormal();
	FVector vector2 = FVector::CrossProduct(up, vector);
	FVector vector3 = FVector::CrossProduct(vector, vector2);
	float m00 = vector2.X;
	float m01 = vector2.Y;
	float m02 = vector2.Z;
	float m10 = vector3.X;
	float m11 = vector3.Y;
	float m12 = vector3.Z;
	float m20 = vector.X;
	float m21 = vector.Y;
	float m22 = vector.Z;


	float num8 = (m00 + m11) + m22;
	FQuat quaternion = FQuat();
	if (num8 > 0.0f)
	{
		float num = (float)FMath::Sqrt(num8 + 1.0f);
		quaternion.W = num * 0.5f;
		num = 0.5f / num;
		quaternion.X = (m12 - m21) * num;
		quaternion.Y = (m20 - m02) * num;
		quaternion.Z = (m01 - m10) * num;
		return FRotator(quaternion);
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)FMath::Sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion.X = 0.5f * num7;
		quaternion.Y = (m01 + m10) * num4;
		quaternion.Z = (m02 + m20) * num4;
		quaternion.W = (m12 - m21) * num4;
		return FRotator(quaternion);
	}
	if (m11 > m22)
	{
		float num6 = (float)FMath::Sqrt(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion.X = (m10 + m01) * num3;
		quaternion.Y = 0.5f * num6;
		quaternion.Z = (m21 + m12) * num3;
		quaternion.W = (m20 - m02) * num3;
		return FRotator(quaternion);
	}
	float num5 = (float)FMath::Sqrt(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion.X = (m20 + m02) * num2;
	quaternion.Y = (m21 + m12) * num2;
	quaternion.Z = 0.5f * num5;
	quaternion.W = (m01 - m10) * num2;


	return FRotator(quaternion);

}


void waitForRequest() {
	while (responsequeue->IsEmpty()) {
		FPlatformProcess::Sleep(0.001f);
	}
}




/*
  int Primitive::PlacingStaticMesh( char* label,char* myStaticMesh, FVector objectPosition, FRotator objectRotation, FVector objectScale, const char* mat ) {
	 PlaceMesh op = PlaceMesh(objectPosition,objectRotation,objectScale,mat, myStaticMesh,label);
	 queue->Enqueue(op);
	 waitForRequest();
	 queue->Dequeue(op);
	 AActor* newActor = op.getResponse();
	 return listActor.Add(newActor);

}*/




void execute(Operation op) {
	op.execute();
}

bool  Primitive::checkQueue(float delta, int SpF) {
	int num = 0;
	Operation fo;
	while (!(queue->IsEmpty())) {
		double start = FPlatformTime::Seconds();
		queue->Dequeue(fo);

		responsequeue->Enqueue(fo.execute());
		double end = FPlatformTime::Seconds();
		UE_LOG(LogTemp, Warning, TEXT("Operation completed in %f seconds."), end - start);
		num++;
		FPlatformProcess::Sleep(0.001f);
	}
	return true;
}


//Class'/Script/CinematicCamera.CineCameraActor'


//Boolean Operations




/*
int32 Primitive::spawnCube(FVector objectPosition, FRotator objectRotation, FVector objectScale) {
	CreateCube op = CreateCube(objectPosition, objectRotation, objectScale);
	queue->Enqueue(op);
	waitForRequest();
	queue->Dequeue(op);
	AActor* newActor = op.getResponse();
	return listActor.Add(newActor);
}

 int32 Primitive::spawnCube(FVector objectPosition, FRotator objectRotation, FVector objectScale,bool addictive ) {
	 CreateCube op = CreateCube(objectPosition, objectRotation, objectScale, addictive);
	 queue->Enqueue(op);
	 waitForRequest();
	 queue->Dequeue(op);
	 AActor* newActor = op.getResponse();
	 return listActor.Add(newActor);
}

  int32 Primitive::spawnCube(FVector objectPosition, FRotator objectRotation, FVector objectScale, bool addictive, bool hollow) {
	  CreateCube op = CreateCube(objectPosition, objectRotation, objectScale, addictive,hollow);
	  queue->Enqueue(op);
	  waitForRequest();
	  queue->Dequeue(op);
	  AActor* newActor = op.getResponse();
	  return listActor.Add(newActor);
  }


  */
void Primitive::spawnBrushCylinder(FVector objectPosition, FRotator objectRotation, float height, float radius, int slides, bool addictive) {
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UCylinderBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UCylinderBuilder* builder = (UCylinderBuilder*)NewBrush->BrushBuilder;
	builder->Z = 200 * height;
	builder->OuterRadius = radius;
	builder->Sides = slides;
	brushes.Add(NewBrush);
	GEditor->RebuildAlteredBSP();
}

void Primitive::spawnBrushCone(FVector objectPosition, FRotator objectRotation, float height, float radius, int slides, bool addictive)
{
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UConeBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UConeBuilder* builder = (UConeBuilder*)NewBrush->BrushBuilder;
	builder->Z = 200 * height;
	builder->OuterRadius = radius;
	builder->Sides = slides;
	builder->Hollow = true;
	brushes.Add(NewBrush);
	GEditor->RebuildAlteredBSP();
}

void Primitive::spawnBrushConeHollow(FVector objectPosition, FRotator objectRotation, float height, float in_height, float radius, float in_radius, int slides, bool addictive)
{
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UConeBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UConeBuilder* builder = (UConeBuilder*)NewBrush->BrushBuilder;
	builder->Z = 200 * height;
	builder->OuterRadius = radius;
	builder->CapZ = in_height;
	builder->InnerRadius = in_radius;
	builder->Sides = slides;
	builder->Hollow = true;
	brushes.Add(NewBrush);
	GEditor->RebuildAlteredBSP();
}

void Primitive::spawnBrushSphere(FVector objectPosition, FRotator objectRotation, float radius, int tesselation, bool addictive)
{
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UTetrahedronBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UTetrahedronBuilder* builder = (UTetrahedronBuilder*)NewBrush->BrushBuilder;
	builder->Radius = radius;
	builder->SphereExtrapolation = tesselation;
	brushes.Add(NewBrush);
	GEditor->RebuildAlteredBSP();
}

void Primitive::spawnBrushCylinderHollow(FVector objectPosition, FRotator objectRotation, float height, float radius, float in_radius, int slides, bool addictive) {
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UCylinderBuilder::StaticClass(), NAME_None, RF_Transactional);
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
	UCylinderBuilder* builder = (UCylinderBuilder*)NewBrush->BrushBuilder;
	builder->Z = 200 * height;
	builder->OuterRadius = radius;
	builder->InnerRadius = in_radius;
	builder->Sides = slides;
	builder->Hollow = true;
	brushes.Add(NewBrush);
	GEditor->RebuildAlteredBSP();
}


void Primitive::spawnCustom(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, int size, bool addictive) {
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
	brushes.Add(NewBrush);

}

AActor* Primitive::ConvertToStaticMesh(FString  name)
{
	GEditor->DoConvertActors(brushes, AStaticMeshActor::StaticClass(), TSet<FString>(), true, FString("/Game") / name);
	AActor* ac = brushes[0];
	brushes.Empty();
	return ac;
}

AActor* Primitive::ConvertToStaticMesh()
{

	char buffer[32];
	itoa(N_StaticMesh, buffer, 10);
	GEditor->DoConvertActors(brushes, AStaticMeshActor::StaticClass(), TSet<FString>(), true, FString("/Game/StaticMesh") + buffer);
	N_StaticMesh++;

	brushes.Empty();
	return brushes[0];

}


AActor* Primitive::ConvertToStaticMesh(const TArray<AActor*> bs, FString name) {
	
	name = name.Replace( TEXT(".") , TEXT("a"));
	name = name.Replace(TEXT(":"), TEXT("b"));
	GEditor->DoConvertActors(bs, AStaticMeshActor::StaticClass(), TSet<FString>(), true, name );
	TArray<AActor*> list = GEditor->GetEditorWorldContext().World()->GetCurrentLevel()->Actors;
	AActor* ac = list[list.Num()-1];
	return ac;
}

AActor* Primitive::ConvertToStaticMesh(const TArray<AActor*> bs)
{
	char buffer[32];
	itoa(N_StaticMesh, buffer, 10);
	GEditor->DoConvertActors(bs, AStaticMeshActor::StaticClass(), TSet<FString>(), true, FString("/Game/StaticMesh") + buffer);
	N_StaticMesh++;
	return bs[0];
}


	

void Primitive::cleanBrushes() {
	brushes.Empty();
}

AActor* Primitive::addBrushToConversion(ABrush* brush) {
	brushes.Add(brush);
	return brush;
}

TArray<AActor*> Primitive::getArray() {
	return  brushes;
}




int Primitive::Sphere(float x, float y, float z, float radius)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Sphere"));
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(FRotator(0,0,0), FVector(0,0,0), objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AActor::StaticClass();

	AActor* realActor = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);
	realActor->SetActorLabel("StaticMesh");
	realActor->SetActorScale3D(objectScale);
	return listActor.Add(realActor);
}

/*
int Primitive::StaticMesh(char* label, char* myStaticMesh, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, const char* mat)
{
	PlaceMesh op = PlaceMesh(FVector(px, py, pz), FRotator(rx, ry, rz), FVector(sx, sy, sz), mat, myStaticMesh, label);
	queue->Enqueue(op);
	waitForRequest();
	responsequeue->Dequeue(op);
	AActor* newActor = op.getResponse();
	return listActor.Add(newActor);
}
*/
int Primitive::Cylinder(FVector bot, float radius, FVector top)
{
	


	double start = FPlatformTime::Seconds();

	UStaticMesh* loaded_mesh = FindMesh(FString("/Game/Cylinder" +
		FString::SanitizeFloat(FVector::Distance(top,bot)) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(50)
	));
	if (loaded_mesh) {
		AActor* realActor = PlaceStaticMesh(loaded_mesh, FQuat::FindBetween(FVector(0, 0, 1), top - bot).Rotator(), bot);
		double end = FPlatformTime::Seconds();
		return listActor.Add(realActor);
	}

	FTransform objectTrasform(FRotator(0,0,0), bot, FVector(1, 1, 1));
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
	builder->Height = FVector::Distance(top, bot);
	builder->Radius = radius * rescale;
	builder->Sides = 50;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> bs;
	bs.Add(NewBrush);
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Cylinder" +
		FString::SanitizeFloat(FVector::Distance(top, bot)) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(builder->Sides)
	));

	realActor->SetActorRotation(FQuat::FindBetween(FVector(0, 0, 1), top - bot).Rotator());
	if (parent > -1)
		realActor->AttachToActor(listActor[parent], FAttachmentTransformRules::KeepRelativeTransform);
	if (current_material > -1)
		realActor->GetStaticMeshComponent()->SetMaterial(0, listMaterial[current_material]);
	double end = FPlatformTime::Seconds();
	return listActor.Add(realActor);
}

int Primitive::Cone(float px, float py, float pz, float rx, float ry, float rz, float height, float radius)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Cone"));
	Operation op = Operation();
	op.op = TypeOP::Cone;
	op.pos = FVector(px, py, pz);
	op.rot = FRotator(rx, ry, rz);
	op.radius = radius;
	op.height = height;
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Box(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz)
{

	double start = FPlatformTime::Seconds();


	UStaticMesh* loaded_mesh = FindMesh(FString("/Game/Box" +
		FString::SanitizeFloat(sx) + ":" +
		FString::SanitizeFloat(sy) + ":" +
		FString::SanitizeFloat(sz)
	));
	if (loaded_mesh) {
		AActor* realActor = PlaceStaticMesh(loaded_mesh, MyLookRotation(vx, vy), pos);

		return listActor.Add(realActor);
	}
		

		FTransform objectTrasform(FRotator(0, 0, 0), pos, FVector(1, 1, 1));
		UWorld* World = GEditor->GetEditorWorldContext().World();
		ABrush* NewBrush = World->SpawnBrush();
		NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UKhepriBox::StaticClass(), NAME_None, RF_Transactional);
		NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
		NewBrush->Brush->Initialize(NewBrush, false);
		NewBrush->SetActorRelativeTransform(objectTrasform);

			NewBrush->BrushType = EBrushType::Brush_Add;

		NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
		NewBrush->SetNeedRebuild(NewBrush->GetLevel());

		UKhepriBox* builder = (UKhepriBox*)NewBrush->BrushBuilder;
		builder->X = sx * rescale;
		builder->Y = sy* rescale;
		builder->Z = sz * rescale;
		builder->Build(World, NewBrush);

		GEditor->RebuildAlteredBSP();
		TArray<AActor*> bs;
		bs.Add(NewBrush);
		AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(bs, FString("/Game/Box" +
			FString::SanitizeFloat(sx) + ":" +
			FString::SanitizeFloat(sy) + ":" +
			FString::SanitizeFloat(sz)
		));

		realActor->SetActorRotation(MyLookRotation(vx, vy));
		realActor->SetActorLocation(pos);
		if (parent > -1) 
			realActor->AttachToActor(listActor[parent], FAttachmentTransformRules::KeepRelativeTransform);
		if (current_material > -1) 
			realActor->GetStaticMeshComponent()->SetMaterial(0, listMaterial[current_material]);


		return listActor.Add(realActor);
	
}



int Primitive::RightCuboid(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz, float angle)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a RightCuboid"));
	RightCuboidCreation op = RightCuboidCreation();
	op.op = TypeOP::RightCuboid;
	op.scale = FVector(sx, sy, sz);
	op.pos = pos;
	op.rot = ( MyLookRotation(vx, vy).Quaternion() * FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle)))).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}	
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}
int Primitive::Pyramid(TArray<FVector> ps, FVector q)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Pyramid"));
	Operation op = Operation();
	op.op = TypeOP::Pyramid;
	op.topPoint = q;
	op.base = ps;
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}
int Primitive::PyramidFrustum(TArray<FVector> ps, TArray<FVector> q)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Pyramid Frustum"));
	Operation op = Operation();
	op.op = TypeOP::PyramidFrustum;
	op.top = q;
	op.base = ps;
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Slab(TArray<FVector> contour, TArray<TArray<FVector>> holes, float h)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Slab"));
	Operation op = Operation();
	op.op = TypeOP::Slab;
	op.height = h*100;
	op.base = contour;
	op.holes = holes;
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat =listMaterial[current_material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Chair(FVector pos, float angle)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Chair"));
	Operation op = Operation();
	op.op = TypeOP::Chair;	
	op.pos = pos;
	op.rot = op.rot = FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle))).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::DeleteAll()
{
	UE_LOG(LogTemp, Warning, TEXT("Deleting All Actors"));
	Operation op = Operation();
	op.op = TypeOP::Delete;
	op.selectedActors = listActor;
	listActor.Empty();
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	
	return 0;
}

int Primitive::InstantiateBIMElement(int family, FVector pos, float angle)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Mesh"));
	Operation op = Operation();
	op.op = TypeOP::PlaceMesh;
	op.pos = pos;
	op.mesh = listMesh[family];
	op.rot = op.rot = FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle))).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}


int Primitive::CurrentParent() {
	return parent;
}

int Primitive::SetCurrentParent(int newParent) {
	if (newParent < listActor.Num()) {
		parent = newParent;
	}
	return parent;
}

int Primitive::LoadMaterial(std::string path)
{
	UE_LOG(LogTemp, Warning, TEXT("Carregar Material"));
	if (path == "") {
		return -1;
	}
	Operation op = Operation();
	op.op = TypeOP::LoadMat;
	op.path = FString(path.c_str());
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	UMaterial* mat = r.getMat();
	if (mat == NULL) {
		return -1;
	}
	current_material = listMaterial.Add(mat);
	return current_material;
}

int Primitive::LoadResource(std::string path)
{
	UE_LOG(LogTemp, Warning, TEXT("Carregar Mesh"));
	Operation op = Operation();
	op.op = TypeOP::LoadRes;
	op.path = FString(path.c_str());
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return listMesh.Add(r.getMesh());
}

int Primitive::CreateBlockInstance(int mesh, FVector pos, FVector vx, FVector vy, float scale)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Mesh"));
	Operation op = Operation();
	op.op = TypeOP::PlaceMesh;
	op.height = scale;
	op.pos = pos;
	op.mesh = listMesh[mesh]; 
	op.rot = MyLookRotation(vx, vy);
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::CurrentMaterial()
{

	return current_material;
}

int Primitive::SetCurrentMaterial(int material)
{
	current_material = material;
	return 0;
}

int Primitive::Panel(TArray<FVector> pts, FVector n, int material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Panel"));
	Operation op = Operation();
	op.op = TypeOP::Panel;
	op.topPoint = n;
	op.base = pts;

	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (material > -1) {
		op.mat = listMaterial[material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::BeamRectSection(FVector pos, FVector vx, FVector vy, float dx, float dy, float dz, float angle, int material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Beam Rect"));
	RightCuboidCreation op = RightCuboidCreation();
	op.op = TypeOP::RightCuboid;
	op.scale = FVector(dx, dy, dz);
	op.pos = pos;
	op.rot = (MyLookRotation(vx, vy).Quaternion() * FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle)))).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (material > -1) {
		op.mat = listMaterial[material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::BeamCircSection(FVector bot, float radius, FVector top, int material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Cylinder"));
	Operation op = Operation();
	op.op = TypeOP::Cylinder;
	op.pos = bot;
	op.radius = radius;
	op.height = FVector::Dist(top, bot);
	op.rot = FQuat::FindBetween(FVector(0, 0, 1), top - bot).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (material > -1) {
		op.mat = listMaterial[material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Subtract(int ac1, int ac2) {
	UE_LOG(LogTemp, Warning, TEXT("Creating a Subtraction"));
	Operation op = Operation();
	op.op = TypeOP::Subtraction;
	op.selectedActors.Add(listActor[ac1]);
	op.selectedActors.Add(listActor[ac2]);
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Unite(int ac1, int ac2) {
	UE_LOG(LogTemp, Warning, TEXT("Creating a Addition"));
	Operation op = Operation();
	op.op = TypeOP::Addition;
	op.selectedActors.Add(listActor[ac1]);
	op.selectedActors.Add(listActor[ac2]);
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];

	}
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::DeleteMany(TArray<int> acs)
{
	UE_LOG(LogTemp, Warning, TEXT("Deleting Some Actors"));
	Operation op = Operation();
	op.op = TypeOP::Delete;
	for (int ac : acs) {
		op.selectedActors.Add(listActor[ac]);
		listActor[ac] = NULL;
	}
	
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return 0;
}



/*
int Primitive::CopyMesh(char* label, int actor, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz, const char* mat)
{
	AStaticMeshActor* smActor = Cast<AStaticMeshActor>(listActor[actor]);
	Operation op = PlaceMesh(FVector(px, py, pz), FRotator(rx, ry, rz), FVector(sx, sy, sz), mat, smActor->GetStaticMeshComponent()->GetStaticMesh(), label);
	queue->Enqueue(op);
	waitForRequest();
	responsequeue->Dequeue(op);
	AActor* newActor = NULL;
	return listActor.Add(newActor);
}
*/
