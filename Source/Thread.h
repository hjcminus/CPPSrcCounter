//2014-11-07 Fri.

#pragma once

#include <windows.h>

/*
================================================================================
Timer
================================================================================
*/
class Timer
{
public:

	Timer();

	void             Start();
	double           Stop();

private:

	double           mPrevious;
	static bool      mInited;
	static double    mMSPerCycle;

};

/*
================================================================================
SpinLocker
================================================================================
*/
class SpinLocker
{
public:

	SpinLocker();

	void             Lock();
	void             Unlock();

private:

	volatile unsigned long mLocker;
};


/*
========================================
Mutex
========================================
*/

class Mutex
{
public:

	Mutex();
	~Mutex();

	void             Lock();
	void             Unlock();

private:

	CRITICAL_SECTION mHandle;
};

/*
========================================
Signal
========================================
*/
class Signal
{
public:

	Signal();
	~Signal();

	void             Raise();
	void             Wait();

private:

	HANDLE           mHandle;
};

/*
========================================
Semaphore
========================================
*/
class Semaphore
{
public:

	Semaphore(int InitialCount, int MaxCount);
	~Semaphore();

	void             Reset();

	void             Release();
	void             Wait();

private:

	HANDLE           mHandle;
	int              mInitialCount;
	int              mMaxCount;
};
