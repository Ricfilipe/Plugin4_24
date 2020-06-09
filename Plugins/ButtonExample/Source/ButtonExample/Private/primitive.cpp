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
#include "Engine/Polys.h"
#include "Operations/BoxCreation.h"
#include "Operations/RightCuboidCreation.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>


int cube = 0, cylinder = 0, sphere = 0, object = 0;
int N_StaticMesh = 0;
TArray< AActor* > brushes;
int parent = -1;
int current_material = -1;

TQueue<Operation>* requestQueue = new TQueue<Operation, EQueueMode::Spsc>();
TQueue<Response>* responsequeue = new TQueue<Response, EQueueMode::Spsc>();
//--------------------------------------------------------Placing Objects---------------------------------------------------------------------------//

TArray<AActor*> listActor;
TArray<UMaterialInterface*> listMaterial;
TArray<UStaticMesh*> listMesh;

ACameraActor* camera;

FRotator MyLookRotation(FVector lookAt, FVector upDirection)
{
	FVector forward = lookAt /100;
	FVector up = upDirection/100;
	


	///////////////////////


	UE_LOG(LogTemp, Warning, TEXT("forward: %s"), *forward.ToString());

	UE_LOG(LogTemp, Warning, TEXT("up: %s"), *up.ToString());

	return UKismetMathLibrary::MakeRotFromXZ(lookAt.GetSafeNormal(), upDirection.GetSafeNormal());

	FVector vector = forward;
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






bool  Primitive::checkQueue(float delta, int SpF) {
	int num = 0;
	Operation fo;




	// Create Static Mesh


	while (!(requestQueue->IsEmpty()) ) {

		requestQueue->Dequeue(fo);

		responsequeue->Enqueue(fo.execute(NULL));
		
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
	Operation op = Operation();
	op.op = TypeOP::Sphere;
	op.pos = FVector(x, y, z);
	op.radius = radius;
	requestQueue->Enqueue(op);
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
int Primitive::Cylinder(FVector bot, float radius, FVector top)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Creating a Cylinder"));
	Operation op = Operation();
	op.op = TypeOP::Cylinder;
	op.pos = bot;
	op.radius = radius;
	op.height = FVector::Dist(top,bot);
	op.rot = FQuat::FindBetween(FVector(0,0,1),top-bot).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	requestQueue->Enqueue(op);
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
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	requestQueue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Box(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz)
{

	//mover para main thread


		UE_LOG(LogTemp, Warning, TEXT("Creating a Cube"));
		BoxCreation op = BoxCreation();
		op.op = TypeOP::Cube;
		op.scale = FVector(sx, sy, sz);
		op.pos = pos;
		op.rot = MyLookRotation(vx, vy);
		if (parent > -1) {
			op.parent = listActor[parent];
		}
		if (current_material > -1) {
			op.mat = listMaterial[current_material];
		}
		requestQueue->Enqueue(op);
		waitForRequest();
		Response r;
		responsequeue->Dequeue(r);
		AActor* newActor = r.getResponse();
		return listActor.Add(newActor);
	
}



int Primitive::RightCuboid(FVector pos, FVector vx, FVector vz, float sx, float sy, float sz, float angle)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a RightCuboid"));
	RightCuboidCreation op = RightCuboidCreation();
	op.op = TypeOP::RightCuboid;
	op.scale = FVector(sx, sy, sz);
	op.pos = pos;

	op.rot = ( MyLookRotation(vx, vz).Quaternion() * FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle)))).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}	
	if (current_material > -1) {
		op.mat = listMaterial[current_material];
	}
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::PyramidFrustumWithMaterial(TArray<FVector> ps, TArray<FVector> q, int material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Pyramid Frustum"));
	Operation op = Operation();
	if (ps.Num() == 4) {
		op.op = TypeOP::RightCuboid;
		op.pos =  q[1] + (ps[0]-q[1])/2;
		op.scale = FVector(FVector::Distance(ps[0],ps[1]), FVector::Distance(ps[0], q[0]), FVector::Distance(ps[0], ps[3]))/100;
		op.rot = MyLookRotation(ps[1]-ps[0], ps[3]-ps[0]);
	}else{
	op.op = TypeOP::PyramidFrustumWall;
	op.top = q;
	op.base = ps;
	}

	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (material > -1) {
		op.mat = listMaterial[material];
	}

	requestQueue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}

int Primitive::Slab(TArray<FVector> contour, TArray<TArray<FVector>> holes, float h, int material)
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
	if (material > -1) {
		op.mat = listMaterial[material];
	}
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	UMaterialInterface* mat = r.getMat();
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	op.rot = (MyLookRotation(vx, vy).Quaternion() * FQuat::MakeFromEuler(FVector(0, 0, -FMath::RadiansToDegrees(angle)))).Rotator();
	if (parent > -1) {
		op.parent = listActor[parent];
	}
	if (material > -1) {
		op.mat = listMaterial[material];
	}
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	requestQueue->Enqueue(op);
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
	
	requestQueue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return 0;
}


int Primitive::PointLight(FVector position, FLinearColor color, float range, float intensity)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a PointLight"));
	Operation op = Operation();
	op.op = TypeOP::PointLight;
	op.pos = position;
	op.radius = range;
	op.height = intensity;
	op.color = color;
	if (parent > -1) {
		op.parent = listActor[parent];
	}

	requestQueue->Enqueue(op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse();
	return listActor.Add(newActor);
}


int Primitive::SetView(FVector position, FVector target, float lens)
{
	Operation op = Operation();
	op.op = TypeOP::Camera;
	op.pos = position;
	op.rot = UKismetMathLibrary::FindLookAtRotation(position, target);
	op.scale = target;
	op.radius = lens;

	requestQueue->Enqueue(op);

	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	camera = Cast<ACameraActor>( r.getResponse());
	return 0;
}

FVector Primitive::ViewCamera()
{
	if (camera != NULL) {
		return FVector();
	}
	return  FVector();
}


FVector Primitive::ViewTarget()
{
	if (camera != NULL) {
		return FVector();
	}
	return FVector();
}

float Primitive::ViewLens()
{
	if (camera != NULL) {
		return 0;
	}
	return 0;
}

int Primitive::RenderView(int width, int height, FString name, FString path, int frame)
{
	Operation op = Operation();
	op.op = TypeOP::Render;
	op.param[0] = width;
	op.param[1] = height;
	op.param[2] = frame;
	op.name = name;
	op.path = path;
	requestQueue->Enqueue(op);

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
