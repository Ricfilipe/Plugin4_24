// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorStaticMeshCreation.h"
#include "Engine/Polys.h"
#include "RawMesh.h"
#include "GeomTools.h"


ActorStaticMeshCreation::ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par): ActorCreation(position, rotation, scl, par) 
{
	pos = position;
	rot = rotation;
	scale = scl;
	parent = par;
}

ActorStaticMeshCreation::ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par, FString meshPath) : ActorCreation(position, rotation, scl, par)
{
	pos = position;
	rot = rotation;
	scale = scl;
	parent = par;
	path = meshPath;
}

ActorStaticMeshCreation::ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par, UMaterialInterface* material) : ActorCreation(position, rotation, scl, par)
{
	pos = position;
	rot = rotation;
	scale = scl;
	parent = par;
	mat = material;
}

ActorStaticMeshCreation::ActorStaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par, UMaterialInterface* material, FString meshPath) : ActorCreation(position, rotation, scl, par)
{
	pos = position;
	rot = rotation;
	scale = scl;
	parent = par;
	mat = material;
	path = meshPath;
}

Response ActorStaticMeshCreation::execute()
{
	UStaticMesh* loaded_mesh = lookForMesh(path);
	if (loaded_mesh) {
		return  Response(PlaceStaticMesh(loaded_mesh));
	}
	return  Response();
}

UStaticMesh* ActorStaticMeshCreation::lookForMesh(FString mesh)
{
	mesh = mesh.Replace(TEXT("."), TEXT("a"));
	mesh = mesh.Replace(TEXT(":"), TEXT("b"));
	return LoadObject<UStaticMesh>(nullptr, *(mesh));
}

AActor* ActorStaticMeshCreation::PlaceStaticMesh(UStaticMesh* mesh)
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
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);




	return realActor;
}

UStaticMesh* ActorStaticMeshCreation::CreateMesh(FString name, TArray<FVector> Vertices, TArray<Face> Faces, int size)
{
	// Object Details
	FString ObjectName = name;
	int numberOfVertices = size;

	ObjectName = ObjectName.Replace(TEXT("."), TEXT("a"));
	ObjectName = ObjectName.Replace(TEXT(":"), TEXT("b"));

	Face oneFace;
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
	if (op == Slab || op == Panel || op == PyramidFrustumWall) {
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


//Auxilliar Functions

TArray<int> AuxiliarFunctions::convertIndices(TArray<FVector> verts, TPPLPolyList polys, FVector delta)
{
	TArray<int> result;
	std::list<TPPLPoly>::iterator iter;
	for (iter = polys.begin(); iter != polys.end(); iter++) {
		result.Add(verts.Find(FVector(iter->GetPoint(0).x, iter->GetPoint(0).y, 0) + delta));
		result.Add(verts.Find(FVector(iter->GetPoint(1).x, iter->GetPoint(1).y, 0) + delta));
		result.Add(verts.Find(FVector(iter->GetPoint(2).x, iter->GetPoint(2).y, 0) + delta));
	}
	return result;
}

FVector AuxiliarFunctions::PolyNormal(TArray<FVector> pts) {
	FVector pt = pts[0];
	FVector sum = FVector::ZeroVector;
	for (int i = 1; i < pts.Num() - 1; i++) {
		if (pts[i] == pt || pts[i + 1] == pt) continue;
		sum += FVector::CrossProduct(pts[i] - pt, pts[i + 1] - pt);
	}
	sum.Normalize();
	return sum;
}

FVector AuxiliarFunctions::GetUV(FVector v, FVector m[3]) {
	return FVector(v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z, v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z, 0);
}

