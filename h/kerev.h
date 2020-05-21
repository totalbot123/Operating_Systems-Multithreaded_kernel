#ifndef KEREV_H_
#define KEREV_H_

class PCB;

class KernelEv {
public:
	KernelEv(int n);
	virtual ~KernelEv();
	int entry;
	PCB *myPCB;

	void signal();
	void wait();
};

#endif /* KEREV_H_ */
