// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/Actor/StaticMesh/SlabCreation.h"

#include "GeomTools.h"



SlabCreation::SlabCreation(FVector pos, TArray<FVector> bs, TArray<TArray<FVector>> holes, float hght, AActor* par, UMaterialInterface* mat) :
	ActorStaticMeshCreation(pos, FRotator(0, 0, 0), FVector(1, 1, 1), par, mat)
{
	height = hght;
	base = bs;
	_holes = holes;
	op = TypeOP::Slab;
}




Response SlabCreation::execute()
{
	FVector n = AuxiliarFunctions::PolyNormal(base);
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


	if (_holes.Num() == 0) {

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
				FVector uv1 = AuxiliarFunctions::GetUV(Vertices[ponto1] - Vertices[0], matrix);
				FVector uv2 = AuxiliarFunctions::GetUV(Vertices[ponto2] - Vertices[0], matrix);
				FVector uv3 = AuxiliarFunctions::GetUV(Vertices[ponto3] - Vertices[0], matrix);

				oneFace = { ponto1,ponto2,ponto3,  0,  FVector2D(uv1.X / 100,uv1.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv3.X / 100,uv3.Y / 100) };
				Faces.Add(oneFace);
				oneFace = { ponto3 + 1 ,ponto2 + 1,ponto1 + 1,  0, FVector2D(uv3.X / 100,uv3.Y / 100) ,FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv1.X / 100,uv1.Y / 100) };
				Faces.Add(oneFace);


			}


		}

		return Response(PlaceStaticMesh(CreateMesh(FString("Slab"
		), Vertices, Faces, Vertices.Num())));
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


	for (TArray<FVector> h : _holes) {
		dist = 0;
		TPPLPoly ph;
		ph.SetHole(true);
		ph.Init(h.Num());
		for (int j = h.Num() - 1; j >= 0; j--) {
			ph[h.Num() - 1 - j].x = h[j].X;
			ph[h.Num() - 1 - j].y = h[j].Y;
			for (int i = 0; i < 2; i = i + 1) {
				Vertices.Add(FVector(h[j].X + i, h[j].Y, h[j].Z + i * height));
			}


		}

		for (int i = 0; i < h.Num(); i++) {
			double old = dist;
			dist = dist + (FVector::Distance(Vertices[i * 2 + numberOfVertices], Vertices[((i * 2 + 2) % (h.Num() * 2)) + numberOfVertices]) * 0.01f);
			oneFace = { i * 2 + numberOfVertices,   ((i * 2 + 1) % (h.Num() * 2)) + numberOfVertices,   ((i * 2 + 3) % (h.Num() * 2)) + numberOfVertices,  0,  FVector2D(old ,0), FVector2D(old , height / 100), FVector2D(dist , height / 100) };
			Faces.Add(oneFace);
			oneFace = { ((i * 2 + 3) % (h.Num() * 2)) + numberOfVertices, ((i * 2 + 2) % (h.Num() * 2)) + numberOfVertices,   i * 2 + numberOfVertices,  0,  FVector2D(dist , height / 100), FVector2D(dist , 0), FVector2D(old, 0) };
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
	TArray<int> result = AuxiliarFunctions::convertIndices(Vertices, trigs, FVector(0, 0, Vertices[0].Z));





	FVector v0 = AuxiliarFunctions::GetUV(Vertices[1], matrix);
	for (int32 i = 0; i < result.Num() / 3; i++) {
		FVector uv1 = AuxiliarFunctions::GetUV(Vertices[result[i * 3] + 1] - Vertices[1], matrix);
		FVector uv2 = AuxiliarFunctions::GetUV(Vertices[result[i * 3 + 1] + 1] - Vertices[1], matrix);
		FVector uv3 = AuxiliarFunctions::GetUV(Vertices[result[i * 3 + 2] + 1] - Vertices[1], matrix);
		oneFace = { result[i * 3 + 2] + 1 , result[i * 3 + 1] + 1 , result[i * 3] + 1 ,  0,  FVector2D(uv3.X / 100,uv3.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv1.X / 100,uv1.Y / 100) };
		Faces.Add(oneFace);
		oneFace = { result[i * 3]   ,  result[i * 3 + 1] ,  result[i * 3 + 2]    ,  0, FVector2D(uv1.X / 100,uv1.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100),  FVector2D(uv3.X / 100,uv3.Y / 100) };
		Faces.Add(oneFace);
	}
	result.Empty();
	return Response( PlaceStaticMesh(CreateMesh(FString("Slab"
	), Vertices, Faces, Vertices.Num())));
	}

}
