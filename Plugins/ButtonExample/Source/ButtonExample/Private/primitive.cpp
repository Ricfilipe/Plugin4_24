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

int cube = 0, cylinder = 0, sphere = 0, object = 0;
int N_StaticMesh = 0;
TArray< AActor* > brushes;

TQueue<Operation>* queue = new TQueue<Operation, EQueueMode::Spsc>();
TQueue<Response>* responsequeue = new TQueue<Response, EQueueMode::Spsc>();
//--------------------------------------------------------Placing Objects---------------------------------------------------------------------------//

TArray<AActor*> listActor;

FRotator MyLookRotation(FVector lookAt, FVector upDirection)
{


	FVector forward = lookAt;
	FVector up = upDirection;


	forward = forward.GetSafeNormal();
	up = up - (forward * FVector::DotProduct(up, forward));
	up = up.GetSafeNormal();

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
		FPlatformProcess::Sleep(0.01f);
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
	while (!(queue->IsEmpty()) && num < SpF) {

		queue->Dequeue(fo);

		responsequeue->Enqueue(fo.execute());
		num++;
	}
	return true;
}


//Class'/Script/CinematicCamera.CineCameraActor'

// Delete All Objects
static  void DeleteAll() {
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	TArray<AActor*> toRemove;

	int size = currentWorld->PersistentLevel->Actors.Num();

	for (int i = 0; i < size; i++) {
		if (currentWorld->PersistentLevel->Actors[i]->ActorHasTag(FName("StaticMesh"))) {
			toRemove.Add(currentWorld->PersistentLevel->Actors[i]);
		};
	}

	size = toRemove.Num();

	for (int i = 0; i < size; i++) {

		currentWorld->RemoveActor(toRemove[i], true);
		toRemove[i]->Destroy();
		delete toRemove[i];
		toRemove[i] = NULL;
	}
	currentWorld->CleanupActors();
	GEditor->EditorUpdateComponents();
	currentWorld->UpdateWorldComponents(true, false);
	currentWorld->ForceGarbageCollection(true);
	GLevelEditorModeTools().MapChangeNotify();
}

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

AActor* Primitive::ConvertToStaticMesh(const TArray<AActor*> bs)
{
	char buffer[32];
	itoa(N_StaticMesh, buffer, 10);
	GEditor->DoConvertActors(bs, AStaticMeshActor::StaticClass(), TSet<FString>(), true, FString("/Game/StaticMesh") + buffer);
	N_StaticMesh++;
	return bs[0];
}


//BIM Objects
void Primitive::createBeamRect(FVector objectPosition, FRotator objectRotation, FVector objectScale)
{
	// Primitive::spawnCube(objectPosition, objectRotation, objectScale, true);
}

void Primitive::createBeamCircle(FVector objectPosition, FRotator objectRotation, float height, float radius, int slides)
{
	FTransform objectTrasform(objectRotation, objectPosition, FVector(1, 1, 1));
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ABrush* NewBrush = World->SpawnBrush();
	NewBrush->BrushBuilder = NewObject<UBrushBuilder>(NewBrush, UCylinderBuilder::StaticClass(), NAME_None, RF_Transactional);
	NewBrush->Brush = NewObject<UModel>(NewBrush, NAME_None, RF_Transactional);
	NewBrush->Brush->Initialize(NewBrush, false);
	NewBrush->SetActorRelativeTransform(objectTrasform);
	NewBrush->BrushBuilder->Build(NewBrush->GetWorld(), NewBrush);
	NewBrush->SetNeedRebuild(NewBrush->GetLevel());
	UCylinderBuilder* builder = (UCylinderBuilder*)NewBrush->BrushBuilder;
	builder->Z = 200 * height;
	builder->OuterRadius = radius;
	builder->Sides = slides;
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> b;
	b.Add(NewBrush);
	ConvertToStaticMesh(b);
}

void Primitive::createBeamCustom(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, int size)
{
	spawnCustom(objectPosition, objectRotation, height, vertices, size, true);
	ConvertToStaticMesh();
}

void Primitive::createSlabs(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, TArray<TArray<FVector>> holes, int size, int num_holes, int* size_holes)
{
	spawnCustom(objectPosition, objectRotation, height, vertices, size, true);

	for (int i = 0; i < num_holes; i++) {
		spawnCustom(objectPosition, objectRotation, height, holes[i], size_holes[i], false);
	}
	ConvertToStaticMesh();
}

void Primitive::createPanels(FVector objectPosition, FRotator objectRotation, float height, TArray<FVector> vertices, FVector Normal, int size)
{
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
	UVertixBuilder* builder = (UVertixBuilder*)NewBrush->BrushBuilder;
	builder->DrawVertices = vertices;
	builder->Size = size;
	builder->Height = height;
	builder->NormalVector = Normal;
	builder->Build(World, NewBrush);
	GEditor->RebuildAlteredBSP();
	TArray<AActor*> b;
	b.Add(NewBrush);
	ConvertToStaticMesh(b);

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
	Operation op = Operation();
	op.op = TypeOP::Sphere;
	op.pos = FVector(x, y, z);
	op.radius = radius;
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
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
int Primitive::Cylinder(float x, float y, float z, float rx, float ry, float rz, float height, float radius)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Cylinder"));
	Operation op = Operation();
	op.op = TypeOP::Cylinder;
	op.pos = FVector(x, y, z);
	op.radius = radius;
	op.height = height;
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
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
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Box(float px, float py, float pz, float rx, float ry, float rz, float tx, float ty, float tz, float sx, float sy, float sz)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Cube"));
	BoxCreation op = BoxCreation();
	op.op = TypeOP::Cube;
	op.scale = FVector(sx, sy, sz);
	op.pos = FVector(px, py, pz);
	op.rot = MyLookRotation(FVector(rx, ry, rz), FVector(tx, ty, tz));
	queue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
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
