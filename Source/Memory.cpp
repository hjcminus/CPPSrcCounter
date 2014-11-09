//2014-11-08 Sat.

#include "stdafx.h"

QueuedStringPool::QueuedStringPool() :mData(nullptr), mCapacity(0), mAllocated(0),
mFirst(nullptr), mLast(nullptr), mNext(nullptr), mCount(0)
{
}

QueuedStringPool::~QueuedStringPool()
{
	Clear();
}

void QueuedStringPool::AddString(const wchar_t *str)
{
	if (!mData)
	{
		mCapacity = 512 * 1024;
		mData = new char[mCapacity];
		memset(mData, 0, mCapacity);
		mAllocated = 0;
	}

	int l = (int)wcslen(str);

	int needSize = sizeof(str_s)+(l + 1) * sizeof(wchar_t);

	int freeSize = mCapacity - mAllocated;
	if (freeSize < needSize)
	{
		int newCapacity = mAllocated + needSize;
		char * newData = new char[newCapacity];
		memcpy(newData, mData, mAllocated);
		mCapacity = newCapacity;
		delete[] mData;
		mData = newData;
	}

	str_s * cur = (str_s*)(mData + mAllocated);
	cur->mNext = nullptr;

	if (mLast)
	{
		mLast->mNext = cur;
	}

	wchar_t * dest = (wchar_t*)(cur + 1);
	memcpy(dest, str, sizeof(wchar_t)* l);
	dest[l] = 0;

	mLast = cur;
	if (!mFirst)
	{
		mFirst = cur;
	}

	mAllocated += needSize;
	mCount++;
}

const wchar_t * QueuedStringPool::GetFirstString()
{
	wchar_t * result = nullptr;
	if (mFirst)
	{
		result = (wchar_t*)(mFirst + 1);
		mNext = mFirst->mNext;
	}
	return result;
}

const wchar_t * QueuedStringPool::GetNextString()
{
	wchar_t * result = nullptr;
	if (mNext)
	{
		result = (wchar_t*)(mNext + 1);
		mNext = mNext->mNext;
	}
	return result;
}

int QueuedStringPool::GetCount() const
{
	return mCount;
}

void QueuedStringPool::Clear()
{
	if (mData)
	{
		delete[] mData;
		mData = nullptr;
	}

	mCapacity = mAllocated = mCount = 0;
	mFirst = mLast = mNext = nullptr;
}
