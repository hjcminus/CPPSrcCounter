//2014-11-07 Fri.

#pragma once

#include "Thread.h"
#include "Linker.h"

/*
========================================
ParallelTask
========================================
*/
class ParallelTask
{
public:

	ParallelTask();
	virtual ~ParallelTask();

	virtual void          Free() = 0;
	virtual void          Execute() = 0;

#ifdef _DEBUG
	void                  AssertValid();
#endif

public:

	Chain<ParallelTask>   mChain;

private:

#ifdef _DEBUG
	int                   mValid;
#endif

};

/*
========================================
ParallelSystem
========================================
*/
class ParallelSystem
{
public:

	ParallelSystem();
	~ParallelSystem();

	void                  Init();
	void                  Shutdown();

	//no priority support now
	void                  PushTask(ParallelTask *Task);

private:

	BOOL                  mInited;

	HANDLE                mTrdHandle;

	Mutex                 mLocker;
	Semaphore             mSignal;

	Chain<ParallelTask> * mTaskChain;

	BOOL                  mQuit;

	void                  TaskLoop();

	static unsigned  __stdcall  TaskFunction(void *pParam);

};

extern ParallelSystem gFileSystem;
extern ParallelSystem gSrcCountSystem;

/*
========================================
FileReadTask
========================================
*/
class FileReadTask : public ParallelTask
{
public:

	static FileReadTask *  Create(const wchar_t *FileName);

	virtual void         Free();
	virtual void         Execute();

private:

	wchar_t              mFileName[MAX_PATH];

	FileReadTask(const wchar_t *FileName);
	virtual ~FileReadTask();

};

/*
========================================
SrcCountTask
========================================
*/
class SrcCountTask : public ParallelTask
{
public:

	static SrcCountTask *  Create(const wchar_t *FileName, const wchar_t * Data);

	virtual void         Free();
	virtual void         Execute();

private:

	wchar_t              mFileName[MAX_PATH];
	const wchar_t *      mData;

	SrcCountTask(const wchar_t *FileName, const wchar_t * Data);
	virtual ~SrcCountTask();

};

/*
========================================
FileReadFailedResult
========================================
*/
class FileReadFailedResult : public ParallelTask
{
public:

	static FileReadFailedResult * Create(const wchar_t *FileName);

	virtual void         Free();
	virtual void         Execute();

public:

	wchar_t              mFileName[MAX_PATH];

private:

	FileReadFailedResult(const wchar_t *FileName);
	virtual ~FileReadFailedResult();
};

/*
========================================
SrcCountResult
========================================
*/
class SrcCountResult : public ParallelTask
{
public:

	static SrcCountResult *  Create(const wchar_t *FileName);

	virtual void         Free();
	virtual void         Execute();

public:

	wchar_t              mFileName[MAX_PATH];
	int                  mCodeLines;
	int                  mCodeCommentLines;
	int                  mCommentLines;
	int                  mBlankLines;

private:

	SrcCountResult(const wchar_t *FileName);
	virtual ~SrcCountResult();

};

void FreeMemory();
