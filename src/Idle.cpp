/*
 * Idle.cpp
 *
 *  Created on: May 31, 2016
 *      Author: OS1
 */

#include "Idle.h"
#include <iostream.h>
#include "thread.h"
#include "timer.h"
#include "SCHEDULE.h"
#include "PCB.h"
#include "kersem.h"


Idle::Idle(StackSize size, int time) : Thread(size, time){ }

void Idle::run(){
	while(1){/*
		cout << "Status: " << endl;
				elem* tek = PCB::listOfAll;
				while (tek){
					cout << "Status of thread: " << tek->pod->myID  << "\t" << tek->pod->blocked << endl;
					tek = tek->sled;
				}
				cout << "hehe xd" << endl;*/
		for (int j = 0; j< 20000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
}



