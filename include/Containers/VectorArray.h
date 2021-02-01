/*******************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------
@file  VectorArray.h
@brief Vector array
@author minseob (leeminseob@outlook.com)
********************************************************************/
#ifndef _LE_VECTORARRAY_H_
#define _LE_VECTORARRAY_H_

#include <functional>
#include <memory.h>
#include "Core/Object.h"
#include "Core/Common.h"
#include "Core/SerializableResource.h"

namespace LimitEngine
{
template <typename ArrayT, typename T> class VectorArrayIterator
{
public:
    VectorArrayIterator(ArrayT& owner, uint32 index = 0u)
        : mOwnerArray(owner)
        , mCurrentIndex(index)
    {}

	VectorArrayIterator& operator++()
	{
		mCurrentIndex++;
		return *this;
	}
	VectorArrayIterator operator++(int)
	{
        VectorArrayInterator result(*this);
		++mCurrentIndex;
		return result;
	}
    VectorArrayIterator operator--()
    {
        mCurrentIndex--;
        return *this;
    }
    VectorArrayIterator operator--(int)
    {
        VectorArrayIterator result(*this);
        --mCurrentIndex;
        return result;
    }

    T& operator*() const  { return  mOwnerArray[mCurrentIndex]; }
    T* operator->() const { return &mOwnerArray[mCurrentIndex]; }

    operator bool() const { return mOwnerArray.Count() > mCurrentIndex; }

	bool operator==(const VectorArrayIterator &iterator) const { return &mOwnerArray == &iterator.mOwnerArray && mCurrentIndex == iterator.mCurrentIndex; }
	bool operator!=(const VectorArrayIterator &iterator) const { return &mOwnerArray != &iterator.mOwnerArray || mCurrentIndex != iterator.mCurrentIndex; }

private:
    ArrayT& mOwnerArray;            // Onwer VectorArray
	uint32  mCurrentIndex;		    // Index of VectorArray
};
template <typename T> class VectorArray : public Object<LimitEngineMemoryCategory::Common>, public SerializableResource
{
#define VECTORARRAY_INIT          mSize(0)\
                                , mData(0)\
                                , mReserved(0)
public:
    VectorArray() : VECTORARRAY_INIT                {}
    VectorArray(uint32 s) : VECTORARRAY_INIT        {}
    VectorArray(const VectorArray &v) : VECTORARRAY_INIT
    {   // Copy
        Resize(v.size());
        for(uint32 dtidx=0;dtidx<v.size();dtidx++)
            mData[dtidx] = v[dtidx];
    }
    ~VectorArray()
    {
        if (mData) 
        {
            for (uint32 i=0;i< mSize;i++) mData[i].~T();
            free(mData);
        }
        mSize = 0;
        mData = NULL;
    }

    inline uint32 GetSize()    const                { return mSize; }
    inline T& GetStart()                            { return *(mData); }
    inline T& GetLast()                             { return *(mData + mSize - 1); }
    
    inline void Reserve(uint32 n)
    {
        if (mSize >= n)
        {
            mReserved = n;
        }
        else
        {
            T *newData = (T*)malloc(sizeof(T) * n);
            memset(newData, 0, sizeof(T) * n);
            if (mSize > 0)
                memcpy(newData, mData, sizeof(T) * mSize);
            free(mData);
            mData = newData;
            mReserved = n;
        }
    }

    inline void Resize(uint32 n)
    {
        if (n <= mReserved)
        {
            mSize = n;
            return;
        }

        T *newData = (T*)malloc(sizeof(T) * n);
        memset(newData, 0, sizeof(T) * n);
        if (n <= mSize)
            memcpy(newData, mData, sizeof(T) * n);
        else
            memcpy(newData, mData, sizeof(T) * mSize);
        mSize = n;
        mReserved = mSize;
        if (mData) free(mData);
        mData = newData;
    }

    inline T& Add() {
        Resize(GetSize()+1);
        return mData[GetSize()-1];
    }

    inline void Add(const T& d)
    {
        Resize(GetSize()+1);
        LEASSERT(mData);
        mData[GetSize()-1] = d;
    }

    inline void Delete(uint32 n)
    {
        LEASSERT(mSize);
        if (!mSize) return;
        mData[n].~T();
        --mSize;
        if (mReserved < mSize)
        {
            mReserved = mSize;
        }
        for(uint32 i=n;i<mSize;i++)
        {
            memcpy(&mData[i], &mData[i+1], sizeof(T));
        }
    }

    T PopFront()
    {
        if (mSize == 0)
            return T();
        T output = mData[0];
        Delete(0);
        return output;
    }

    int32 IndexOf(const T &t)
    {
        for (int32 i = 0; i < static_cast<int32>(mSize); i++) {
            if (mData[i] == t)
                return i;
        }
        return -1;
    }

    inline T* GetData() const { return mData; }

    inline void Sort(std::function<bool(const T &t1, const T &t2)> func)
    {
        if(mData == NULL || mSize == 0) return;

        T *newData = (T*)malloc(sizeof(T) * mSize);
        ::memset(newData, 0, sizeof(T) * mSize);
        newData[0] = mData[0];
        uint32 newDataSize = 1;
        T *prevData = &newData[0];
        for(uint32 idx=1;idx< mSize;idx++, newDataSize++) {
            uint32 nidx=0;
            for(;nidx<newDataSize;nidx++) {
                if(func(newData[nidx], mData[idx]) == false) {
                    for(uint32 nnidx=newDataSize;nnidx>nidx;nnidx--) {
                        newData[nnidx] = newData[nnidx-1];
                    }
                    break;
                }
            }
            newData[nidx] = mData[idx];
        }
        for (uint32 i=0;i< mSize;i++) mData[i].~T();
        free(mData);
        mData = newData;
    }

    virtual bool Serialize(Archive &OutArchive) override
    {
        if (OutArchive.IsLoading()) {
            uint32 size;
            OutArchive << size;
            Resize(size);
            void* archiveData = OutArchive.GetData(mSize * sizeof(T));
            memcpy(mData, archiveData, mSize * sizeof(T));
        }
        else {
            OutArchive << mSize;
            void* archiveData = OutArchive.AddSize(mSize * sizeof(T));
            memcpy(archiveData, mData, mSize * sizeof(T));
        }
        return true;
    }

    // For STL
    inline void push_back(const T& d)       { Add(d); }
    inline uint32 count() const             { return mSize; }
    inline uint32 size() const              { return mSize; }
    inline void erase(uint32 n)             { Delete(n); }
    
    void Clear(bool FreeReservedData = true)
    {
        if (mData)
        {
            for(uint32 i=0;i<mSize;i++) mData[i].T::~T();
            memset(mData, 0, sizeof(T) * mReserved);
        }
        mSize = 0;
        if (FreeReservedData) {
            if (mData) {
                free(mData);
                mData = nullptr;
            }
            mReserved = 0;
        }
    }
    T& First()              { return mData[0]; }
    const T& First() const  { return mData[0]; }
    T& Last()               { return mData[mSize - 1]; }
    const T& Last() const   { return mData[mSize - 1]; }

    T& operator [] (uint32 n)               { LEASSERT(n < mSize); return *(mData + n); }
    const T& operator [] (uint32 n) const   { LEASSERT(n < mSize); return *(mData + n); }
    void operator=(const VectorArray<T> &t)
    {
        Resize(t.size());
        for(uint32 i=0;i< mSize;i++)
        {
            mData[i] = t[i];
        }
    }

	typedef VectorArrayIterator<VectorArray<T>, T> Iterator;
	Iterator begin() { return Iterator(*this); }
	Iterator end() { return Iterator(*this, mSize); }
private:
    uint32  mSize;
    uint32  mReserved;
    T*      mData;
};
}

#endif