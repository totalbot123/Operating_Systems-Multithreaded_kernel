#include "ivtentry.h"
#include "pcb.h"
#include "timer.h"
#include "thread.h"
#include "kerev.h"
#include <dos.h>

IVTEntry **IVTEntry::listOfIVT = new IVTEntry*[256];

IVTEntry::IVTEntry(int n, InterruptRoutine r){
	IVTEntry::listOfIVT[n] = this;
	myRoutine = r;
	oldRoutine = swapRoutine(n, myRoutine, myEvent);
}

void IVTEntry::signal(){
	myEvent->signal();
}

IVTEntry* IVTEntry::getIVT(int n){
	return IVTEntry::listOfIVT[n];
}

InterruptRoutine IVTEntry::swapRoutine(int n, InterruptRoutine routine, KernelEv *ke){
	lock

	myEvent = ke;

	unsigned int tseg = FP_SEG(routine);
	unsigned int toff = FP_OFF(routine);

	unsigned int oldSEG, oldOFF;
	InterruptRoutine oldRoutine;

	n *= 4;

	asm {
		push es
		push ax
		push bx

		mov ax,0
		mov es,ax
		mov bx, word ptr n

		mov ax, word ptr es:bx+2
		mov word ptr oldSEG, ax
		mov ax, word ptr es:bx
		mov word ptr oldOFF, ax

		mov ax, word ptr tseg
		mov word ptr es:bx+2, ax
		mov ax, word ptr toff
		mov word ptr es:bx, ax

		pop bx
		pop ax
		pop es
	}

	oldRoutine = (InterruptRoutine)MK_FP(oldSEG,oldOFF);

	unlock
	return oldRoutine;
}

void IVTEntry::oldRout(){
	(*oldRoutine)();
}

IVTEntry::~IVTEntry() {
	delete myEvent;
}

