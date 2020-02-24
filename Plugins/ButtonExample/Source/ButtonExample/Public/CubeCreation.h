// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeCreation.generated.h"

UCLASS()
class BUTTONEXAMPLE_API ACubeCreation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeCreation();
	UStaticMesh* mesh;
	UStaticMesh* getMesh();


};
