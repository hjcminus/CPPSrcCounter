//2014-11-08 Sat.

#pragma once

#include "Thread.h"
#include "Linker.h"
#include <assert.h>

/*
================================================================================
QueuedStringPool
  single thread
================================================================================
*/
class QueuedStringPool
{
public:

	QueuedStringPool();
	~QueuedStringPool();

	void            AddString(const wchar_t *str);

	const wchar_t * GetFirstString();
	const wchar_t * GetNextString();
	int             GetCount() const;

	void            Clear();

private:

	struct str_s
	{
		str_s *     mNext;
	};

	char *          mData;
	int             mCapacity;
	int             mAllocated;
	int             mCount;

	str_s *         mFirst;
	str_s *         mLast;
	str_s *         mNext;
};

/*
================================================================================
Chunk
================================================================================
*/

#define CHUNK_ID 0x5E0E52AB

template<size_t blocksize>
class Chunk
{
public:

	static const size_t ALIGNED_SIZE = (blocksize + 15) & ~15;

	struct Block
	{
		Chunk * mOwner;
		char    mData[ALIGNED_SIZE];
	};

	Chunk() : mID(CHUNK_ID), mFreeCount(256)
	{
		mChain.InitOwner(this);
		for (unsigned int i = 0; i < 256; i++)
		{
			mBlocks[i].mOwner = this;
			mFreeIdx[i] = i;
		}
	}

	~Chunk()
	{
		memset(mBlocks, 0, sizeof(mBlocks));
		mID = 0;
	}

	Chain<Chunk>  mChain;
	unsigned int  mID;
	Block         mBlocks[256];
	unsigned char mFreeIdx[256];
	unsigned int  mFreeCount;

	Block  * Alloc()
	{
		Block * b = nullptr;

		if (mFreeCount > 0)
		{
			b = mBlocks + mFreeIdx[--mFreeCount];
			return b;
		}

		return nullptr;
	}

	void Free(Block *b)
	{
		assert(b->mOwner == this);
		unsigned char idx = b - mBlocks;
		mFreeIdx[mFreeCount++] = idx;
	}

};

/*
================================================================================
FixedAllocator
================================================================================
*/

template<size_t blocksize>
class FixedAllocator
{
public:

	FixedAllocator() :mFreeChain(nullptr), mFullChain(nullptr)
	{
	}

	void * Alloc()
	{
		mLocker.Lock();

		Chunk<blocksize>::Block * b = nullptr;

		if (mFreeChain)
		{
			Chunk<blocksize> * chunk = mFreeChain->mOwner;

			b = chunk->Alloc();

			if (0 == chunk->mFreeCount)
			{
				//remove from free chain
				chunk->mChain.RemoveFromChain(mFreeChain);

				//add to full chain
				chunk->mChain.AddToChain(mFullChain);
			}
		}
		else
		{
			Chunk<blocksize> * chunk = new Chunk<blocksize>();

			b = chunk->Alloc();

			//add to free chain
			chunk->mChain.AddToChain(mFreeChain);
		}

		mLocker.Unlock();

		return b->mData;
	}

	void Free(void *p)
	{
		Chunk<blocksize>::Block * b = (Chunk<blocksize>::Block*)((char*)p - sizeof(Chunk<blocksize>*));
		Chunk<blocksize> * chunk = b->mOwner;
		assert(chunk->mID == CHUNK_ID);

		mLocker.Lock();

		chunk->Free(b);

		if (1 == chunk->mFreeCount)
		{
			//remove from full chain
			chunk->mChain.RemoveFromChain(mFullChain);

			//add to free chain
			chunk->mChain.AddToChain(mFreeChain);
		}
		else if (256 == chunk->mFreeCount)
		{
			//remove from free chain
			chunk->mChain.RemoveFromChain(mFreeChain);

			//free memory
			delete chunk;
		}

		mLocker.Unlock();
	}

	//force free
	void FreeAll()
	{
		mLocker.Lock();

		while (mFreeChain)
		{
			Chunk<blocksize> * chunk = mFreeChain->mOwner;
			chunk->mChain.RemoveFromChain(mFreeChain);

			delete chunk;
		}

		while (mFullChain)
		{
			Chunk<blocksize> * chunk = mFullChain->mOwner;
			chunk->mChain.RemoveFromChain(mFullChain);

			delete chunk;
		}

		mLocker.Unlock();
	}

private:

	SpinLocker mLocker;
	Chain<Chunk<blocksize>> * mFreeChain;
	Chain<Chunk<blocksize>> * mFullChain;
};
