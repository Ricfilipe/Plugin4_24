// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/ActorOperation/DeleteCreation.h"

#include "EditorModeManager.h"


DeleteCreation::DeleteCreation(TArray<AActor*> acs): ActorsOperationCreation(acs)
{
}

Response DeleteCreation::execute()
{
	UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	for (int i = 0; i < actors.Num(); i++) {

		currentWorld->EditorDestroyActor(actors[i], true);
	}
	actors.Empty();

	currentWorld->CleanupActors();
	GEditor->EditorUpdateComponents();
	currentWorld->UpdateWorldComponents(true, false);
	currentWorld->ForceGarbageCollection(true);
	GLevelEditorModeTools().MapChangeNotify();
	return Response();
}
