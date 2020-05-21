#ifndef TIMER_H_
#define TIMER_H_

#define lockFlag Timer::flag--;

#define unlockCare Timer::flag++;

#define unlockFlag		\
	Timer::flag++;	\
	if (Timer::zahtev){	\
		dispatch();		\
	};

void inicialize();
void restore();
void interrupt timer();

class Timer {
public:
	//Timer();
	//virtual ~Timer();
	static volatile unsigned brojac;
	static volatile unsigned flag;
	static volatile unsigned zahtev;

protected:
	friend class PCB;
};

#endif /* TIMER_H_ */
