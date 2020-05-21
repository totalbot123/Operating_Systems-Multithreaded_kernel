//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "schedule.h"
#include <stdlib.h>
#include "Thread.h"
#include "pcb.h"
#include "Idle.h"
#include "timer.h"
#include "ivtentry.h"
#include "kerev.h"
#include "event.h"



volatile int hehe=0;
int userMain (int argc, char* argv[]);


class UserMainThread: public Thread{
	int count;
	char** argu;
public:
	UserMainThread(StackSize stackSize=defaultStackSize , Time timeSlice=defaultTimeSlice, int c, char** arg );
	~UserMainThread(){waitToComplete();}
	Thread* clone() const {return new UserMainThread(defaultStackSize, defaultTimeSlice, count, argu);}
	void run(){
		hehe=userMain(count,argu);
	}

};

UserMainThread::UserMainThread(StackSize stackSize , Time timeSlice, int c, char** arg ):Thread(stackSize,timeSlice){
	count = c;
	argu = arg;
}

int main(int argc, char* argv[]){
	inicialize();

  	lock
	PCB::running = new PCB(20);
	new Idle(4096, 2);
	UserMainThread* user=new UserMainThread(defaultStackSize, defaultTimeSlice, argc, argv);
	user->start();

	unlock
	delete user;

	restore();

	cout << "Kraj main-a" << endl;

	return hehe;
}
