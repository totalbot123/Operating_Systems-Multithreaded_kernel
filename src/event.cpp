#include "event.h"
#include "kerev.h"
#include "ivtentry.h"

Event::Event(IVTNo ivtNo){
	myImpl = new KernelEv(ivtNo);
	IVTEntry::getIVT(ivtNo)->oldRoutine = IVTEntry::getIVT(ivtNo)->swapRoutine(ivtNo, IVTEntry::getIVT(ivtNo)->myRoutine, myImpl);
}

Event::~Event(){
	delete myImpl;
}

void Event::wait(){
	myImpl->wait();
}

void Event::signal(){
	myImpl->signal();
}
