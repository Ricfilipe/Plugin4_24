#pragma once
#include "GameFramework/Actor.h"
#include "Materials/Material.h"

class Response
{
private:
	AActor* _resp;
	UMaterialInterface* _mat;
	UStaticMesh* _mesh;
public:
	Response(AActor* resp);
	Response(UMaterialInterface* resp);
	Response(UStaticMesh* resp);
	Response();
	AActor* getResponse();
	UMaterialInterface* getMat();
	UStaticMesh* getMesh();
};

