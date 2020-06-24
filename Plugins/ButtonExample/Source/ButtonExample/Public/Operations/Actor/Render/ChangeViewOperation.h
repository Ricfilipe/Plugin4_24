// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Actor\ActorCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API ChangeViewOperation : public ActorCreation
{
public:
	ChangeViewOperation(FVector position, FVector target);
	Response execute() override;

protected:
	FVector _target;
};
