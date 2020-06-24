// Fill out your copyright notice in the Description page of Project Settings.


#include "SpotlightCreation.h"
#include "Engine/SpotLight.h"
#include "Components/SpotLightComponent.h"

Response SpotlightCreation::execute()
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(rot, pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = ASpotLight::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	ASpotLight* realActor = Cast<ASpotLight>(newActorCreated);
	realActor->SetActorLabel("Spotlight");
	realActor->SpotLightComponent->Intensity = intensity;
	realActor->SpotLightComponent->SetAttenuationRadius(range *rescale);
	realActor->SpotLightComponent->SetLightColor(color);
	realActor->SpotLightComponent->InnerConeAngle = inner_cone;
	realActor->SpotLightComponent->OuterConeAngle = outer_cone;
	realActor->SetActorScale3D(objectScale);
	realActor->SetActorRotation(rot);
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);

	return Response(realActor);
}

SpotlightCreation::SpotlightCreation(FVector position, FRotator rotation, AActor* par, FLinearColor clr, float intnsty, float rg, float inner, float outer) 
	: LightCreation(position, rotation, par, clr, intnsty, rg)
{
	inner_cone = inner;
	outer_cone = outer;
}
