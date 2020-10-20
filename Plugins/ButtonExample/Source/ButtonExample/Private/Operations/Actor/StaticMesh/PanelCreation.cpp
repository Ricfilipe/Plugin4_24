// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/Actor/StaticMesh/PanelCreation.h"
#include "GeomTools.h"

using namespace AuxiliarFunctions;

PanelCreation::PanelCreation(FVector pos, TArray<FVector> bs, FVector n, AActor* par, UMaterialInterface* mat) :
	ActorStaticMeshCreation(pos, FRotator(0, 0, 0), FVector(1, 1, 1), par, mat)
{
	normal = n;
	base = bs;
}

Response PanelCreation::execute()
{
	FVector n = PolyNormal(base);


	TArray<FVector> Vertices;
	TArray<Face> Faces;
	if (n.Z > 1) {
		for (int j = 0; j < base.Num(); j++) {
			FVector v = base[j];
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(v.X + i, v.Y, v.Z + i * normal.Size()));
			}
		}
	}
	else {
		n = -n;
		for (int j = base.Num() - 1; j >= 0; j--) {
			FVector v = base[j];
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(v.X + i, v.Y, v.Z + i * normal.Size()));
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
		oneFace = { i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(dist * i,0), FVector2D(dist * i + delta.X * normal.Size(),  delta.Y * normal.Size() / 100), FVector2D(dist * (i + 1) + delta.X * normal.Size(), delta.Y * normal.Size() / 100) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % (numberOfVertices)),   i * 2,  0,  FVector2D(dist * (i + 1) + delta.X * normal.Size(), delta.Y * normal.Size() / 100), FVector2D(dist * (i + 1), 0), FVector2D(dist * i, 0) };
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

	return Response( PlaceStaticMesh(CreateMesh(FString("Panel"
	), Vertices, Faces, Vertices.Num())));
}
