// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builders/EditorBrushBuilder.h"
#include "KhepriBox.generated.h"

/**
 * 
 */
UCLASS()
class BUTTONEXAMPLE_API UKhepriBox : public UEditorBrushBuilder
{
public:
	GENERATED_BODY()
public:
	UKhepriBox(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The size of the cube in the X dimension */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "0.000001"))
		float X;

	/** The size of the cube in the Y dimension */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "0.000001"))
		float Y;

	/** The size of the cube in the Z dimension */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "0.000001"))
		float Z;


	UPROPERTY()
		FName GroupName;




	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface

	//~ Begin UBrushBuilder Interface
	virtual bool Build(UWorld* InWorld, ABrush* InBrush = NULL) override;
	//~ End UBrushBuilder Interface

	// @todo document
	virtual void BuildCube(int32 Direction, float dx, float dy, float dz);
	
};
