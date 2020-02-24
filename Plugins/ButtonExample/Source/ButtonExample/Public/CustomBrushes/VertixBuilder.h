// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "Builders/EditorBrushBuilder.h"
#include "VertixBuilder.generated.h"

/**
 * VertixBuilder is a brush builder that receives a set of vertices to form surface and then extrudes
 */

class ABrush;

UCLASS(MinimalAPI, autoexpandcategories = BrushSettings, EditInlineNew, meta = (DisplayName = "Box"))
class  UVertixBuilder : public UEditorBrushBuilder
{
	GENERATED_BODY()

public:
	UVertixBuilder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditAnywhere, Category = BrushSettings)
		FVector NormalVector;

	UPROPERTY(EditAnywhere, Category = BrushSettings)
		TArray<FVector> DrawVertices;

	/** The size of the cube in the Y dimension */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "0.000001"))
		float Height;

	UPROPERTY()
		FName GroupName;

	UPROPERTY()
		int Size;

	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface

	//~ Begin UBrushBuilder Interface
	virtual bool Build(UWorld* InWorld, ABrush* InBrush = NULL) override;
	//~ End UBrushBuilder Interface

	// @todo document
	virtual void BuildCustom(int32 Direction, TArray<FVector> vertices,float height, int number);
};
