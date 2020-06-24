// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorCreation.h"


ActorCreation::ActorCreation(FVector position, FRotator rotation, FVector scl, AActor* par)
{
	pos = position;
	rot = rotation;
	scale = scl;
	parent = par;
}

Response ActorCreation::execute()
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot, pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AActor::StaticClass();

	AActor* realActor = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);
	realActor->SetActorLabel("StaticMesh");
	realActor->SetActorScale3D(objectScale);
	return Response(realActor);
}
