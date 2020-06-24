// Fill out your copyright notice in the Description page of Project Settings.


#include "LightCreation.h"




LightCreation::LightCreation(FVector position, FRotator rotation, AActor* par, FLinearColor clr, float intnsty, float rg) : ActorCreation(position, rotation, FVector(0,0,0), par)
{
	color = clr;
	intensity = intnsty;
	range = rg;
}

Response LightCreation::execute()
{
	return  Response();
}
