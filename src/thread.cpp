#include "thread.h"
#include <dos.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "timer.h"
#include "pcb.h"

unsigned num;
unsigned pom;
unsigned pomBP;
unsigned pomPCOFF;
unsigned pomPCSEG;
unsigned pomDS;
unsigned pomDI;
unsigned pomSI;
unsigned BPTest;
Thread *t;

void Thread::start(){
	Scheduler::put(myPCB);
}

void Thread::waitToComplete(){
	lock
	if (!this->myPCB->finished && this->myPCB != PCB::running && this->myPCB != PCB::idle){
		PCB::running->blocked = 1;
		PCB::put((PCB*)PCB::running, this->myPCB->blockedOnes);
		dispatch();
	}
	unlock
}

Thread::~Thread(){
	delete myPCB;
};

Thread::Thread(StackSize stackSize, Time timeSlice){
	myPCB = new PCB(stackSize, timeSlice, this);
};

ID Thread::getId(){
	return myPCB->myID;
}

ID Thread::getRunningId(){
	return PCB::running->myID;
}

Thread * Thread::getThreadById(ID id){
	volatile PCB* pom = PCB::get(id, PCB::listOfAll);
	return pom->myThread;
}

ID Thread::fork(){
	asm{
		mov pom, sp
		mov pomBP, bp
	}
	num = (pom - FP_OFF(PCB::running->stack))/2;

	if (num < 6) return -1;

	t = PCB::running->myThread->clone();
	if (t == NULL) return -1;

	if(FP_OFF(t->myPCB->stack) != FP_OFF(PCB::running->stack)){
		while (BPTest < FP_OFF(PCB::running->stack + PCB::running->size -1)){
			t->myPCB->stack[(BPTest - FP_OFF(PCB::running->stack))/2] += FP_OFF(t->myPCB->stack) - FP_OFF(PCB::running->stack);
			BPTest = *((unsigned*)MK_FP(PCB::running->ss, BPTest));
		}
	}

	PCB::running->numberOfChildren++;
	t->myPCB->father = (PCB*)PCB::running;
	t->myPCB->time = t->myPCB->father->time;

	for(int i = num; i < PCB::running->size;i++){
		t->myPCB->stack[i] = PCB::running->stack[i];
	}

	pomDS = PCB::running->stack[num];
	pomDI = PCB::running->stack[num+1];
	pomSI = PCB::running->stack[num+2];
	pomBP = PCB::running->stack[num+3];
	pomPCOFF = PCB::running->stack[num+4];
	pomPCSEG = PCB::running->stack[num+5];
	/*pomSI = PCB::running->stack[num+1];
	pomBP = PCB::running->stack[num+2];
	pomPCOFF = PCB::running->stack[num+3];
	pomPCSEG = PCB::running->stack[num+4];*/

	asm{
		pushf
		pop pom
	}
	t->myPCB->stack[num+5] = pom;
	t->myPCB->stack[num+4] = pomPCSEG;
	t->myPCB->stack[num+3] = pomPCOFF;
	t->myPCB->stack[num+2] = 0;
	asm mov pom, bx;
	t->myPCB->stack[num+1] = pom;
	asm mov pom, cx;
	t->myPCB->stack[num] = pom;
	asm mov pom, dx;
	t->myPCB->stack[num-1] = pom;
	asm mov pom, es;
	t->myPCB->stack[num-2] = pom;
	t->myPCB->stack[num-3] = pomDS;
	t->myPCB->stack[num-4] = pomSI;
	//asm mov pom, di;
	t->myPCB->stack[num-5] = pomDI;
	t->myPCB->stack[num-6] = pomBP;
	t->myPCB->sp = FP_OFF(t->myPCB->stack + num-6);
	t->myPCB->ss = FP_SEG(t->myPCB->stack + num-6);
	t->myPCB->bp = FP_OFF(t->myPCB->stack + num-6);
	Scheduler::put(t->myPCB);
	return t->myPCB->myID;
}

void Thread::waitForForkChildren(){
	lock
	if (PCB::running->numberOfChildren){
		PCB::running->blocked = 1;
		dispatch();
	}
	unlock
}

void Thread::exit(){
	lock

	PCB::running->finished = 1;

	if (PCB::running->father != NULL && !PCB::running->father->finished){
		if (--PCB::running->father->numberOfChildren == 0){
			PCB::running->father->blocked = 0;
			Scheduler::put(PCB::running->father);
		}
	}

	while (PCB::running->blockedOnes){
		volatile PCB* pcb = PCB::get((elem*&)PCB::running->blockedOnes);
		pcb->blocked = 0;
		if (!PCB::running->father->finished) Scheduler::put((PCB*)pcb);
	}

	PCB* dispose = PCB::get(PCB::running->myID, PCB::listOfAll);
	dispatch();

	unlock
}

Thread* Thread::clone() const{
	return NULL;
}

//#ifndef BCC_BLOCK_IGNORE
void dispatch(){
	lock
	Timer::zahtev = 1;
	timer();
	unlock
}
//#endif
