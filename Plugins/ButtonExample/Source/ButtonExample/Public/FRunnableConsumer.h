#pragma once
#include "FuctionObject.h"
#include "Containers/Queue.h"
#include "HAL/Runnable.h"
#include <vector>
#include <map>
#include <memory>
#include "RPI/binary_stream.h"

class FRunnableConsumer:public FRunnable
{
	binary_stream::BinaryStream* stream;

	FRunnableThread* Thread;
public:
	FRunnableConsumer(binary_stream::BinaryStream stream);
	virtual ~FRunnableConsumer();
	virtual uint32 Run();
	virtual void Stop();


	
};

