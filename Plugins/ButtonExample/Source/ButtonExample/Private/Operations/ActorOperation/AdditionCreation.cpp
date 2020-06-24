// Fill out your copyright notice in the Description page of Project Settings.


#include "AdditionCreation.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Primitive.h"

int num_addition = 0;

AdditionCreation::AdditionCreation(TArray<AActor*> acs, AActor* par, UMaterialInterface* mat) : ActorsOperationCreation(acs)
{
	material = mat;
	parent = par;
}

Response AdditionCreation::execute()
{
	TArray<AActor*> brushes;
	for (int i = 0; i < actors.Num(); i++) {
		AStaticMeshActor* ac = Cast<AStaticMeshActor>(actors[i]);
		brushes.Add(AuxiliarFunctions::convertMeshtoBrush(ac->GetStaticMeshComponent()->GetStaticMesh(), ac->GetActorRotation(), ac->GetActorLocation(), false));
	}
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(brushes, FString("/Game/Addition" +
		FString::SanitizeFloat(num_addition++)
	));

	if (material != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, material);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);

	return Response(realActor);
}
