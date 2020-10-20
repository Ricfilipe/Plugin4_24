// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ButtonExample.h"
#include "ButtonExampleStyle.h"
#include "ButtonExampleCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SSlider.h"
#include "Misc/Attribute.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "FRunnableConsumer.h"
#include "FRunnableProducer.h"
#include <string>
#include <iostream>	
#include <fstream>
#include "Widgets/Input/SCheckBox.h"
#include "Styling/SlateTypes.h"
#include "Engine/World.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "EditorModeManager.h"
#include "RawMesh.h"
#include "polypartition.h"
#include "Primitive.h"
#include "LevelSequenceActor.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Factories/Factory.h"
#include "LevelSequence.h"
#include "Tracks/MovieSceneCameraCutTrack.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "MovieScene.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Tracks/MovieSceneFloatTrack.h"
#include "AutomatedLevelSequenceCapture.h"
#include "Slate/SceneViewport.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/GameMode.h"
#include "Protocols/AudioCaptureProtocol.h"
#include "Protocols/ImageSequenceProtocol.h"
#include "ImageWriteTypes.h"

using namespace std;



static const FName ButtonExampleTabName("ButtonExample");

TAttribute<int> SpF;
bool AddictiveAtribute=true;

float Vy, Vz;
static int cubecount =0;
UAutomatedLevelSequenceCapture* MovieSceneCapture;

FRunnableProducer* producer;

#define LOCTEXT_NAMESPACE "FButtonExampleModule"

void FButtonExampleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

		
		ofstream myfile;
		myfile.open("C:\\Users\\Utilizador\\Documents\\teste.txt");

		int x=0, z=0;


		for (int i=0; i < 20; i++) {
			for (int j=0; j < 20; j++) {
				myfile << "cube " << x << " 0 " << z << " 0 0 0 2 2 1 /Game/StarterContent/Materials/M_Wood_Oak.M_Wood_Oak\n";
					x += 140;
			}
			x = 0;
			z += 140;
		}
		myfile.close();

	FButtonExampleStyle::Initialize();
	FButtonExampleStyle::ReloadTextures();

	FButtonExampleCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FButtonExampleCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FButtonExampleModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FButtonExampleModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FButtonExampleModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ButtonExampleTabName, FOnSpawnTab::CreateRaw(this, &FButtonExampleModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FButtonExampleTabTitle", "Khepri"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	 FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FButtonExampleModule::checkQueue),0.001f);
	 producer = new FRunnableProducer();
}

void FButtonExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	producer->Kill();



	FButtonExampleStyle::Shutdown();

	FButtonExampleCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ButtonExampleTabName);
}

TSharedRef<SDockTab> FButtonExampleModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::FromString(
		"Action done per Tick");
	SpF.Set(3);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SScrollBox) + SScrollBox::Slot().Padding(10, 5)[
			SNew(SUniformGridPanel)
			.SlotPadding(FMargin(5.0f))
			+ SUniformGridPanel::Slot(0, 0)
			.HAlign(HAlign_Left)
				[
					
					SNew(STextBlock)
					.Text(WidgetText)
				]
			+ SUniformGridPanel::Slot(1, 0)
				.HAlign(HAlign_Left)
				[

					SNew(SSpinBox<int>).MaxValue(1000.0f)
					.MinDesiredWidth(100.0f)
					.OnValueChanged_Raw(this, &FButtonExampleModule::OnSpFChanged)
					.Value(SpF)	
				]
			+ SUniformGridPanel::Slot(0, 1)
			.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this,&FButtonExampleModule::ButtonReply)[
						SNew(STextBlock)
							.Text (FText::FromString("Criar"))
					]
				]
			+ SUniformGridPanel::Slot(1, 1)
				.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this, &FButtonExampleModule::ButtonReply2)[
						SNew(STextBlock)
							.Text(FText::FromString("Apagar"))
					]
				]
			+ SUniformGridPanel::Slot(0, 2)
				.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this, &FButtonExampleModule::ButtonReply3)[
						SNew(STextBlock)
							.Text(FText::FromString("Brush"))
					]
				]
			+ SUniformGridPanel::Slot(1, 2)
				.HAlign(HAlign_Left)
				[
					SNew(SCheckBox).OnCheckStateChanged_Raw(this,&FButtonExampleModule::Addictive)
					
				]
			+ SUniformGridPanel::Slot(0, 3)
				.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this, &FButtonExampleModule::ButtonReply4)[
						SNew(STextBlock)
							.Text(FText::FromString("Make StaticMesh"))
					]
				]
			]

		
		];

}

void FButtonExampleModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(ButtonExampleTabName);
}

void FButtonExampleModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FButtonExampleCommands::Get().OpenPluginWindow);
}

void FButtonExampleModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FButtonExampleCommands::Get().OpenPluginWindow);
}

void FButtonExampleModule::OnSpFChanged(int x)
{
	SpF.Set(x);
}










struct Diagonal {
	long index1;
	long index2;

	bool operator==(const Diagonal& p) const {
		if ((index1 == p.index1) && (index2 == p.index2)) return true;
		else return false;
	}

};

struct DPState {
	bool visible;
	double weight;
	long bestvertex;
};



bool IsConvex(const FVector p1, const FVector p2, const FVector p3) {
	double tmp;
	tmp = (p3.Y - p1.Y) * (p2.X - p1.X) - (p3.X - p1.X) * (p2.Y - p1.Y);
	if (tmp > 0) return 1;
	else return 0;
}

bool InCone(FVector p1, FVector p2, FVector p3, FVector p) {
	bool convex;

	convex = IsConvex(p1, p2, p3);

	if (convex) {
		if (!IsConvex(p1, p2, p)) return false;
		if (!IsConvex(p2, p3, p)) return false;
		return true;
	}
	else {
		if (IsConvex(p1, p2, p)) return true;
		if (IsConvex(p2, p3, p)) return true;
		return false;
	}
}

int Intersects(FVector p11, FVector p12, FVector p21, FVector p22) {
	if ((p11.X == p21.X) && (p11.Y == p21.Y)) return 0;
	if ((p11.X == p22.X) && (p11.Y == p22.Y)) return 0;
	if ((p12.X == p21.X) && (p12.Y == p21.Y)) return 0;
	if ((p12.X == p22.X) && (p12.Y == p22.Y)) return 0;

	FVector v1ort, v2ort, v;
	double dot11, dot12, dot21, dot22;

	v1ort.X = p12.Y - p11.Y;
	v1ort.Y = p11.X - p12.X;

	v2ort.X = p22.Y - p21.Y;
	v2ort.Y = p21.X - p22.X;

	v = p21 - p11;
	dot21 = v.X * v1ort.X + v.Y * v1ort.Y;
	v = p22 - p11;
	dot22 = v.X * v1ort.X + v.Y * v1ort.Y;

	v = p11 - p21;
	dot11 = v.X * v2ort.X + v.Y * v2ort.Y;
	v = p12 - p21;
	dot12 = v.X * v2ort.X + v.Y * v2ort.Y;

	if (dot11 * dot12 > 0) return 0;
	if (dot21 * dot22 > 0) return 0;

	return 1;
}

FVector From3Dto2D(FVector v, FVector m[3]) {
	return FVector(v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z, v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z, 0);
}

TArray<int> Triangulate_OPT(TArray<FVector> poly) {
	long i, j, k, gap, n;
	FVector p1, p2, p3, p4;
	long bestvertex;
	DPState** dpstates = NULL;
	double weight, minweight, d1, d2;
	Diagonal diagonal, newdiagonal;
	TArray<Diagonal> diagonals;

	TArray<int> triangles;
	int ret = 1;
	n = poly.Num();
	dpstates = new DPState * [n];

	for (i = 1; i < n; i++) {
		dpstates[i] = new DPState[i];
	}

	for (i = 0; i < (n - 1); i++) {
		p1 = poly[i];
		for (j = i + 1; j < n; j++) {
			dpstates[j][i].visible = true;
			dpstates[j][i].weight = 0;
			dpstates[j][i].bestvertex = -1;
			if (j != (i + 1)) {
				p2 = poly[j];

				//visibility check
				if (i == 0) p3 = poly[n - 1];
				else p3 = poly[i - 1];
				if (i == (n - 1)) p4 = poly[0];
				else p4 = poly[i + 1];
				if (!InCone(p3, p1, p4, p2)) {
					dpstates[j][i].visible = false;
					continue;
				}

				if (j == 0) p3 = poly[n - 1];
				else p3 = poly[j - 1];
				if (j == (n - 1)) p4 = poly[0];
				else p4 = poly[j + 1];
				if (!InCone(p3, p2, p4, p1)) {
					dpstates[j][i].visible = false;
					continue;
				}

				for (k = 0; k < n; k++) {
					p3 = poly[k];
					if (k == (n - 1)) p4 = poly[0];
					else p4 = poly[k + 1];
					if (Intersects(p1, p2, p3, p4)) {
						dpstates[j][i].visible = false;
						break;
					}
				}
			}
		}
	}
	dpstates[n - 1][0].visible = true;
	dpstates[n - 1][0].weight = 0;
	dpstates[n - 1][0].bestvertex = -1;

	for (gap = 2; gap < n; gap++) {
		for (i = 0; i < (n - gap); i++) {
			j = i + gap;
			if (!dpstates[j][i].visible) continue;
			bestvertex = -1;
			for (k = (i + 1); k < j; k++) {
				if (!dpstates[k][i].visible) continue;
				if (!dpstates[j][k].visible) continue;

				if (k <= (i + 1)) d1 = 0;
				else d1 = FVector::Distance(poly[i], poly[k]);
				if (j <= (k + 1)) d2 = 0;
				else d2 = FVector::Distance(poly[k], poly[j]);

				weight = dpstates[k][i].weight + dpstates[j][k].weight + d1 + d2;

				if ((bestvertex == -1) || (weight < minweight)) {
					bestvertex = k;
					minweight = weight;
				}
			}
			if (bestvertex == -1) {
				for (i = 1; i < n; i++) {
					delete[] dpstates[i];
				}
				delete[] dpstates;

				return triangles;
			}

			dpstates[j][i].bestvertex = bestvertex;
			dpstates[j][i].weight = minweight;
		}
	}

	newdiagonal.index1 = 0;
	newdiagonal.index2 = n - 1;
	diagonals.Add(newdiagonal);
	while (!diagonals.Num() == 0) {
		diagonal = diagonals[0];
		diagonals.RemoveAt(0);
		bestvertex = dpstates[diagonal.index2][diagonal.index1].bestvertex;
		if (bestvertex == -1) {
			ret = 0;
			break;
		}

		triangles.Add(diagonal.index1);
		triangles.Add((bestvertex));
		triangles.Add(diagonal.index2);
		if (bestvertex > (diagonal.index1 + 1)) {
			newdiagonal.index1 = diagonal.index1;
			newdiagonal.index2 = bestvertex;
			diagonals.Add(newdiagonal);
		}
		if (diagonal.index2 > (bestvertex + 1)) {
			newdiagonal.index1 = bestvertex;
			newdiagonal.index2 = diagonal.index2;
			diagonals.Add(newdiagonal);
		}
	}

	for (i = 1; i < n; i++) {
		delete[] dpstates[i];
	}
	delete[] dpstates;

	return triangles;
}





TArray<int> convert2Indices(TArray<FVector> verts, TPPLPolyList polys, FVector delta) {
	TArray<int> result;
	list<TPPLPoly>::iterator iter;
	for (iter = polys.begin(); iter != polys.end(); iter++) {
		result.Add(verts.Find(FVector(iter->GetPoint(0).x, iter->GetPoint(0).y, 0) + delta));
		result.Add(verts.Find(FVector(iter->GetPoint(1).x, iter->GetPoint(1).y, 0) + delta));
		result.Add(verts.Find(FVector(iter->GetPoint(2).x, iter->GetPoint(2).y, 0) + delta));
	}
	return result;
}


void CreateStaticMesh() {
	TPPLPoly poly;
	// Object Details
	FString ObjectName = FString("MyObject");
	TArray<FVector> vertices2;
	TArray<FVector> Vertices;

	poly.Init(5);
	
		Vertices.Add(FVector(0 , 0, 0));
		Vertices.Add(FVector(0, 0,  200));
		Vertices.Add(FVector(200, 0, 0));
		Vertices.Add(FVector(200, 0,  200));
		Vertices.Add(FVector(200, 200, 0));
		Vertices.Add(FVector(200, 200,  200));
		Vertices.Add(FVector(100, 100, 0));
		Vertices.Add(FVector(100, 100, 200));
		Vertices.Add(FVector(0, 200, 0));
		Vertices.Add(FVector(0, 200,  200));

			vertices2.Add(FVector(0, 0, 0));
			vertices2.Add(FVector(200, 0,0));
			vertices2.Add(FVector(200, 200, 0));
			vertices2.Add(FVector(100, 100, 0));
			vertices2.Add(FVector(0, 200,0));
			poly[0].x = 0;
			poly[0].y = 0;
			poly[1].x = 200;
			poly[1].y = 0;
			poly[2].x = 200;
			poly[2].y = 200;
			poly[3].x = 100;
			poly[3].y = 100;
			poly[4].x = 0;
			poly[4].y = 200;

		
	
	TPPLPartition pp;

	TPPLPolyList trigs;
	if(!pp.Triangulate_MONO(&poly,&trigs))
		UE_LOG(LogTemp, Log, TEXT("ERROR!!!"));
	TArray<int> result = convert2Indices(vertices2,trigs,FVector(0,0,0));

	int numberOfVertices = Vertices.Num();

	struct Face {
		 int v1;
		 int v2;
		 int v3;
		short materialID;
		FVector2D uvCoords1;
		FVector2D uvCoords2;
		FVector2D uvCoords3;
	};


	TArray<Face> Faces;
	Face oneFace;

	double dist = FVector::Distance(Vertices[0], Vertices[2])/100;

	FVector matrix[3];
	matrix[2] = FVector::CrossProduct(Vertices[3] - Vertices[1], Vertices[5] - Vertices[1]).GetSafeNormal();
	matrix[0] = (Vertices[3] - Vertices[1]).GetSafeNormal();
	matrix[1] = FVector::CrossProduct(matrix[2],matrix[0]).GetSafeNormal();



	for (int i = 0; i < numberOfVertices / 2; i++) {
		oneFace = {   i * 2,   i * 2 + 1,   ((i * 2 + 3) % (numberOfVertices)),  0,  FVector2D(dist * i,0), FVector2D(dist*i, 2), FVector2D(dist * (i + 1), 2) };
		Faces.Add(oneFace);
		oneFace = { ((i * 2 + 3) % (numberOfVertices)), ((i * 2 + 2) % ( numberOfVertices)),   i * 2,  0,  FVector2D(dist * (i + 1),2), FVector2D(dist*(i+1), 0), FVector2D(dist * i, 0) };
		Faces.Add(oneFace);
	}

	FVector v0 = From3Dto2D(Vertices[1], matrix);
	for (int32 i = 0; i < result.Num() / 3; i++) {
		FVector uv1 = From3Dto2D(Vertices[result[i*3] * 2 + 1]- Vertices[1],matrix);
		FVector uv2 = From3Dto2D(Vertices[result[i * 3 + 1] * 2 + 1] - Vertices[1], matrix);
		FVector uv3 = From3Dto2D(Vertices[result[i * 3 + 2] * 2 + 1] - Vertices[1], matrix);
		oneFace = {result[i*3+2] * 2 + 1 , result[i*3+1] * 2 + 1 , result[i*3] * 2 + 1 ,  0,  FVector2D(uv3.X / 100,uv3.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100), FVector2D(uv1.X / 100,uv1.Y / 100) };
		Faces.Add(oneFace);
		oneFace = { result[i*3] * 2   ,  result[i*3+1] * 2 ,  result[i*3+2] * 2   ,  0, FVector2D(uv1.X / 100,uv1.Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100),  FVector2D(uv3.X / 100,uv3.Y / 100) };
		Faces.Add(oneFace);
	}

	/*

	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(0, 0, 200));


	
	for (int32 i = 0; i < numberOfVertices/2; i++){
		FVector uv1 = Vertices[i * 2 + 1]- Vertices[0];
		FVector uv2 = Vertices[((i + 1) % (numberOfVertices / 2)) * 2 + 1]-Vertices[0];
		oneFace = { i * 2 +1 ,  numberOfVertices+1,  ((i+1)% (numberOfVertices/2)) * 2+1,  0,  FVector2D(uv1.X/100,uv1.Y / 100), -FVector2D(Vertices[0].X / 100,Vertices[0].Y / 100), FVector2D(uv2.X / 100,uv2.Y / 100) };
		Faces.Add(oneFace);
		oneFace = { ((i + 1) % (numberOfVertices / 2)) * 2  ,  numberOfVertices,  i * 2   ,  0, FVector2D(uv2.X / 100,uv2.Y / 100), -FVector2D(Vertices[0].X / 100,Vertices[0].Y / 100),  FVector2D(uv1.X / 100,uv1.Y / 100) };
		Faces.Add(oneFace);
	}
	*/
	numberOfVertices = Vertices.Num();
	int numberOfFaces = Faces.Num();

	TArray<FStaticMaterial> Materials; //This should contain the real Materials, this is just an example
	Materials.Add(FStaticMaterial());
	Materials.Add(FStaticMaterial());
	int numberOfMaterials = Materials.Num();

	// Create Package
	FString pathPackage = FString("/Game");
	FString absolutePathPackage = FPaths::ProjectContentDir();

	FPackageName::RegisterMountPoint(*pathPackage, *absolutePathPackage);

	UPackage* Package = CreatePackage(nullptr, *pathPackage);

	// Create Static Mesh
	FName StaticMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), FName(*ObjectName));
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

			myRawMesh.WedgeTangentX.Add(EmptyVector);
			myRawMesh.WedgeTangentX.Add(EmptyVector);
			myRawMesh.WedgeTangentX.Add(EmptyVector);

			myRawMesh.WedgeTangentY.Add(EmptyVector);
			myRawMesh.WedgeTangentY.Add(EmptyVector);
			myRawMesh.WedgeTangentY.Add(EmptyVector);
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
		myStaticMesh->SourceModels[0].BuildSettings.bRecomputeTangents = true;
		myStaticMesh->SourceModels[0].BuildSettings.bUseMikkTSpace = false;
		myStaticMesh->SourceModels[0].BuildSettings.bGenerateLightmapUVs = true;
		myStaticMesh->SourceModels[0].BuildSettings.bBuildAdjacencyBuffer = false;
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

	};
}


ULevelSequence* CreateLevelSequence(const FString& AssetName, const FString& PackagePath, UObject* AssetToDuplicate) {
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UObject* NewAsset = nullptr;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* CurrentClass = *It;
		if (CurrentClass->IsChildOf(UFactory::StaticClass()) && !(CurrentClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UFactory* Factory = Cast<UFactory>(CurrentClass->GetDefaultObject());
			if (Factory->CanCreateNew() && Factory->ImportPriority >= 0 && Factory->SupportedClass == ULevelSequence::StaticClass())
			{
				if (AssetToDuplicate != nullptr)
				{
					NewAsset = AssetTools.DuplicateAsset(AssetName, PackagePath, AssetToDuplicate);
				}
				else
				{
					NewAsset = AssetTools.CreateAsset(AssetName, PackagePath, ULevelSequence::StaticClass(), Factory);
				}
				break;
			}
		}
	}
	return Cast<ULevelSequence>(NewAsset);
}

void  FButtonExampleModule::OnYChanged(float y) {
	Vy = y;
}

void  FButtonExampleModule::OnZChanged(float z) {
	Vz = z;
}

 FReply FButtonExampleModule::ButtonReply() {

	 producer->Kill();
	 producer = new FRunnableProducer();
	return FReply::Handled();
}

 FReply FButtonExampleModule::ButtonReply2() {

	 FButtonExampleModule::DeleteAll();
	 return FReply::Handled();
 }

 FReply FButtonExampleModule::ButtonReply3() {

	 FVector pos = FVector(0, 0, 0);
	 FRotator rot = FRotator(0, 0, 0);
	 FVector objectScale = FVector(0, 0, 0);
	 FTransform objectTrasform(rot, pos, objectScale);
	 UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	 ULevel* currentLevel = currentWorld->GetCurrentLevel();
	 UClass* staticMeshClass = ALevelSequenceActor::StaticClass();

	 AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	 ALevelSequenceActor* realActor = Cast<ALevelSequenceActor>(newActorCreated);
	 realActor->SetActorLabel("LevelSequence");
	auto original = LoadObject<ULevelSequence>(nullptr, *FString("/Game/MyStaticMeshes/LevelSequence/ExampleSequence.ExampleSequence"));
	auto copy = CreateLevelSequence(FString("LevelSequence"), FString("/Game/MyStaticMeshes/LevelSequence"), original);



	MovieSceneCapture = NewObject<UAutomatedLevelSequenceCapture>(GetTransientPackage(), UAutomatedLevelSequenceCapture::StaticClass(), FName("MovieCapture"), RF_Transient);
	MovieSceneCapture->ImageCaptureProtocolType = UImageSequenceProtocol_PNG::StaticClass();
	UImageSequenceProtocol_PNG* ImageCaptureProtocol = NewObject<UImageSequenceProtocol_PNG>(MovieSceneCapture, UImageSequenceProtocol_PNG::StaticClass(), FName("UUserDefinedImageCaptureProtocol"));
	MovieSceneCapture->ImageCaptureProtocol = ImageCaptureProtocol;
	MovieSceneCapture->LoadFromConfig();

	MovieSceneCapture->LevelSequenceAsset = copy->GetMovieScene()->GetOuter()->GetPathName();
	ULevelEditorPlaySettings* PlayInEditorSettings = GetMutableDefault<ULevelEditorPlaySettings>();

		
	const FMovieSceneCaptureSettings& Settings = MovieSceneCapture->GetSettings();
	
	PlayInEditorSettings->NewWindowWidth = Settings.Resolution.ResX;
	PlayInEditorSettings->NewWindowHeight = Settings.Resolution.ResY;
	PlayInEditorSettings->CenterNewWindow = true;
	PlayInEditorSettings->LastExecutedPlayModeType = EPlayModeType::PlayMode_InEditorFloating;

	TSharedRef<SWindow> CustomWindow = SNew(SWindow)
		.Title(LOCTEXT("MovieRenderPreviewTitle", "Movie Render - Preview"))
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.UseOSWindowBorder(true)
		.FocusWhenFirstShown(false)
		.ActivationPolicy(EWindowActivationPolicy::Never)
		.HasCloseButton(true)
		.SupportsMaximize(false)
		.SupportsMinimize(true)
		.MaxWidth(Settings.Resolution.ResX)
		.MaxHeight(Settings.Resolution.ResY)
		.SizingRule(ESizingRule::FixedSize);

	FSlateApplication::Get().AddWindow(CustomWindow);

	PlayInEditorSettings->CustomPIEWindow = CustomWindow;

	// Reset everything else
	PlayInEditorSettings->GameGetsMouseControl = false;
	PlayInEditorSettings->ShowMouseControlLabel = false;
	PlayInEditorSettings->ViewportGetsHMDControl = false;
	PlayInEditorSettings->ShouldMinimizeEditorOnVRPIE = true;
	PlayInEditorSettings->EnableGameSound = MovieSceneCapture->AudioCaptureProtocolType != UNullAudioCaptureProtocol::StaticClass();
	PlayInEditorSettings->bOnlyLoadVisibleLevelsInPIE = false;
	PlayInEditorSettings->bPreferToStreamLevelsInPIE = false;
	PlayInEditorSettings->PIEAlwaysOnTop = false;
	PlayInEditorSettings->DisableStandaloneSound = false;
	PlayInEditorSettings->AdditionalLaunchParameters = TEXT("");
	PlayInEditorSettings->BuildGameBeforeLaunch = EPlayOnBuildMode::PlayOnBuild_Never;
	PlayInEditorSettings->LaunchConfiguration = EPlayOnLaunchConfiguration::LaunchConfig_Default;
	PlayInEditorSettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
	PlayInEditorSettings->SetRunUnderOneProcess(true);
	PlayInEditorSettings->SetPlayNetDedicated(false);
	PlayInEditorSettings->SetPlayNumberOfClients(1);


	FFrameRate DisplayRate = FFrameRate();
	MovieSceneCapture->Settings.bUseCustomFrameRate = true;

	MovieSceneCapture->Settings.FrameRate = DisplayRate;
	MovieSceneCapture->Settings.ZeroPadFrameNumbers =0;
	MovieSceneCapture->Settings.bUseRelativeFrameNumbers = false;

	FFrameNumber StartFrame = FFrameNumber(0);
	FFrameNumber EndFrame = FFrameNumber(1);
	MovieSceneCapture->SetFrameOverrides(StartFrame, EndFrame);

	MovieSceneCapture->AddToRoot();
	MovieSceneCapture->OnCaptureFinished().AddRaw(this, &FButtonExampleModule::OnLevelSequenceFinished);
	MovieSceneCapture->Settings.Resolution = FCaptureResolution(1920,1080);


	UGameViewportClient::OnViewportCreated().AddRaw(this, &FButtonExampleModule::OnPIEViewportStarted);

	realActor->SetSequence(copy);
	 realActor->SetActorScale3D(objectScale);
	 realActor->SetActorRotation(rot);
	 realActor->SetActorLocation(pos);



	 GEditor->RequestPlaySession(true, nullptr, false);
	 return FReply::Handled();
 }

 FReply FButtonExampleModule::ButtonReply4() {


	 auto original = LoadObject<ULevelSequence>(nullptr, *FString("/Game/MyStaticMeshes/LevelSequence/LevelSequence.LevelSequence"));
	 FFrameRate DisplayRate = FFrameRate();

	 MovieSceneCapture = NewObject<UAutomatedLevelSequenceCapture>(GetTransientPackage(), UAutomatedLevelSequenceCapture::StaticClass(), FName(), RF_Transient);
	 MovieSceneCapture->LoadFromConfig();

	 MovieSceneCapture->LevelSequenceAsset = original->GetMovieScene()->GetOuter()->GetPathName();



	 MovieSceneCapture->Settings.bUseCustomFrameRate = true;
	 MovieSceneCapture->Settings.FrameRate = DisplayRate;
	 MovieSceneCapture->Settings.ZeroPadFrameNumbers = 0;
	 MovieSceneCapture->Settings.bUseRelativeFrameNumbers = false;

	 FFrameNumber StartFrame = FFrameNumber(0);
	 FFrameNumber EndFrame = FFrameNumber(10);
	 MovieSceneCapture->SetFrameOverrides(StartFrame, EndFrame);

	 MovieSceneCapture->AddToRoot();
	 MovieSceneCapture->OnCaptureFinished().AddRaw(this, &FButtonExampleModule::OnLevelSequenceFinished);
	 UGameViewportClient::OnViewportCreated().AddRaw(this, &FButtonExampleModule::OnPIEViewportStarted);
	 return FReply::Handled();
 }

 void FButtonExampleModule::PlacingStaticMesh() {
	 // Name & Transform

 }


 void FButtonExampleModule::OnPIEViewportStarted() {
	 for (const FWorldContext& Context : GEngine->GetWorldContexts())
	 {
		 if (Context.WorldType == EWorldType::PIE)
		 {
		
			 FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
			 if (SlatePlayInEditorSession)
			 {
				UWorld* CapturingFromWorld = Context.World();

				 TSharedPtr<SWindow> Window = SlatePlayInEditorSession->SlatePlayInEditorWindow.Pin();

				 const FMovieSceneCaptureSettings& Settings = MovieSceneCapture->GetSettings();

			
				 SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.Get()->SetViewportSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

				 FVector2D PreviewWindowSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

				 // Keep scaling down the window size while we're bigger than half the desktop width/height
				 {
					 FDisplayMetrics DisplayMetrics;
					 FSlateApplication::Get().GetCachedDisplayMetrics(DisplayMetrics);

					 while (PreviewWindowSize.X >= DisplayMetrics.PrimaryDisplayWidth * .5f || PreviewWindowSize.Y >= DisplayMetrics.PrimaryDisplayHeight * .5f)
					 {
						 PreviewWindowSize *= .5f;
					 }
				 }

				 // Resize and move the window into the desktop a bit
				 FVector2D PreviewWindowPosition(50, 50);
				 Window->ReshapeWindow(PreviewWindowPosition, PreviewWindowSize);

				 if (MovieSceneCapture->Settings.GameModeOverride != nullptr)
				 {
				
					  CapturingFromWorld->GetWorldSettings()->DefaultGameMode = MovieSceneCapture->Settings.GameModeOverride;
				 }

				 auto CachedEngineShowFlags = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport->GetClient()->GetEngineShowFlags();
				 if (CachedEngineShowFlags && Settings.bUsePathTracer)
				 {
					 auto CachedPathTracingMode = CachedEngineShowFlags->PathTracing;
					 CachedEngineShowFlags->SetPathTracing(true);
				 }
				 MovieSceneCapture->Initialize(SlatePlayInEditorSession->SlatePlayInEditorWindowViewport, Context.PIEInstance);
				 
			 }

			 FString CapturePath = MovieSceneCapture->ResolveFileFormat(MovieSceneCapture->Settings.OutputDirectory.Path, FFrameMetrics());
			 UE_LOG(LogTemp, Warning, TEXT("%s"), *(CapturePath));
			 return;
		 }
	 }

	 UE_LOG(LogTemp, Warning, TEXT("Recieved PIE Creation callback but failed to find PIE World or missing FSlatePlayInEditorInfo for world."));
 }

 void FButtonExampleModule::OnLevelSequenceFinished() {


	 GEditor->RequestEndPlayMap();

	 FEditorDelegates::EndPIE.RemoveAll(this);
	 UGameViewportClient::OnViewportCreated().RemoveAll(this);
	 MovieSceneCapture->OnCaptureFinished().RemoveAll(this);

	 MovieSceneCapture->Close();
	 MovieSceneCapture->RemoveFromRoot();
 }

   void FButtonExampleModule::DeleteAll() {
	 UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	 TArray<AActor *> toRemove;

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
	 }
	 currentWorld->CleanupActors();
	 GEditor->EditorUpdateComponents();
	 currentWorld->UpdateWorldComponents(true, false);
	 GLevelEditorModeTools().MapChangeNotify();
 }




   bool  FButtonExampleModule::checkQueue(float delta) {
	   return Primitive::checkQueue(delta, SpF.Get());
   }

   int FButtonExampleModule::GetSpF() const
   {
	   return SpF.Get();
   }

   void FButtonExampleModule::Addictive(ECheckBoxState state) {


		   AddictiveAtribute = state == ECheckBoxState::Unchecked;
	   


   }
		
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FButtonExampleModule, ButtonExample)
