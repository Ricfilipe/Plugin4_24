// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/Actor/StaticMesh/CylinderCreation.h"




CylinderCreation::CylinderCreation(FVector position, FRotator rotation, float rad, float hght, AActor* par, UMaterialInterface* mat):
	ActorStaticMeshCreation(position,rotation,FVector(1,1,1), par, mat, FString("/Game/MyStaticMeshes/Cylinder" +
		FString::SanitizeFloat(hght) + ":" +
		FString::SanitizeFloat(rad) + ":" +
		FString::SanitizeFloat(50)))
{
	radius = rad;
	height = hght;
}

Response CylinderCreation::execute()
{

	Response res = ActorStaticMeshCreation::execute();
	if (res.getResponse<AActor>() != NULL) {
		return res;
	}


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
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(dist * i,0), FVector2D(dist * i, height / 100), FVector2D(dist * (i + 1), height / 100),
			Vertices[i * 2] - Vertices[(i * 2 + numberOfVertices - 2) % (numberOfVertices)]
			,Vertices[i * 2] - Vertices[(i * 2 + numberOfVertices - 2) % (numberOfVertices)]
			,Vertices[(i * 2 + 2) % (numberOfVertices)] - Vertices[i * 2],FVector(0,0,1) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist * (i + 1),height / 100), FVector2D(dist * (i + 1), 0), FVector2D(dist * i, 0),Vertices[(i * 2 + 2) % (numberOfVertices)] - Vertices[i * 2],Vertices[(i * 2 + 2) % (numberOfVertices)] - Vertices[i * 2],Vertices[i * 2] - Vertices[(i * 2 + numberOfVertices - 2) % (numberOfVertices)],FVector(0,0,1) };
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

	return Response( PlaceStaticMesh(CreateMesh(FString("Cylinder" +
		FString::SanitizeFloat(height) + ":" +
		FString::SanitizeFloat(radius) + ":" +
		FString::SanitizeFloat(50)
	), Vertices, Faces, Vertices.Num())));
}
