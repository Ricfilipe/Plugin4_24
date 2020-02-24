#include "Operations/Response.h"


Response::Response(AActor* resp)
{
	_resp = resp;
}

Response::Response()
{
	_resp = NULL;
}



AActor* Response::getResponse()
{
	return _resp;
}


