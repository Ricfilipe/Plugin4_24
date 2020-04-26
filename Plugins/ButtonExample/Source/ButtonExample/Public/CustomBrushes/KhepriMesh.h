// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builders/EditorBrushBuilder.h"
#include "GameFramework/Actor.h"

#include "KhepriMesh.generated.h"

/**
 * 
 */
UCLASS()
class BUTTONEXAMPLE_API UKhepriMesh : public UEditorBrushBuilder
{
	GENERATED_BODY()
public:
		UKhepriMesh(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		UPROPERTY(EditAnywhere, Category = BrushSettings)

		UStaticMesh* mesh;


		//~ Begin UBrushBuilder Interface
		virtual bool Build(UWorld* InWorld, ABrush* InBrush = NULL) override;
		//~ End UBrushBuilder Interface

private:
		TArray<FVector> vertices;

		TArray<int> indices;

		UPROPERTY()
		FName GroupName;

		//~ Begin UObject Interface
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
		//~ End UObject Interface


		// @todo document
		virtual void BuildMesh(int32 Direction, TArray<FVector> verts, TArray<int> ids, TArray<int> poly);
};
