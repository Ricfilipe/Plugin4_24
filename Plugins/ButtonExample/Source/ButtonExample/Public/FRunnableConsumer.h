#pragma once
#include "FuctionObject.h"
#include "Containers/Queue.h"
#include "HAL/Runnable.h"

class FRunnableConsumer:public FRunnable
{
	TQueue<FuctionObject>* _queue;
	FRunnableThread* Thread;
public:
	FRunnableConsumer(TQueue<FuctionObject, EQueueMode::Spsc>* queue);
	virtual ~FRunnableConsumer();
	virtual uint32 Run();
	virtual void Stop();
	virtual void evaluate(FuctionObject fo);

	
};

