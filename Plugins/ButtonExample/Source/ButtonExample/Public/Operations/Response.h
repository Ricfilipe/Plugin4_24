#pragma once
#include "GameFramework/Actor.h"

class Response
{
private:
	AActor* _resp;
public:
	Response(AActor* resp);
	Response();
	AActor* getResponse();
};

