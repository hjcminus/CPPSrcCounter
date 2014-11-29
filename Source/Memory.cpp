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
		int newCapacity = mCapacity + (mCapacity >> 1);
		if (newCapacity < mAllocated + needSize)
		{
			newCapacity = mAllocated + needSize;
		}

		char * newData = new char[newCapacity];
		assert(newData != nullptr);

		memcpy(newData, mData, mAllocated);

		//update link chain
		UpdateLinkChain(newData, mData);

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

void QueuedStringPool::UpdateLinkChain(char * pNewData, char * pOldData)
{
	if (!mFirst)
	{
		assert(mLast == nullptr);
		return;
	}

	str_s * OldFirst = mFirst;
	str_s * NewFirst = (str_s*)pNewData;

	mFirst = NewFirst;
	mFirst->mNext = nullptr;

	str_s * OldStr = OldFirst;
	str_s * NewStr = NewFirst;

	while (OldStr->mNext)
	{
		size_t offset = (size_t)OldStr->mNext - (size_t)pOldData;
		NewStr->mNext = (str_s*)((size_t)pNewData + offset);

		//update mLast
		mLast = NewStr->mNext;
		mLast->mNext = nullptr;

		OldStr = OldStr->mNext;
		NewStr = mLast;
	}
	
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
