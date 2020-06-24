// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeViewOperation.h"
#include "LevelEditorViewport.h"
#include "Slate/SceneViewport.h"


ChangeViewOperation::ChangeViewOperation(FVector position, FVector target)
{
	pos = position;
	_target = target;
}

Response ChangeViewOperation::execute()
{
	GCurrentLevelEditingViewportClient->SetViewLocation(pos);
	GCurrentLevelEditingViewportClient->SetLookAtLocation(_target);
	return Response();
}
