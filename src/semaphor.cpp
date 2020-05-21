#include "semaphor.h"
#include "kersem.h"
#include "thread.h"
#include "timer.h"
#include "pcb.h"
#include <iostream.h>
#include <stdlib.h>
#include "SCHEDULE.H"

Semaphore::Semaphore (int init){
	myImpl = new KernelSem(init);
}

Semaphore::~Semaphore (){
	delete myImpl;
}

int Semaphore::val () const{
	return myImpl->value;
}

int Semaphore::wait (Time maxTimeToWait){
	return myImpl->wait(maxTimeToWait);
}

void Semaphore::signal(){
	myImpl->signal();
}
