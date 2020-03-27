
#include "FRunnableConsumer.h"
#include "Primitive.h"
#include "HAL/RunnableThread.h"

FRunnableConsumer::FRunnableConsumer(binary_stream::BinaryStream sstream) {
	stream = &sstream;
}

FRunnableConsumer::~FRunnableConsumer() {
	delete Thread;
	Thread = NULL;
}

uint32 FRunnableConsumer::Run() {

	return 0;
}

void FRunnableConsumer::Stop() {
	
}



