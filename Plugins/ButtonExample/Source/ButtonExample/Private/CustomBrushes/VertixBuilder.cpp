// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomBrushes/VertixBuilder.h"
#include "Builders/EditorBrushBuilder.h"
#include "EditorStyleSet.h"
#include "GameFramework/Actor.h"
#include "Engine/Brush.h"
#include "Engine/Polys.h"
#include "Editor.h"
#include "BSPOps.h"
#include "SnappingUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Engine/Selection.h"


#define LOCTEXT_NAMESPACE "BrushBuilder"

UEditorBrushBuilder::UEditorBrushBuilder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BitmapFilename = TEXT("BBGeneric");
	ToolTip = TEXT("BrushBuilderName_Generic");
	NotifyBadParams = true;
}


UVertixBuilder::UVertixBuilder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName NAME_Custom;
		FConstructorStatics()
			: NAME_Custom(TEXT("Custom"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	GroupName = ConstructorStatics.NAME_Custom;
	Height = 200;
	NormalVector = FVector(0, 0, 1);
	Size = 0;
	BitmapFilename = TEXT("Btn_Custom");
	ToolTip = TEXT("BrushBuilderName_Custom");
}


void UVertixBuilder::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UVertixBuilder::Build(UWorld * InWorld, ABrush * InBrush)
{
	

	BeginBrush(false, GroupName);
	if (Size != 0) {
		BuildCustom(+1, DrawVertices, Height, Size);

	
			PolyBegin(1, FName(TEXT("Cap")));
			for (int j = 0; j < Size*2; j = j + 2)
				Polyi(j);
			PolyEnd();

			PolyBegin(1, FName(TEXT("Cap")));
			for (int j = Size*2 -1; j >0; j = j - 2)
				Polyi(j);
			PolyEnd();
			TArray<FPoly> Polygons;
			
	}
	return EndBrush(InWorld, InBrush);

}

void UVertixBuilder::BuildCustom(int32 Direction, TArray<FVector> vertices, float height, int number)
{
	for (int j = 0; j < number; j++) {
		FVector v = vertices[j];
		for (int i = -1; i < 2; i = i + 2) {
			Vertex3f(v.X+ i * NormalVector.X*height / 2, v.Y+ i * NormalVector.Y*height / 2, v.Z + i*NormalVector.Z*height/2);
		}
	}
	for (int32 i = 0; i < number; i++)
		Poly4i(Direction,  i * 2,  i * 2 + 1, ((i * 2 + 3) % (2 * number)),  ((i * 2 + 2) % (2 * number)), FName(TEXT("Wall")));
}





//Solving virtual methods

void UEditorBrushBuilder::BeginBrush(bool InMergeCoplanars, FName InLayer)
{
	Layer = InLayer;
	MergeCoplanars = InMergeCoplanars;
	Vertices.Empty();
	Polys.Empty();
}

bool UEditorBrushBuilder::EndBrush(UWorld* InWorld, ABrush* InBrush)
{
	//!!validate
	check(InWorld != nullptr);
	ABrush* BuilderBrush = (InBrush != nullptr) ? InBrush : InWorld->GetDefaultBrush();

	// Ensure the builder brush is unhidden.
	BuilderBrush->bHidden = false;
	BuilderBrush->bHiddenEdLayer = false;

	AActor* Actor = GEditor->GetSelectedActors()->GetTop<AActor>();
	FVector Location;
	if (InBrush == nullptr)
	{
		Location = Actor ? Actor->GetActorLocation() : BuilderBrush->GetActorLocation();
	}
	else
	{
		Location = InBrush->GetActorLocation();
	}

	UModel* Brush = BuilderBrush->Brush;
	if (Brush == nullptr)
	{
		return true;
	}

	Brush->Modify();
	BuilderBrush->Modify();

	FRotator Temp(0.0f, 0.0f, 0.0f);
	FSnappingUtils::SnapToBSPVertex(Location, FVector::ZeroVector, Temp);
	BuilderBrush->SetActorLocation(Location, false);
	BuilderBrush->SetPivotOffset(FVector::ZeroVector);

	// Try and maintain the materials assigned to the surfaces. 
	TArray<FPoly> CachedPolys;
	UMaterialInterface* CachedMaterial = nullptr;
	if (Brush->Polys->Element.Num() == Polys.Num())
	{
		// If the number of polygons match we assume its the same shape.
		CachedPolys.Append(Brush->Polys->Element);
	}
	else if (Brush->Polys->Element.Num() > 0)
	{
		// If the polygons have changed check if we only had one material before. 
		CachedMaterial = Brush->Polys->Element[0].Material;
		if (CachedMaterial != NULL)
		{
			for (auto Poly : Brush->Polys->Element)
			{
				if (CachedMaterial != Poly.Material)
				{
					CachedMaterial = NULL;
					break;
				}
			}
		}
	}

	// Clear existing polys.
	Brush->Polys->Element.Empty();

	const bool bUseCachedPolysMaterial = CachedPolys.Num() > 0;
	int32 CachedPolyIdx = 0;
	for (TArray<FBuilderPoly>::TIterator It(Polys); It; ++It)
	{
		if (It->Direction < 0)
		{
			for (int32 i = 0; i < It->VertexIndices.Num() / 2; i++)
			{
				Exchange(It->VertexIndices[i], It->VertexIndices.Last(i));
			}
		}

		FPoly Poly;
		Poly.Init();
		Poly.ItemName = It->ItemName;
		Poly.Base = Vertices[It->VertexIndices[0]];
		Poly.PolyFlags = It->PolyFlags;

		// Try and maintain the polygons material where possible
		Poly.Material = (bUseCachedPolysMaterial) ? CachedPolys[CachedPolyIdx++].Material : CachedMaterial;

		for (int32 j = 0; j < It->VertexIndices.Num(); j++)
		{
			new(Poly.Vertices) FVector(Vertices[It->VertexIndices[j]]);
		}
		if (Poly.Finalize(BuilderBrush, 1) == 0)
		{
			new(Brush->Polys->Element)FPoly(Poly);
		}
	}

	if (MergeCoplanars)
	{
		GEditor->bspMergeCoplanars(Brush, 0, 1);
		FBSPOps::bspValidateBrush(Brush, 1, 1);
	}
	Brush->Linked = 1;
	FBSPOps::bspValidateBrush(Brush, 0, 1);
	Brush->BuildBound();

	GEditor->RedrawLevelEditingViewports();
	GEditor->SetPivot(BuilderBrush->GetActorLocation(), false, true);

	BuilderBrush->ReregisterAllComponents();

	return true;
}

int32 UEditorBrushBuilder::GetVertexCount() const
{
	return Vertices.Num();
}

FVector UEditorBrushBuilder::GetVertex(int32 i) const
{
	return Vertices.IsValidIndex(i) ? Vertices[i] : FVector::ZeroVector;
}

int32 UEditorBrushBuilder::GetPolyCount() const
{
	return Polys.Num();
}

bool UEditorBrushBuilder::BadParameters(const FText& Msg)
{
	if (NotifyBadParams)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("Msg"), Msg);
		FNotificationInfo Info(FText::Format(LOCTEXT("BadParameters", "Bad parameters in brush builder\n{Msg}"), Arguments));
		Info.bFireAndForget = true;
		Info.ExpireDuration = Msg.IsEmpty() ? 4.0f : 6.0f;
		Info.bUseLargeFont = Msg.IsEmpty();
		
		FSlateNotificationManager::Get().AddNotification(Info);
	}
	return 0;
}

int32 UEditorBrushBuilder::Vertexv(FVector V)
{
	int32 Result = Vertices.Num();
	new(Vertices)FVector(V);

	return Result;
}

int32 UEditorBrushBuilder::Vertex3f(float X, float Y, float Z)
{
	int32 Result = Vertices.Num();
	new(Vertices)FVector(X, Y, Z);
	return Result;
}

void UEditorBrushBuilder::Poly3i(int32 Direction, int32 i, int32 j, int32 k, FName ItemName, bool bIsTwoSidedNonSolid)
{
	new(Polys)FBuilderPoly;
	Polys.Last().Direction = Direction;
	Polys.Last().ItemName = ItemName;
	new(Polys.Last().VertexIndices)int32(i);
	new(Polys.Last().VertexIndices)int32(j);
	new(Polys.Last().VertexIndices)int32(k);
	Polys.Last().PolyFlags = PF_DefaultFlags | (bIsTwoSidedNonSolid ? (PF_TwoSided | PF_NotSolid) : 0);
}

void UEditorBrushBuilder::Poly4i(int32 Direction, int32 i, int32 j, int32 k, int32 l, FName ItemName, bool bIsTwoSidedNonSolid)
{
	new(Polys)FBuilderPoly;
	Polys.Last().Direction = Direction;
	Polys.Last().ItemName = ItemName;
	new(Polys.Last().VertexIndices)int32(i);
	new(Polys.Last().VertexIndices)int32(j);
	new(Polys.Last().VertexIndices)int32(k);
	new(Polys.Last().VertexIndices)int32(l);
	Polys.Last().PolyFlags = PF_DefaultFlags | (bIsTwoSidedNonSolid ? (PF_TwoSided | PF_NotSolid) : 0);
}

void UEditorBrushBuilder::PolyBegin(int32 Direction, FName ItemName)
{
	new(Polys)FBuilderPoly;
	Polys.Last().ItemName = ItemName;
	Polys.Last().Direction = Direction;
	Polys.Last().PolyFlags = PF_DefaultFlags;
}

void UEditorBrushBuilder::Polyi(int32 i)
{
	new(Polys.Last().VertexIndices)int32(i);
}

void UEditorBrushBuilder::PolyEnd()
{
}

void UEditorBrushBuilder::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (!GIsTransacting)
	{
		// Rebuild brush on property change
		ABrush* Brush = Cast<ABrush>(GetOuter());
		UWorld* BrushWorld = Brush ? Brush->GetWorld() : nullptr;
		if (BrushWorld)
		{
			Brush->bInManipulation = PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive;
			Build(BrushWorld, Brush);
		}
	}
}

void FBSPOps::bspValidateBrush(UModel* Brush, bool ForceValidate, bool DoStatusUpdate)
{
	check(Brush != nullptr);
	Brush->Modify();
	if (ForceValidate || !Brush->Linked)
	{
		Brush->Linked = 1;
		for (int32 i = 0; i < Brush->Polys->Element.Num(); i++)
		{
			Brush->Polys->Element[i].iLink = i;
		}
		int32 n = 0;
		for (int32 i = 0; i < Brush->Polys->Element.Num(); i++)
		{
			FPoly* EdPoly = &Brush->Polys->Element[i];
			if (EdPoly->iLink == i)
			{
				for (int32 j = i + 1; j < Brush->Polys->Element.Num(); j++)
				{
					FPoly* OtherPoly = &Brush->Polys->Element[j];
					if
						(OtherPoly->iLink == j
							&& OtherPoly->Material == EdPoly->Material
							&& OtherPoly->TextureU == EdPoly->TextureU
							&& OtherPoly->TextureV == EdPoly->TextureV
							&& OtherPoly->PolyFlags == EdPoly->PolyFlags
							&& (OtherPoly->Normal | EdPoly->Normal) > 0.9999)
					{
						float Dist = FVector::PointPlaneDist(OtherPoly->Vertices[0], EdPoly->Vertices[0], EdPoly->Normal);
						if (Dist > -0.001 && Dist < 0.001)
						{
							OtherPoly->iLink = i;
							n++;
						}
					}
				}
			}
		}
		// 		UE_LOG(LogBSPOps, Log,  TEXT("BspValidateBrush linked %i of %i polys"), n, Brush->Polys->Element.Num() );
	}

	// Build bounds.
	Brush->BuildBound();
}