#include "kersem.h"
#include "pcb.h"
#include "timer.h"
#include <dos.h>
#include <iostream.h>
#include "thread.h"
#include "SCHEDULE.H"

elem *KernelSem::waitingQue = NULL;

KernelSem::KernelSem(int v) {
	value = v;
	myQue = NULL;
}

int KernelSem::wait(unsigned maxTimeToWait){
	lock
	if (--value < 0){
		PCB::running->blocked = 1;
		PCB::running->mySem = this;
		PCB::put((PCB*)PCB::running, myQue);
		if (maxTimeToWait != 0){
			PCB::running->blockedTime = maxTimeToWait;
			KernelSem::putPriTime((PCB*)PCB::running, (elem*&)KernelSem::waitingQue);
			dispatch();
			if (PCB::running->signalized == 1){
				PCB::running->signalized = 0;
				unlock
				return 1;
			}else{
				unlock
				return 0;
			}
		}
		dispatch();
		unlock
		return 1;
	}
	unlock
	return 0;
}

void KernelSem::signal(){
	lock
	if (value++ < 0){
		volatile PCB* pom = PCB::get(myQue);
		volatile PCB* tester;
		int id = pom->myID;
		tester = KernelSem::updateSignalQue(id, KernelSem::waitingQue);
		pom->blocked = 0;
		pom->signalized = 1;
		Scheduler::put((PCB*)pom);
	}
	unlock
}

void KernelSem::putPriTime(PCB* pcb, elem* &list){
	lock
	if (list == NULL){
		list = new elem();
		list->pod = pcb;
		list->sled = NULL;
	}else{
		elem *tek = new elem();
		if (pcb->blockedTime < list->pod->blockedTime){
			list->pod->blockedTime = list->pod->blockedTime - pcb->blockedTime;
			tek->pod = pcb;
			tek->sled = list;
			list = tek;
		}else{
			elem* pom = list;
			elem* preth = NULL;
			int n = 0;
			while (pom || pcb->blockedTime > n + pom->pod->blockedTime){
				n += pom->pod->blockedTime;
				preth = pom;
				pom = pom->sled;
			}
			pcb->blockedTime = pcb->blockedTime - n;
			tek->pod = pcb;
			tek->sled = pom;
			preth->sled = tek;
		}
	}
	unlock
	return;
}

void KernelSem::updateWaitingQue(){

	PCB* pom;

	lock

	if (KernelSem::waitingQue){
		KernelSem::waitingQue->pod->blockedTime--;
		while (KernelSem::waitingQue->pod->blockedTime<=0){
			pom = PCB::get(KernelSem::waitingQue);
			int id = pom->myID;
			pom = PCB::get(id, (elem*&)pom->mySem->myQue);
			pom->blocked = 0;
			pom->mySem->value++;
			pom->signalized = 0;
			Scheduler::put((PCB*)pom);
		}
	}
	unlock
}

PCB* KernelSem::updateSignalQue(int ID, elem *&list){
	elem *preth  = NULL, *tek = list, *sled  = tek->sled;
	PCB *pom;

	lock

	while (tek->pod->myID != ID && tek != NULL){
		preth = tek;
		tek = tek->sled;
		sled = tek->sled;
	}

	if (tek){
		if (!preth){
			list = sled;
		}else{
			preth->sled = tek->sled;
		}
		sled->pod->blockedTime += tek->pod->blockedTime;
		pom = tek->pod;
		delete tek;
		return pom;
	}
	unlock
	return NULL;
}

KernelSem::~KernelSem() {
	delete myQue;
}
