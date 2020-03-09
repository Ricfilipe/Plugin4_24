// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builders/EditorBrushBuilder.h"
#include "KhepriPyramid.generated.h"

/**
 * 
 */
UCLASS()
class BUTTONEXAMPLE_API UKhepriPyramid : public UEditorBrushBuilder
{
	GENERATED_BODY()
public:

	UKhepriPyramid(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = BrushSettings)
		FVector top;

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
	virtual void BuildPyramid(int32 Direction, FVector top , TArray<FVector> vertices, int number);
};
