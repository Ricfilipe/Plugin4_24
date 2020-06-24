// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Operation.h"
#include "LevelSequence.h"

/**
 * 
 */



class BUTTONEXAMPLE_API RenderCreation : public Operation
{
public:
	RenderCreation(FVector pos, FRotator rot, float focal, float ape, float fcs, int wdh, int hgt, int frame, FString nm, FString pth);
	Response execute() override;

protected:

	FVector camera_pos;
	FRotator camera_rot;
	float focalLenght;
	float aperture;
	float focus;

	int frame;
	int width;
	int height;

	FString name;
	FString path;
};

namespace AuxiliarFunctions {

	ULevelSequence* CreateLevelSeq(const FString& AssetName, const FString& PackagePath, UObject* AssetToDuplicate);
}