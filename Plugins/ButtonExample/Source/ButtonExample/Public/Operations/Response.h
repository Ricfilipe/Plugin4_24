#pragma once
#include "GameFramework/Actor.h"
#include "Materials/Material.h"

class BUTTONEXAMPLE_API Response
{
private:
	UObject* _resp;
public:
	Response(UObject* resp);
	Response();
	template <class T>
	T* getResponse();

};

template<class T>
inline T* Response::getResponse()
{
	return Cast<T>(_resp);
}
