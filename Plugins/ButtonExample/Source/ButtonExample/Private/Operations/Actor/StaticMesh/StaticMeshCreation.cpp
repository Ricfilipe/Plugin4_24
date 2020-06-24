// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticMeshCreation.h"




StaticMeshCreation::StaticMeshCreation(FVector position, FRotator rotation, FVector scl, AActor* par, UStaticMesh* msh):ActorStaticMeshCreation(position, rotation, scl, par)
{
	mesh = msh;
}

Response StaticMeshCreation::execute()
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot, pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = AStaticMeshActor::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	AStaticMeshActor* realActor = Cast<AStaticMeshActor>(newActorCreated);
	realActor->SetActorLabel("StaticMesh");
	realActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
	realActor->SetActorScale3D(objectScale);
	realActor->SetActorRotation(rot);
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	if (mat != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, mat);




	return Response(realActor);
}
