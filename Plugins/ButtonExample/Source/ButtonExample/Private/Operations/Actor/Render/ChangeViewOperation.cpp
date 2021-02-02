// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/Actor/Render/ChangeViewOperation.h"
#include "LevelEditorViewport.h"
#include "Slate/SceneViewport.h"


ChangeViewOperation::ChangeViewOperation(FVector position, FRotator target)
{
	pos = position;
	_target = target;
}

Response ChangeViewOperation::execute()
{
	GCurrentLevelEditingViewportClient->SetViewLocation(pos);
	GCurrentLevelEditingViewportClient->SetViewRotation(_target);
	return Response();
}
