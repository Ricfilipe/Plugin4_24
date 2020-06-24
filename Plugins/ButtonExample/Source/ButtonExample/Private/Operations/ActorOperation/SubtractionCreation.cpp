// Fill out your copyright notice in the Description page of Project Settings.


#include "SubtractionCreation.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Primitive.h"

int num_subtraction = 0;

SubtractionCreation::SubtractionCreation(TArray<AActor*> acs, AActor* par, UMaterialInterface* mat) : ActorsOperationCreation(acs)
{
	material = mat;
	parent = par;
}

Response SubtractionCreation::execute()
{
	TArray<AActor*> brushes;
	AStaticMeshActor* ac = Cast<AStaticMeshActor>(actors[0]);
	brushes.Add(AuxiliarFunctions::convertMeshtoBrush(ac->GetStaticMeshComponent()->GetStaticMesh(), ac->GetActorRotation(), ac->GetActorLocation(), true));
	for (int i = 1; i < actors.Num(); i++) {
		AStaticMeshActor* ac = Cast<AStaticMeshActor>(actors[i]);
		brushes.Add(AuxiliarFunctions::convertMeshtoBrush(ac->GetStaticMeshComponent()->GetStaticMesh(), ac->GetActorRotation(), ac->GetActorLocation(), false));
	}
	AStaticMeshActor* realActor = (AStaticMeshActor*)Primitive::ConvertToStaticMesh(brushes, FString("/Game/Subtraction" +
		FString::SanitizeFloat(num_subtraction++)
	));

	if (material != NULL)
		realActor->GetStaticMeshComponent()->SetMaterial(0, material);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);


	return Response(realActor);
}
