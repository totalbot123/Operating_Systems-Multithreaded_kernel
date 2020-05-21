#ifndef IVTENTRY_H_
#define IVTENTRY_H_

class KernelEv;

#define PREPAREENTRY(number,flag) void interrupt routine##number(); \
		IVTEntry* entry##number = new IVTEntry(number,routine##number); \
		void interrupt routine##number(){ \
			entry##number->signal(); \
			if (flag == 1){ \
				entry##number->oldRout(); \
			}; \
}

//void interrupt key();

typedef void (interrupt *InterruptRoutine)();

class KernelEv;

class IVTEntry {
public:
	IVTEntry(int n, InterruptRoutine r);
	virtual ~IVTEntry();
	KernelEv* myEvent;
	void signal();
	InterruptRoutine myRoutine;
	InterruptRoutine oldRoutine;
	static IVTEntry** listOfIVT;

	static IVTEntry* getIVT(int n);
	InterruptRoutine swapRoutine(int n, InterruptRoutine routine, KernelEv *ke);

	void oldRout();

};

#endif /* IVTENTRY_H_ */
