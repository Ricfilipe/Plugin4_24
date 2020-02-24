#pragma once
#include "UObject/UObjectGlobals.h"


enum typeObject
{
	cubeCreation, cylinderCreation, sphereCreation, objectCreation
};


class FuctionObject
{

public:
	typeObject _type;
	const TCHAR* _nameObject;
	FVector _position;
	FRotator _rotation;
	FVector _scale;
	const TCHAR* _texture;
	FuctionObject();
	FuctionObject(typeObject type, const TCHAR* nameObject, FVector position, FRotator rotation, FVector scale, const TCHAR* texture);

};

