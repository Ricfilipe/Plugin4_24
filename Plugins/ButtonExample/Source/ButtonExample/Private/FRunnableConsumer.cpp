
#include "FRunnableConsumer.h"
#include "Primitive.h"
#include "HAL/RunnableThread.h"

FRunnableConsumer::FRunnableConsumer(TQueue<FuctionObject>* queue) {
	_queue = queue;
	Thread = FRunnableThread::Create(this, TEXT("FRunnableConsumer"), 0, TPri_BelowNormal);
}

FRunnableConsumer::~FRunnableConsumer() {
	delete Thread;
	Thread = NULL;
}

uint32 FRunnableConsumer::Run() {
	while (true) {
		if (_queue->IsEmpty()) {
			FPlatformProcess::Sleep(0.1f);
		}
		else {
			FuctionObject fo;
			_queue->Dequeue(fo);
			evaluate(fo);
		}
	}
	return 0;
}

void FRunnableConsumer::Stop() {
	
}

void FRunnableConsumer::evaluate(FuctionObject fo)
{

}


