#ifndef PCB_H_
#define PCB_H_


class Thread;
class KernelSem;
struct elem;

#define lock asm{\
pushf;\
cli;\
}

#define unlock asm popf

class PCB {
public:
	PCB(unsigned long size, unsigned tiem, Thread *t);
	PCB(unsigned tiem);
	~PCB();
	unsigned ss;
	unsigned sp;
	unsigned bp;
	unsigned *stack;
	unsigned time;
	int finished;
	int blocked;
	unsigned blockedTime;
	int signalized;
	int numberOfChildren;
	PCB* father;
	KernelSem *mySem;
	Thread *myThread;
	static volatile PCB* running;
	static volatile PCB* idle;
	static int pomID;
	int myID;
	static void wrapper();
	unsigned long size;
	elem *blockedOnes;

	static elem *listOfAll;

	static void put(PCB* pcb, elem *&list);
	static PCB* get(int ID, elem *&list);
	static PCB* get(elem *&list);
};

struct elem{
	elem();
	PCB *pod;
	elem *sled;
	~elem();
};


#endif /* PCB_H_ */
