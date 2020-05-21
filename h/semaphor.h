// File: semaphor.h
#ifndef _semaphor_h_
#define _semaphor_h_

typedef unsigned int Time;

class KernelSem;

class Semaphore {
public:
 Semaphore (int init=1);
 virtual ~Semaphore ();
 virtual int wait (Time maxTimeToWait);
 virtual void signal();
 KernelSem* myImpl;
 int val () const; // Returns the current value of the semaphore

private:
 //KernelSem* myImpl;

};
#endif
