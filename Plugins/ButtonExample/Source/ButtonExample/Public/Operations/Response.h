#pragma once
#include "GameFramework/Actor.h"
#include "Materials/Material.h"

class Response
{
private:
	AActor* _resp;
	UMaterial* _mat;
	UStaticMesh* _mesh;
public:
	Response(AActor* resp);
	Response(UMaterial* resp);
	Response(UStaticMesh* resp);
	Response();
	AActor* getResponse();
	UMaterial* getMat();
	UStaticMesh* getMesh();
};

