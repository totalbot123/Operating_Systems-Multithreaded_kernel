#include "pcb.h"
#include "thread.h"
#include "timer.h"
#include <dos.h>
#include <iostream.h>
#include "SCHEDULE.H"

ID PCB::pomID = 0;
volatile PCB* PCB::running = NULL;
volatile PCB* PCB::idle = NULL;

elem *PCB::listOfAll;

void PCB::wrapper(){
	PCB::running->myThread->run();
	lock

	PCB::running->finished = 1;
	PCB* disposable = PCB::get(PCB::running->myID, PCB::listOfAll);

	if (PCB::running->father != NULL){
		if (--PCB::running->father->numberOfChildren == 0){
			PCB::running->father->blocked = 0;
			Scheduler::put(PCB::running->father);
		}
	}

	while (PCB::running->blockedOnes){
		volatile PCB* pcb = PCB::get((elem*&)PCB::running->blockedOnes);
		pcb->blocked = 0;
		Scheduler::put((PCB*)pcb);
	}

	dispatch();
	unlock
}


PCB::PCB(StackSize size, Time tiem, Thread *t){
	this->size = size;
	stack = new unsigned[size];
	stack[size - 1] = 0x200;
	stack[size - 2] = FP_SEG(PCB::wrapper);
	stack[size - 3] = FP_OFF(PCB::wrapper);
	ss = FP_SEG(stack + size - 12);
	sp = FP_OFF(stack + size - 12);
	bp = FP_OFF(stack + size - 12);
	myThread = t;
	signalized = 0;
	numberOfChildren = 0;
	father = NULL;
	tiem == 0 ? time = -1 : time = tiem;
	if (PCB::idle == NULL && this != PCB::running){
		PCB::idle = this;
		blocked = 1;
		finished = 1;
		myID = 0;
	}else{
		finished = 0;
		blocked = 0;
		myID = ++pomID;
		PCB::put(this, listOfAll);
	}
	blockedOnes = NULL;
}

PCB::PCB(Time tiem){
	time = tiem;
	myID = ++pomID;
	finished = 0;
	blocked =0;
	PCB::put(this, listOfAll);
}

void PCB::put(PCB* pcb, elem *&list){
	lock
	volatile elem *tek = list;
	if (!list){
		list = new elem;
		list->pod = pcb;
		list->sled = NULL;
	}else{
		while (tek->sled){
			tek = tek->sled;
		}
		tek->sled = new elem;
		tek = tek->sled;
		tek->pod = pcb;
		tek->sled = NULL;
	}
	unlock
}

PCB* PCB::get(int ID, elem *&list){
	elem *preth  = NULL, *tek = list;
	PCB *pom;

	lock

	while (tek->pod->myID != ID && tek != NULL){
		preth = tek;
		tek = tek->sled;
	}

	if (tek){
		if (!preth){
			list = tek->sled;
		}else{
			preth->sled = tek->sled;
		}
		pom = tek->pod;
		delete tek;
		return pom;
	}

	unlock

	return NULL;
}

PCB* PCB::get(elem *&list){
	lock
	PCB* pom;
	pom = list->pod;
	elem * stari = list;
	list=list->sled;
	delete stari;
	unlock
	return pom;
}

PCB::~PCB() {
	if (stack){
		delete[] stack;
	}
}

elem::elem(){};

elem::~elem(){};

