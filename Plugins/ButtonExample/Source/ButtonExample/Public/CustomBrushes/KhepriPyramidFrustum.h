// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builders/EditorBrushBuilder.h"
#include "KhepriPyramidFrustum.generated.h"

/**
 * 
 */
UCLASS()
class BUTTONEXAMPLE_API UKhepriPyramidFrustum : public UEditorBrushBuilder
{
	GENERATED_BODY()

public:
	UKhepriPyramidFrustum(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = BrushSettings)
		TArray<FVector> top;

	UPROPERTY(EditAnywhere, Category = BrushSettings)
		TArray<FVector> base;


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
	virtual void BuildPyramidFrustum(int32 Direction, TArray<FVector>  head , TArray<FVector> vertices, int number);
		
};
