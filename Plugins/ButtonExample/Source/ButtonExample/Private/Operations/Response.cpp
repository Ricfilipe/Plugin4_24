#include "Operations/Response.h"


Response::Response(AActor* resp)
{
	_resp = resp;
}

Response::Response(UMaterialInterface* resp)
{
	_mat = resp;
}

Response::Response(UStaticMesh* resp)
{
	_mesh = resp;
}

Response::Response()
{
	_resp = NULL;
}



AActor* Response::getResponse()
{
	return _resp;
}

UMaterialInterface* Response::getMat()
{
	return _mat;
}

UStaticMesh* Response::getMesh()
{
	return _mesh;
}


