
#include "FRunnableProducer.h"
#include <vector>
#include <algorithm>
#include "HAL/RunnableThread.h"


FRunnableProducer::FRunnableProducer()
{
	rpi = new rpi_service::RpiService;
	Thread = FRunnableThread::Create(this, TEXT("FProducer"), 0, TPri_BelowNormal);
}



FRunnableProducer::~FRunnableProducer()
{
	delete Thread;
	delete rpi;
	Thread = NULL;

}

size_t split(std::string &txt, std::vector<std::string> &strs, char ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}







const TCHAR* TCHARfromString(std::string str) {
	FString s(str.c_str());
	return *s;
}

const FVector FVectorfromString(std::string x, std::string y, std::string z) {
	
	return FVector(std::stof(x), std::stof(y), std::stof(z));
}

const FRotator  FRotatorfromString(std::string x, std::string y, std::string z) {

	return FRotator(std::stof(x), std::stof(y), std::stof(z));
}

uint32 FRunnableProducer::Run()
{
	rpi->Run(11002);
	
	return 0;
}

void FRunnableProducer::Stop()
{
}
