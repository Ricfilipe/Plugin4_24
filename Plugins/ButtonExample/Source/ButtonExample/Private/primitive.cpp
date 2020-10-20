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
#include <Operations\Actor\StaticMesh\BoxCreation.h>
#include "Operations/Actor/StaticMesh/CylinderCreation.h"
#include <Operations\Actor\StaticMesh\RightCuboidCreation.h>
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>
#include <Operations\Actor\StaticMesh\PyramidCreation.h>
#include <Operations\Actor\StaticMesh\PyramidFrustumCreation.h>
#include <Operations\Actor\StaticMesh\SlabCreation.h>
#include <Operations\ActorOperation\DeleteCreation.h>
#include <Operations\Actor\StaticMesh\StaticMeshCreation.h>
#include <Operations\Actor\StaticMesh\PanelCreation.h>
#include <Operations\ActorOperation\SubtractionCreation.h>
#include <Operations\ActorOperation\AdditionCreation.h>
#include <Operations\Actor\Light\PointLightCreation.h>
#include <Operations\Actor\Render\RenderCreation.h>
#include <Operations\Actor\Light\SpotlightCreation.h>
#include <Operations\Load\LoadCreation.h>
#include <Operations\Actor\Render\ChangeViewOperation.h>


int cube = 0, cylinder = 0, sphere = 0, object = 0;
int N_StaticMesh = 0;
TArray< AActor* > brushes;
int parent = -1;
int current_material = -1;

FVector nextFramePosition;
FRotator nextFramerotation;
float nextFrameCamera[3];


TQueue<Operation*>* requestQueue = new TQueue<Operation*, EQueueMode::Spsc>();
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






UMaterialInterface* Primitive::intToUMaterial(int index)
{
	if (index > -1) {
		return listMaterial[index];
	}

	return NULL;
}

UStaticMesh* Primitive::intToUStaticMesh(int index)
{
	if (index > -1) {
		return listMesh[index];
	}

	return NULL;
}

AActor* Primitive::intToAActor(int index)
{
	if (index > -1) {
		return listActor[index];
	}

	return NULL;
}

bool  Primitive::checkQueue(float delta, int SpFs) {
	int num = 0;
	Operation* fo;




	// Create Static Mesh


	while (!(requestQueue->IsEmpty()) ) {

		requestQueue->Dequeue(fo);

		responsequeue->Enqueue(fo->execute());
		
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
	
	AActor* par = NULL;
	UMaterialInterface* mat = NULL;
	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}
	CylinderCreation op = CylinderCreation(bot, FQuat::FindBetween(FVector(0, 0, 1), top - bot).Rotator(), radius, FVector::Dist(top, bot),par,mat);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}



int Primitive::Box(FVector pos, FVector vx, FVector vy, float sx, float sy, float sz)
{

	//mover para main thread


		UE_LOG(LogTemp, Warning, TEXT("Creating a Cube"));

		AActor* par = NULL;
		UMaterialInterface* mat = NULL;
		if (parent > -1) {
			par = listActor[parent];
		}
		if (current_material > -1) {
			mat = listMaterial[current_material];
		}
		BoxCreation op = BoxCreation(pos, MyLookRotation(vx, vy), FVector(sx,sy,sz),par,mat);
		requestQueue->Enqueue(&op);
		waitForRequest();
		Response r;
		responsequeue->Dequeue(r);
		AActor* newActor = r.getResponse<AActor>();
		return listActor.Add(newActor);
		
}



int Primitive::RightCuboid(FVector pos, FVector vx, FVector vz, float sx, float sy, float sz, float angle)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a RightCuboid"));

	AActor* par = NULL;
	UMaterialInterface* mat = NULL;
	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}
	RightCuboidCreation op = RightCuboidCreation(pos, (MyLookRotation(vx, vz).Quaternion() * FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle)))).Rotator(),FVector(sx,sy,sz),par,mat);

	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
	
}
int Primitive::Pyramid(TArray<FVector> ps, FVector q)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Pyramid"));

	AActor* par = NULL;
	UMaterialInterface* mat = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}

	PyramidCreation op = PyramidCreation(FVector(0, 0, 0), ps, q, par, mat);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}

int Primitive::PyramidFrustum(TArray<FVector> ps, TArray<FVector> q)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Pyramid Frustum"));
	AActor* par = NULL;
	UMaterialInterface* mat = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}

	PyramidFrustumCreation op = PyramidFrustumCreation(FVector(0, 0, 0), ps, q, par, mat);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}



int Primitive::Slab(TArray<FVector> contour, TArray<TArray<FVector>> holes, float h, UMaterialInterface* material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Slab"));
	
	TArray<FVector> base = contour;

	for (int i = 0; i < base.Num(); i++) {
		base[i] = base[i] - contour[0];
	}


	for (TArray<FVector> vectors : holes) {
		for (int i = 0; i < vectors.Num(); i++) {
			vectors[i]= vectors[i] - contour[0];
		}
	}

	AActor* par = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}

	SlabCreation op = SlabCreation(contour[0], base,holes, h * 100, par, material);

	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}


int Primitive::DeleteAll()
{
	UE_LOG(LogTemp, Warning, TEXT("Deleting All Actors"));
	DeleteCreation op = DeleteCreation(listActor);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	listActor.Empty();
	return 0;
}

int Primitive::InstantiateBIMElement(UStaticMesh* family, FVector pos, float angle)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Mesh"));
	AActor* par = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}

	StaticMeshCreation op = StaticMeshCreation(pos, FQuat::MakeFromEuler(FVector(0, 0, FMath::RadiansToDegrees(angle))).Rotator(), FVector(1, 1, 1),par,family);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
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
	LoadCreation<UMaterialInterface> op = LoadCreation<UMaterialInterface>(FString(path.c_str()));
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	UMaterialInterface* mat =(r.getResponse<UMaterialInterface>());
	if (mat == NULL) {
		return -1;
	}
	current_material = listMaterial.Add(mat);
	return current_material;
}

int Primitive::LoadResource(std::string path)
{
	UE_LOG(LogTemp, Warning, TEXT("Carregar Mesh"));
	LoadCreation<UStaticMesh> op = LoadCreation<UStaticMesh>(FString(path.c_str()));
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return listMesh.Add((r.getResponse<UStaticMesh>()));
}

int Primitive::CreateBlockInstance(UStaticMesh* mesh, FVector pos, FVector vx, FVector vy, float scale)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Mesh"));
	


	AActor* par = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}

	StaticMeshCreation op = StaticMeshCreation(pos, MyLookRotation(vx, vy), FVector(scale, scale, scale), par, mesh);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
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

int Primitive::Panel(TArray<FVector> pts, FVector n, UMaterialInterface* material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Panel"));


	TArray<FVector> base = pts;
	for (int i = 0; i < base.Num(); i++) {
		base[i] = base[i] - pts[0];
	}

	AActor* par = NULL;
	UMaterialInterface* mat = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}
	PanelCreation op = PanelCreation(pts[0], base, n, par, material);

	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}

int Primitive::BeamRectSection(FVector pos, FVector vx, FVector vy, float dx, float dy, float dz, float angle, UMaterialInterface* material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Beam Rect"));
	AActor* par = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}

	RightCuboidCreation op = RightCuboidCreation(pos, (MyLookRotation(vx, vy).Quaternion() * FQuat::MakeFromEuler(FVector(0, 0, -FMath::RadiansToDegrees(angle)))).Rotator(), FVector(dx, dy, dz), par, material);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}

int Primitive::BeamCircSection(FVector bot, float radius, FVector top, UMaterialInterface* material)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Cylinder"));

	AActor* par = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}


	CylinderCreation op = CylinderCreation(bot, FQuat::FindBetween(FVector(0, 0, 1), top - bot).Rotator(), radius, FVector::Dist(top, bot), par, material);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}

int Primitive::Subtract(AActor* ac1, AActor* ac2) {
	UE_LOG(LogTemp, Warning, TEXT("Creating a Subtraction"));
	
	TArray<AActor*> selectedActors;
	 selectedActors.Add(ac1);
	selectedActors.Add(ac2);
	
	AActor* par = NULL;
	UMaterialInterface* mat = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}

	SubtractionCreation op = SubtractionCreation(selectedActors,par,mat);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}

int Primitive::Unite(AActor* ac1, AActor* ac2) {
	UE_LOG(LogTemp, Warning, TEXT("Creating a Addition"));
	TArray<AActor*> selectedActors = TArray<AActor*>();
	selectedActors.Add(ac1);
	selectedActors.Add(ac2);

	AActor* par = NULL;
	UMaterialInterface* mat = NULL;

	if (parent > -1) {
		par = listActor[parent];
	}
	if (current_material > -1) {
		mat = listMaterial[current_material];
	}

	AdditionCreation op = AdditionCreation(selectedActors, par, mat);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}

int Primitive::DeleteMany(TArray<AActor*> acs)
{
	UE_LOG(LogTemp, Warning, TEXT("Deleting Some Actors"));

	for (AActor* ac : acs) {
		listActor.Remove(ac);
	}
	
	DeleteCreation op = DeleteCreation(acs);
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return 0;
}


int Primitive::PointLight(FVector position, FLinearColor color, float range, float intensity)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a PointLight"));

	AActor* par = NULL;
	if (parent > -1) {
		par = listActor[parent];
	}

	PointLightCreation op = PointLightCreation(position, par, color, intensity, range);

	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}


int Primitive::SetView(FVector position, FVector target, float lens, float aperture)
{
	ChangeViewOperation op = ChangeViewOperation(position,target);
	nextFramePosition = position;
	nextFramerotation = UKismetMathLibrary::FindLookAtRotation(position, target);
	nextFrameCamera[0] = FVector::Distance(position,target);
	nextFrameCamera[2] = lens;
	nextFrameCamera[1] = aperture;
	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return 0;
}

FVector Primitive::ViewCamera()
{
	return nextFramePosition;
}


FVector Primitive::ViewTarget()
{
	return nextFramePosition + nextFramerotation.Vector() * nextFrameCamera[0];
}

float Primitive::ViewLens()
{
	return nextFrameCamera[2];
}

int Primitive::RenderView(int width, int height, std::string name, std::string path, int frame)
{
	RenderCreation op = RenderCreation(nextFramePosition,nextFramerotation, nextFrameCamera[0], nextFrameCamera[1], nextFrameCamera[2],width,height,frame, FString(name.c_str()), FString(path.c_str()));

	requestQueue->Enqueue(&op);

	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	return 0;
}

int Primitive::Spotlight(FVector position, FVector dir, FLinearColor color, float range, float intensity, float hotspot, float falloff)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating a Spotlight"));


	AActor* par = NULL;
	if (parent > -1) {
		par = listActor[parent];
	}	
	
	SpotlightCreation op = SpotlightCreation(position, FRotator::MakeFromEuler(dir), par, color, intensity, range, hotspot, falloff);

	requestQueue->Enqueue(&op);
	waitForRequest();
	Response r;
	responsequeue->Dequeue(r);
	AActor* newActor = r.getResponse<AActor>();
	return listActor.Add(newActor);
}


