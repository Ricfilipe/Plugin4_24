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
#include "CustomBrushes/KhepriCylinder.h"
#include "CustomBrushes/KhepriBox.h"
#include "CustomBrushes/KhepriRightCuboid.h"
#include "CustomBrushes/KhepriPyramid.h"
#include "CustomBrushes/KhepriPyramidFrustum.h"


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


UKhepriCylinder::UKhepriCylinder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName NAME_Cylinder;
		FConstructorStatics()
			: NAME_Cylinder(TEXT("Cylinder"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	Height = 200.0f;
	Radius = 200.0f;
	Sides = 8;
	GroupName = ConstructorStatics.NAME_Cylinder;
	BitmapFilename = TEXT("Btn_Cylinder");
	ToolTip = TEXT("BrushBuilderName_Cylinder");
}

void UKhepriCylinder::BuildCylinder(int32 Direction, int32 InSides, float InZ, float Radius)
{
	int32 n = GetVertexCount();
	int32 Ofs = 0;


	// Vertices.
	for (int32 i = 0; i < InSides; i++)
		for (int32 j = 0; j < 2; j += 1)
			Vertex3f(Radius * FMath::Sin((2 * i + Ofs) * PI / InSides), Radius * FMath::Cos((2 * i + Ofs) * PI / InSides), j * InZ);

	// Polys.
	for (int32 i = 0; i < InSides; i++)
		Poly4i(Direction, n + i * 2, n + i * 2 + 1, n + ((i * 2 + 3) % (2 * InSides)), n + ((i * 2 + 2) % (2 * InSides)), FName(TEXT("Wall")));
}

void UKhepriCylinder::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		static FName Name_OuterRadius(GET_MEMBER_NAME_CHECKED(UKhepriCylinder, Radius));


	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UKhepriCylinder::Build(UWorld* InWorld, ABrush* InBrush)
{
	if (Sides < 3)
		return BadParameters(LOCTEXT("CylinderInvalidSides", "Not enough cylinder sides."));
	if (Height <= 0 || Radius <= 0)
		return BadParameters(LOCTEXT("CylinderInvalidRadius", "Invalid cylinder radius"));


	BeginBrush(false, GroupName);
	BuildCylinder(+1, Sides, Height, Radius);

	for (int32 j = -1; j < 2; j += 2)
	{
		PolyBegin(j, FName(TEXT("Cap")));
		for (int32 i = 0; i < Sides; i++)
			Polyi(i * 2 + (1 - j) / 2);
		PolyEnd();
	}

	return EndBrush(InWorld, InBrush);
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

UKhepriRightCuboid::UKhepriRightCuboid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName NAME_Cube;
		FConstructorStatics()
			: NAME_Cube(TEXT("Box"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	X = 200.0f;
	Y = 200.0f;
	Z = 200.0;
	GroupName = ConstructorStatics.NAME_Cube;
	BitmapFilename = TEXT("Btn_Box");
	ToolTip = TEXT("BrushBuilderName_Cube");
}

void UKhepriRightCuboid::BuildCube(int32 Direction, float dx, float dy, float dz) {
	int32 n = GetVertexCount();

	for (int32 i = -1; i < 2; i += 2)
		for (int32 j = -1; j < 2; j += 2)
			for (int32 k = 0; k < 2; k += 1)
				Vertex3f(i * dx / 2, j * dy / 2, k * dz );

	Poly4i(Direction, n + 0, n + 1, n + 3, n + 2);
	Poly4i(Direction, n + 2, n + 3, n + 7, n + 6);
	Poly4i(Direction, n + 6, n + 7, n + 5, n + 4);
	Poly4i(Direction, n + 4, n + 5, n + 1, n + 0);
	Poly4i(Direction, n + 3, n + 1, n + 5, n + 7);
	Poly4i(Direction, n + 0, n + 2, n + 6, n + 4);
}

bool UKhepriRightCuboid::Build(UWorld* InWorld, ABrush* InBrush)
{
	if (Z <= 0 || Y <= 0 || X <= 0)
		return BadParameters(LOCTEXT("CubeInvalidDimensions", "Invalid cube dimensions"));


	BeginBrush(false, GroupName);
	BuildCube(+1, X, Y, Z);

	return EndBrush(InWorld, InBrush);
}

void UKhepriRightCuboid::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		static FName Name_X(GET_MEMBER_NAME_CHECKED(UKhepriBox, X));
		static FName Name_Y(GET_MEMBER_NAME_CHECKED(UKhepriBox, Y));
		static FName Name_Z(GET_MEMBER_NAME_CHECKED(UKhepriBox, Z));


	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}


UKhepriBox::UKhepriBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName NAME_Cube;
		FConstructorStatics()
			: NAME_Cube(TEXT("RightCuboid"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	X = 200.0f;
	Y = 200.0f;
	Z = 200.0;
	GroupName = ConstructorStatics.NAME_Cube;
	BitmapFilename = TEXT("Btn_Box");
	ToolTip = TEXT("BrushBuilderName_Cube");
}

void UKhepriBox::BuildCube(int32 Direction, float dx, float dy, float dz) {
	int32 n = GetVertexCount();

	for (int32 i = -1; i < 1; i += 1)
		for (int32 j = -1; j < 1; j += 1)
			for (int32 k = 0; k < 2; k += 1)
				Vertex3f(i * dx , j * dy , k * dz);

	Poly4i(Direction, n + 0, n + 1, n + 3, n + 2);
	Poly4i(Direction, n + 2, n + 3, n + 7, n + 6);
	Poly4i(Direction, n + 6, n + 7, n + 5, n + 4);
	Poly4i(Direction, n + 4, n + 5, n + 1, n + 0);
	Poly4i(Direction, n + 3, n + 1, n + 5, n + 7);
	Poly4i(Direction, n + 0, n + 2, n + 6, n + 4);
}

bool UKhepriBox::Build(UWorld* InWorld, ABrush* InBrush)
{
	if (Z <= 0 || Y <= 0 || X <= 0)
		return BadParameters(LOCTEXT("CubeInvalidDimensions", "Invalid cube dimensions"));


	BeginBrush(false, GroupName);
	BuildCube(+1, X, Y, Z);

	return EndBrush(InWorld, InBrush);
}

void UKhepriBox::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		static FName Name_X(GET_MEMBER_NAME_CHECKED(UKhepriBox, X));
		static FName Name_Y(GET_MEMBER_NAME_CHECKED(UKhepriBox, Y));
		static FName Name_Z(GET_MEMBER_NAME_CHECKED(UKhepriBox, Z));


	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UKhepriPyramid::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	if (PropertyChangedEvent.Property)
	{



	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UKhepriPyramid::Build(UWorld* InWorld, ABrush* InBrush) {

	if(base.Num() <= 2 )
		return BadParameters(LOCTEXT("PyramidInvalid", "Invalid number of vertices"));

		BeginBrush(false, GroupName);
	BuildPyramid(+1, top, base,Size);

	PolyBegin(1, FName(TEXT("Cap")));
	for (int j = Size-1 ; j >= 0 ; j--)
		Polyi(j);
	PolyEnd();

	return EndBrush(InWorld, InBrush);
}

void UKhepriPyramid::BuildPyramid(int32 Direction, FVector top, TArray<FVector> vertices, int number){
	for (int j = 0; j < number; j++) {
		Vertex3f(vertices[j].X , vertices[j].Y, vertices[j].Z);
	}
	Vertex3f(top.X, top.Y, top.Z);

	for (int i = 0; i < number; i++) {
		Poly3i(Direction, number, i,(i + 1) % number);
	}
}

UKhepriPyramid::UKhepriPyramid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName NAME_Pyramid;
		FConstructorStatics()
			: NAME_Pyramid(TEXT("Pyramid"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	
	TArray<FVector> cap;
	top = FVector(0, 0, 100);
	cap.Add(FVector(100, 0, 0));
	cap.Add(FVector(0, -100,0 ));
	cap.Add(FVector(-100, 0, 0));
	cap.Add(FVector(0, 100, 0));
	Size = cap.Num();
	base = cap;

	GroupName = ConstructorStatics.NAME_Pyramid;
	BitmapFilename = TEXT("Btn_Pyramid");
	ToolTip = TEXT("BrushBuilderName_Pyramid");
}



void UKhepriPyramidFrustum::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	if (PropertyChangedEvent.Property)
	{



	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UKhepriPyramidFrustum::Build(UWorld* InWorld, ABrush* InBrush) {

	if (base.Num() <= 2 && base.Num() == top.Num())
		return BadParameters(LOCTEXT("PyramidInvalid", "Invalid number of vertices"));

	BeginBrush(false, GroupName);
	BuildPyramidFrustum(+1, top, base, Size);

	PolyBegin(1, FName(TEXT("Cap")));
	for (int j = 0; j < Size * 2; j = j + 2)
		Polyi(j);
	PolyEnd();
	PolyBegin(-1, FName(TEXT("Cap")));
	for (int j = 0; j < Size * 2; j = j + 2)
		Polyi(j);
	PolyEnd();


	PolyBegin(1, FName(TEXT("Cap")));
	for (int j = Size * 2 - 1; j > 0; j = j - 2)
		Polyi(j);
	PolyEnd();

	PolyBegin(-1, FName(TEXT("Cap")));
	for (int j = Size * 2 - 1; j > 0; j = j - 2)
		Polyi(j);
	PolyEnd();

	return EndBrush(InWorld, InBrush);
}

void UKhepriPyramidFrustum::BuildPyramidFrustum(int32 Direction, TArray<FVector>  head, TArray<FVector> vertices, int number){
	for (int j = number-1; j >= 0; j--) {
		Vertex3f(vertices[j].X, vertices[j].Y, vertices[j].Z);
		Vertex3f(head[j].X, head[j].Y, head[j].Z);
	}
	

	for (int32 i = 0; i < number; i++) {
		Poly4i(-Direction, i * 2, i * 2 + 1, ((i * 2 + 3) % (2 * number)), ((i * 2 + 2) % (2 * number)), FName(TEXT("Wall")));
		Poly4i(Direction, i * 2, i * 2 + 1, ((i * 2 + 3) % (2 * number)), ((i * 2 + 2) % (2 * number)), FName(TEXT("Wall")));
	}
}

UKhepriPyramidFrustum::UKhepriPyramidFrustum(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName NAME_Pyramid;
		FConstructorStatics()
			: NAME_Pyramid(TEXT("PyramidFrustum"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;


	TArray<FVector> cap;
	TArray<FVector> head;
	head.Add(FVector(50, 0, 50));
	head.Add(FVector(0, -50, 50));
	head.Add(FVector(-50, 0, 50));
	head.Add(FVector(0, 50, 50));
	cap.Add(FVector(100, 0, 0));
	cap.Add(FVector(0, -100, 0));
	cap.Add(FVector(-100, 0, 0));
	cap.Add(FVector(0, 100, 0));
	Size = cap.Num();
	base = cap;
	top = head;
	GroupName = ConstructorStatics.NAME_Pyramid;
	BitmapFilename = TEXT("Btn_Pyramid");
	ToolTip = TEXT("BrushBuilderName_Pyramid");
}