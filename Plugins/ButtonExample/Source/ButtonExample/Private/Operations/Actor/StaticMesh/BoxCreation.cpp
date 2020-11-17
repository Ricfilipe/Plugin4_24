// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/Actor/StaticMesh/BoxCreation.h"



BoxCreation::BoxCreation(FVector position, FRotator rotation, FVector scale, AActor* parent, UMaterialInterface* mat) :
	ActorStaticMeshCreation(position, rotation, scale, parent, mat, FString("/Game/MyStaticMeshes/Box" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)))
{
}

Response BoxCreation::execute()
{

	Response res = ActorStaticMeshCreation::execute();
	if (res.getResponse<AActor>() != NULL) {
		return res;
	}

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


	Face oneFace = { 2,4,0,  0,  FVector2D(scale.X,0), FVector2D(scale.X, scale.Y), FVector2D(0, 0),FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(0,1,0) };
	Faces.Add(oneFace);
	oneFace = { 4,6,0,  0,  FVector2D(scale.X,scale.Y), FVector2D(0, scale.Y), FVector2D(0, 0),FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(0,1,0) };
	Faces.Add(oneFace);

	oneFace = { 1,5,3,  0,  FVector2D(0,0), FVector2D(scale.X, scale.Y), FVector2D(scale.X, 0),FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(0,1,0) };
	Faces.Add(oneFace);
	oneFace = { 1,7,5,  0,  FVector2D(0,0), FVector2D(0, scale.Y), FVector2D(scale.X, scale.Y),FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(0,1,0) };
	Faces.Add(oneFace);

	int numberOfVertices = Vertices.Num();

	for (int i = 0; i < numberOfVertices / 2; i++) {
		float dist = FVector::Distance(Vertices[i * 2 + 1], Vertices[((i * 2 + 3) % (numberOfVertices))]) / 100;
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(0,0), FVector2D(0,  scale.Z), FVector2D(dist,  scale.Z) ,  };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist, scale.Z), FVector2D(dist, 0), FVector2D(0, 0) };
		Faces.Add(oneFace);
	}
	return Response(PlaceStaticMesh(CreateMesh(FString("Box" +
		FString::SanitizeFloat(scale.X) + ":" +
		FString::SanitizeFloat(scale.Y) + ":" +
		FString::SanitizeFloat(scale.Z)
	), Vertices, Faces, Vertices.Num())));
}
