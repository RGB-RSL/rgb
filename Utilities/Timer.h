#ifndef __TIMER_H_
#define __TIMER_H_

#include <sys/time.h>

#ifndef NULL
#define NULL 0
#endif // NULL

class Timer
{
protected:
	long unsigned mSimTime;
	float mTime;

public:
	Timer();
	virtual ~Timer();

	virtual void resetCycleTime() = 0;
	virtual long unsigned getCycleTime() const = 0;

	void setSimTime(long unsigned simTime);
	void setTime(float time);
	long unsigned getSimTime() const;
	float getTime() const;
};

class OnlineTimer: public Timer
{
protected:
	timeval mCycleTimeBegin;

public:
	OnlineTimer();
	virtual ~OnlineTimer();

	virtual void resetCycleTime();
	virtual long unsigned getCycleTime() const;
};

#endif // __TIMER_H_
