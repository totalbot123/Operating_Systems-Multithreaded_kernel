#include "kerev.h"
#include "pcb.h"
#include "thread.h"
#include "timer.h"
#include "ivtentry.h"
#include "SCHEDULE.H"

KernelEv::KernelEv(int n){
	myPCB = (PCB*)PCB::running;
	entry = n;
}

KernelEv::~KernelEv() {
	InterruptRoutine firstOne = IVTEntry::getIVT(entry)->swapRoutine(entry, IVTEntry::getIVT(entry)->oldRoutine, this);
}

void KernelEv::signal(){
	lock
	if (myPCB->blocked){
		myPCB->blocked = 0;
		Scheduler::put(myPCB);
		dispatch();
	}
	unlock
}

void KernelEv::wait(){
	lock
	if (!myPCB->blocked && myPCB == (PCB*)PCB::running){
		myPCB->blocked = 1;
		dispatch();
	}
	unlock
}
