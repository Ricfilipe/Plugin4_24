
#include "FuctionObject.h"



FuctionObject::FuctionObject()
{
	_nameObject = TEXT("cube");
	_position = FVector(0, 0, 0);
	_rotation = FRotator(0, 0, 0);
	_scale = FVector(1, 1, 1);

}

FuctionObject::FuctionObject(typeObject type,const TCHAR* nameObject, FVector position, FRotator rotation, FVector scale, const TCHAR* texture)
{		
	_type = type;
	_nameObject = nameObject;
	_position = position;
	_rotation = rotation;
	_scale = scale;
	_texture = texture;
}



