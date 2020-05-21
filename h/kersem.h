#ifndef KERSEM_H_
#define KERSEM_H_

class Sem;
class PCB;
struct elem;

class KernelSem {
public:
	KernelSem(int v);
	virtual ~KernelSem();
	static elem* waitingQue;
	elem* myQue;
	volatile int value;

	int wait(unsigned maxTimeToWait);
	void signal();

	static void putPriTime(PCB* pcb, elem *&list);
	static void updateWaitingQue();
	static PCB* updateSignalQue(int ID, elem *&list);

};

#endif /* KERSEM_H_ */
