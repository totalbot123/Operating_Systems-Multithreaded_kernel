#include "timer.h"
#include <dos.h>
#include "pcb.h"
#include "thread.h"
#include <iostream.h>
#include "SCHEDULE.H"
#include "kersem.h"

volatile Time Timer::brojac = 2;
volatile unsigned Timer::flag = 1;
volatile unsigned Timer::zahtev = 0;

unsigned psp, pss, pbp;
unsigned oldTimerSEG, oldTimerOFF;
void tick();

void interrupt timer(){
	if(!Timer::zahtev){
		asm int 60h;
		tick();
	}
	if (!Timer::zahtev){
		Timer::brojac--;
		KernelSem::updateWaitingQue();
	}

	if (Timer::brojac == 0 || Timer::zahtev) {
		if (Timer::flag){
			Timer::zahtev = 0;

#ifndef BCC_BLOCK_IGNORE

			asm{
				mov psp, sp
				mov pss, ss
				mov pbp, bp
			}

#endif
			/*Timer::flag = 0;
			cout << "Hehe xd" << endl;
			Timer::flag = 1;*/

			PCB::running->sp = psp;
			PCB::running->ss = pss;
			PCB::running->bp = pbp;

			if (PCB::running->finished != 1 && PCB::running->blocked != 1) Scheduler::put((PCB*)PCB::running);
			PCB* pom = Scheduler::get();

			if (pom){
				PCB::running = pom;
			}else{
				PCB::running = PCB::idle;
			}
			//while (PCB::running->finished) PCB::running= Scheduler::get();

			/*Timer::flag = 0;
			cout << (PCB*)PCB::running->myID << endl;
			Timer::flag = 1;*/

			psp = PCB::running->sp;
		    pss = PCB::running->ss;
		    pbp = PCB::running->bp;

		    Timer::brojac = PCB::running->time;

#ifndef BCC_BLOCK_IGNORE

			asm {
				mov sp, psp
				mov ss, pss
				mov bp, pbp
			}

#endif

		}else{
			Timer::zahtev = 1;
		}
	}
};

void inicialize(){
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
};
