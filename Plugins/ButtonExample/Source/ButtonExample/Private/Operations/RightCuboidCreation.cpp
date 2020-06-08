// Fill out your copyright notice in the Description page of Project Settings.


#include "RightCuboidCreation.h"
#include "Operations/BoxCreation.h"
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
#include "CustomBrushes/KhepriRightCuboid.h"


Response RightCuboidCreation::execute(UPackage* Package)
{
	UE_LOG(LogTemp, Warning, TEXT("Polymorphism"));
	TArray<FVector> Vertices;
	TArray<Face> Faces;

	Vertices.Add(FVector(-scale.X * rescale / 2, -scale.Y * rescale / 2, 0));
	Vertices.Add(FVector(-scale.X * rescale / 2, -scale.Y * rescale / 2, scale.Z * rescale));

	Vertices.Add(FVector(scale.X * rescale / 2, -scale.Y * rescale / 2, 0));
	Vertices.Add(FVector(scale.X * rescale / 2, -scale.Y * rescale / 2, scale.Z * rescale));

	Vertices.Add(FVector(scale.X * rescale / 2, scale.Y * rescale / 2, 0));
	Vertices.Add(FVector(scale.X * rescale / 2, scale.Y * rescale / 2, scale.Z * rescale));

	Vertices.Add(FVector(-scale.X * rescale / 2, scale.Y * rescale / 2, 0));
	Vertices.Add(FVector(-scale.X * rescale / 2, scale.Y * rescale / 2, scale.Z * rescale));


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