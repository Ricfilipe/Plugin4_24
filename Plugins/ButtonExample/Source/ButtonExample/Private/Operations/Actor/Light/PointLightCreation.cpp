// Fill out your copyright notice in the Description page of Project Settings.


#include "PointLightCreation.h"
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"



PointLightCreation::PointLightCreation(FVector position, AActor* par, FLinearColor clr, float intnsty, float rg) : LightCreation(position, FRotator(0,0,0), par,clr,intnsty,rg)
{

}

Response PointLightCreation::execute()
{
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(FRotator(0, 0, 0), pos, objectScale);
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	ULevel* currentLevel = currentWorld->GetCurrentLevel();
	UClass* staticMeshClass = APointLight::StaticClass();

	AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

	APointLight* realActor = Cast<APointLight>(newActorCreated);
	realActor->SetActorLabel("PointLight");
	realActor->PointLightComponent->Intensity = intensity;
	realActor->PointLightComponent->SetAttenuationRadius(range * rescale);
	realActor->PointLightComponent->SetLightColor(color);
	realActor->SetActorScale3D(objectScale);
	realActor->SetActorRotation(FRotator(0, 0, 0));
	realActor->SetActorLocation(pos);
	if (parent != NULL)
		realActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);





	return Response( realActor);
}


