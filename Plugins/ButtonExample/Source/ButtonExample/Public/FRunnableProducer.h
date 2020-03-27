#pragma once
#include "FuctionObject.h"
#include "Containers/Queue.h"
#include "HAL/Runnable.h"
#include <fstream>
#include <string>
#include "Operations/Operation.h"
#include "rpi_service.h"

class FRunnableProducer : public FRunnable
{
	TQueue<Operation>* _queue;
	TQueue<Operation>* responsequeue;
	
	rpi_service::RpiService* rpi;


public:
	FRunnableThread* Thread;
	FRunnableProducer();
	virtual ~FRunnableProducer();
	virtual uint32 Run();
	virtual void Stop();
	void Kill();
};

