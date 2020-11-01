// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operations\Operation.h"

/**
 * 
 */

template<class T>
class BUTTONEXAMPLE_API LoadCreation : public Operation
{
public:
	LoadCreation(FString path);
    Response execute() override;
protected:
    FString _path;
};

template<class T>
inline Response LoadCreation<T>::execute()
{
    return Response(LoadObject<T>(nullptr, *_path));
}

template<class T>
inline LoadCreation<T>::LoadCreation(FString path)
{
    _path = path;
}



