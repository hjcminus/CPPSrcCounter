//2014-11-07 Fri.

#include "stdafx.h"

/*
========================================
ParallelTask
========================================
*/
ParallelTask::ParallelTask()
{
#ifdef _DEBUG
	mValid = 1;
#endif
}

ParallelTask::~ParallelTask()
{
#ifdef _DEBUG
	mValid = 0;
#endif
}

#ifdef _DEBUG
void ParallelTask::AssertValid()
{
	assert(1 == mValid);
}
#endif

/*
========================================
ParallelSystem
========================================
*/

ParallelSystem gFileSystem;
ParallelSystem gSrcCountSystem;

ParallelSystem::ParallelSystem() : mInited(FALSE), mTrdHandle(NULL), mSignal(0, 99999999), mTaskChain(nullptr), mQuit(FALSE)
{
	//do nothing
}

ParallelSystem::~ParallelSystem()
{
	//do nothing
}

void ParallelSystem::Init()
{
	if (mInited)
	{
		return;
	}
	mInited = TRUE;

	mSignal.Reset();
	mQuit = FALSE;

	unsigned int TrdID = 0; //not using
	mTrdHandle = (HANDLE)_beginthreadex(nullptr, 0, ParallelSystem::TaskFunction, this, 0, &TrdID);
}

void ParallelSystem::Shutdown()
{
	if (!mInited)
	{
		return;
	}
	mInited = FALSE;

	if (mTrdHandle)
	{
		mQuit = TRUE;
		mSignal.Release(); //pump thread loop

		WaitForSingleObject(mTrdHandle, INFINITE);
		CloseHandle(mTrdHandle);
	}

	while (mTaskChain) //free unprocessed task
	{
		ParallelTask * Task = mTaskChain->mOwner;
		Task->mChain.RemoveFromChain(mTaskChain);
		Task->Free();
	}
}

void ParallelSystem::PushTask(ParallelTask *task)
{
	mLocker.Lock();
	{
		task->mChain.AddToChain(mTaskChain);
	}
	mLocker.Unlock();

	mSignal.Release();
}

void ParallelSystem::TaskLoop()
{
	while (1) //loop until quit
	{
		mSignal.Wait();

		if (mQuit)
		{
			break;
		}

		ParallelTask * Task = nullptr;

		mLocker.Lock();
		{
			if (mTaskChain)
			{
				Task = mTaskChain->mOwner;
				Task->mChain.RemoveFromChain(mTaskChain);
			}
		}
		mLocker.Unlock();

		if (Task)
		{
#ifdef _DEBUG
			Task->AssertValid();
#endif

			Task->Execute();
			Task->Free();

		}

	}

}

unsigned ParallelSystem::TaskFunction(void *pParam)
{
	ParallelSystem * obj = (ParallelSystem*)pParam;
	obj->TaskLoop();

	return 0;
}

/*
========================================
FileReadTask
========================================
*/

static FixedAllocator<sizeof(FileReadTask)> gFileReadTaskAllocator;

FileReadTask * FileReadTask::Create(const wchar_t *FileName)
{
	void * Buffer = gFileReadTaskAllocator.Alloc();
	return new (Buffer)FileReadTask(FileName);
}

FileReadTask::FileReadTask(const wchar_t *FileName)
{
	mChain.InitOwner(this);
	wcscpy_s(mFileName, FileName);
}

FileReadTask::~FileReadTask()
{
	//do nothing
}

void FileReadTask::Free()
{
	this->~FileReadTask();
	gFileReadTaskAllocator.Free(this);
}

void FileReadTask::Execute()
{
	char * Data = nullptr;
	int Len = 0;

	if (ReadAnsiText(mFileName, Data, Len))
	{
		wchar_t * WideChar = new wchar_t[Len];

		AnsiToUnicode(Data, WideChar, Len);

		delete [] Data;

		//do next task
		SrcCountTask * NextTask = SrcCountTask::Create(mFileName, WideChar);
		gSrcCountSystem.PushTask(NextTask);
	}
	else
	{
		FileReadFailedResult * Result = FileReadFailedResult::Create(mFileName);
		GetMainDialog()->NotifyResult(Result);
	}
}

/*
========================================
SrcCountTask
========================================
*/
static FixedAllocator<sizeof(SrcCountTask)> gSrcCountTaskAllocator;

SrcCountTask * SrcCountTask::Create(const wchar_t *FileName, const wchar_t * Data)
{
	void * Buffer = gSrcCountTaskAllocator.Alloc();
	return new (Buffer)SrcCountTask(FileName, Data);
}

SrcCountTask::SrcCountTask(const wchar_t *FileName, const wchar_t * Data) :mData(Data)
{
	mChain.InitOwner(this);
	wcscpy_s(mFileName, FileName);
}

SrcCountTask::~SrcCountTask()
{
	if (mData)
	{
		delete[] mData;
		mData = nullptr;
	}
}

void SrcCountTask::Free()
{
	this->~SrcCountTask();
	gSrcCountTaskAllocator.Free(this);
}

void SrcCountTask::Execute()
{
	SrcFileStatistic Statistic;

	const wchar_t * Src = (const wchar_t*)mData;

	GetSrcFileStatistic(Src, Statistic);

	SrcCountResult * Result = SrcCountResult::Create(mFileName);

	Result->mCodeLines = Statistic.CodeLines;
	Result->mCodeCommentLines = Statistic.CodeCommentLines;
	Result->mCommentLines = Statistic.CommentLines;
	Result->mBlankLines = Statistic.BlankLines;

	GetMainDialog()->NotifyResult(Result);
}

/*
========================================
FileReadFailedResult
========================================
*/
static FixedAllocator<sizeof(FileReadFailedResult)> gFileReadFailedResultAllocator;

FileReadFailedResult * FileReadFailedResult::Create(const wchar_t *FileName)
{
	void * Buffer = gFileReadFailedResultAllocator.Alloc();
	return new (Buffer)FileReadFailedResult(FileName);
}

FileReadFailedResult::FileReadFailedResult(const wchar_t *FileName)
{
	mChain.InitOwner(this);
	wcscpy_s(mFileName, FileName);
}

FileReadFailedResult::~FileReadFailedResult()
{
	//do nothing
}

void FileReadFailedResult::Free()
{
	this->~FileReadFailedResult();
	gFileReadFailedResultAllocator.Free(this);
}

void FileReadFailedResult::Execute()
{
	GetMainDialog()->InsertError(mFileName);
}

/*
========================================
SrcCountResult
========================================
*/
static FixedAllocator<sizeof(SrcCountResult)> gSrcCountResultAllocator;

SrcCountResult * SrcCountResult::Create(const wchar_t *filename)
{
	void * buffer = gSrcCountResultAllocator.Alloc();
	return new (buffer)SrcCountResult(filename);
}

SrcCountResult::SrcCountResult(const wchar_t *filename) :mCodeLines(0), mCodeCommentLines(0), mCommentLines(0), mBlankLines(0)
{
	mChain.InitOwner(this);
	wcscpy_s(mFileName, filename);
}

SrcCountResult::~SrcCountResult()
{
	//do nothing
}

void SrcCountResult::Free()
{
	this->~SrcCountResult();
	gSrcCountResultAllocator.Free(this);
}

void SrcCountResult::Execute()
{
	GetMainDialog()->InsertRecord(mFileName, mCodeLines, mCodeCommentLines, mCommentLines, mBlankLines);
}

void FreeMemory()
{
	gFileReadTaskAllocator.FreeAll();
	gSrcCountTaskAllocator.FreeAll();
	gFileReadFailedResultAllocator.FreeAll();
	gSrcCountResultAllocator.FreeAll();
}
