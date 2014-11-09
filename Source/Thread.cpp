//2014-11-07 Fri.

#include "stdafx.h"

/*
================================================================================
Timer
================================================================================
*/
bool   Timer::mInited;
double Timer::mMSPerCycle;

Timer::Timer() :mPrevious(0)
{
}

void Timer::Start()
{
	if (!mInited)
	{
		mInited = true;

		__int64 f;
		QueryPerformanceFrequency((LARGE_INTEGER*)&f);
		mMSPerCycle = 1.0 / f;
	}

	__int64 c;
	QueryPerformanceCounter((LARGE_INTEGER*)&c);
	mPrevious = c * mMSPerCycle;
}

double Timer::Stop()
{
	__int64 c;
	QueryPerformanceCounter((LARGE_INTEGER*)&c);
	double cur = c * mMSPerCycle;
	return cur - mPrevious;
}

/*
================================================================================
SpinLocker
================================================================================
*/
SpinLocker::SpinLocker() :mLocker(0)
{
}

void SpinLocker::Lock()
{
	while (1)
	{
		if (0 == InterlockedCompareExchange(&mLocker, 1, 0))
		{
			return;
		}
		Sleep(0); //give up the remainder of its time slice for other threads
	}
}

void SpinLocker::Unlock()
{
	InterlockedExchange(&mLocker, 0);
}

/*
========================================
Mutex
========================================
*/
Mutex::Mutex()
{
	InitializeCriticalSection(&mHandle);
}

Mutex::~Mutex()
{
	DeleteCriticalSection(&mHandle);
}

void Mutex::Lock()
{
	EnterCriticalSection(&mHandle);
}

void Mutex::Unlock()
{
	LeaveCriticalSection(&mHandle);
}

/*
========================================
Signal
========================================
*/
Signal::Signal()
{
	mHandle = CreateEvent(nullptr, FALSE, 0, nullptr);
}

Signal::~Signal()
{
	if (mHandle)
	{
		CloseHandle(mHandle);
		mHandle = NULL;
	}
}

void Signal::Raise()
{
	SetEvent(mHandle);
}

void Signal::Wait()
{
	//TO FIX: check result WAIT_OBJECT_0
	WaitForSingleObject(mHandle, INFINITE);
}

/*
========================================
Semaphore
========================================
*/
Semaphore::Semaphore(int InitialCount, int MaxCount) : mHandle(NULL), mInitialCount(InitialCount), mMaxCount(MaxCount)
{
	Reset();
}

Semaphore::~Semaphore()
{
	if (mHandle)
	{
		CloseHandle(mHandle);
		mHandle = NULL;
	}
}

void Semaphore::Reset()
{
	if (mHandle)
	{
		CloseHandle(mHandle);
		mHandle = NULL;
	}

	mHandle = CreateSemaphore(nullptr, mInitialCount, mMaxCount, nullptr);
}

void Semaphore::Release()
{
	ReleaseSemaphore(mHandle, 1, nullptr);
}

void Semaphore::Wait()
{
	WaitForSingleObject(mHandle, INFINITE);
}
