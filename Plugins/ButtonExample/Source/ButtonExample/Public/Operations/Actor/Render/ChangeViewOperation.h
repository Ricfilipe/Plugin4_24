// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Actor\ActorCreation.h"

/**
 * 
 */
class BUTTONEXAMPLE_API ChangeViewOperation : public ActorCreation
{
public:
	ChangeViewOperation(FVector position, FRotator target);
	Response execute() override;

protected:
	FRotator _target;
};
