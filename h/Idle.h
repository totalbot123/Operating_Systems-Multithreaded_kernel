#ifndef IDLE_H_
#define IDLE_H_

#include "Thread.h"

class Idle : Thread{
public:
	Idle(StackSize size = defaultStackSize, int time = defaultTimeSlice);
	void run();
};


#endif /* IDLE_H_ */
