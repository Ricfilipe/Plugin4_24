#pragma once

#include "CoreMinimal.h"
#include "Builders/EditorBrushBuilder.h"
#include "KhepriCylinder.generated.h"

/**
 *
 */
UCLASS()
class UKhepriCylinder : public UEditorBrushBuilder
{
	GENERATED_BODY()
public:
	UKhepriCylinder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Height of cylinder */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "0.000001"))
		float Height;

	/** Radius of cylinder */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "0.000001"))
		float Radius;


	/** How many sides this cylinder should have */
	UPROPERTY(EditAnywhere, Category = BrushSettings, meta = (ClampMin = "3", ClampMax = "500"))
		int32 Sides;

	UPROPERTY()
		FName GroupName;


	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface

	//~ Begin UBrushBuilder Interface
	virtual bool Build(UWorld* InWorld, ABrush* InBrush = NULL) override;
	//~ End UBrushBuilder Interface

	// @todo document
	virtual void BuildCylinder(int32 Direction, int32 InSides, float InZ, float Radius);
};