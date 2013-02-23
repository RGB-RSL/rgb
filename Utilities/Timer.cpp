
#include "Timer.h"
//#include <Debugger.h>

// class Timer

Timer::Timer(): mSimTime(0u), mTime(.0f)
{
}

Timer::~Timer()
{
}

void Timer::setSimTime(long unsigned simTime)
{
	mSimTime = simTime;
}

void Timer::setTime(float time)
{
	mTime = time;
}

long unsigned Timer::getSimTime() const
{
	return mSimTime;
}

float Timer::getTime() const
{
	return mTime;
}

// class OnlineTimer

OnlineTimer::OnlineTimer()
{
	resetCycleTime();
}

OnlineTimer::~OnlineTimer()
{
}

void OnlineTimer::resetCycleTime()
{
	gettimeofday(&mCycleTimeBegin, NULL);
}

long unsigned OnlineTimer::getCycleTime() const
{
	timeval cycleTimeCur;
	gettimeofday(&cycleTimeCur, NULL);
	return (long unsigned)(((cycleTimeCur.tv_usec - mCycleTimeBegin.tv_usec) / 1000. +
	  (cycleTimeCur.tv_sec - mCycleTimeBegin.tv_sec) * 1000.) + .5);
}
