//2014-11-08 Sat.

#pragma once

/*
================================================================================
Chain
================================================================================
*/

template<typename T>
struct Chain
{
	T *     mOwner;
	Chain * mPrior;
	Chain * mNext;

	Chain() :mOwner(nullptr), mPrior(this), mNext(this)
	{
	}

	void InitOwner(T * owner)
	{
		mOwner = owner;
	}

	void AddToChain(Chain * &head)
	{
		if (head)
		{
			head->mPrior->mNext = this;
			mPrior = head->mPrior;
			mNext = head;
			head->mPrior = this;
		}
		else
		{
			head = this;
			mPrior = mNext = this;
		}
	}

	void RemoveFromChain(Chain * &head)
	{
		mPrior->mNext = mNext;
		mNext->mPrior = mPrior;

		if (head == this)
		{
			if (head->mNext == this)
			{
				head = nullptr;
			}
			else
			{
				head = head->mNext;
			}
		}
	}
};
