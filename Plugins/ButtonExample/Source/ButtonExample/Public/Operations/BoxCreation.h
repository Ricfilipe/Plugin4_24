#pragma once
#include "Operations\Operation.h"


class  BoxCreation : public Operation
{


public:
	// Sets default values for this actor's properties
	BoxCreation() {}
	 Response execute(UPackage* Package) override;


};