/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_VectorArray.h
 @brief VectorArray Class (such as std::vector)
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/13 Created by minseob
 ***********************************************************/

#ifndef _LE_VECTORARRAY_H_
#define _LE_VECTORARRAY_H_

#include <functional>
#include <iterator>
#include <memory.h>
#include "Core/Object.h"
#include "Core/Common.h"

namespace LimitEngine
{
template <typename T> class VectorArray;
template <typename T> class VectorArrayIterator : public std::iterator<std::forward_iterator_tag, T>
{
	friend VectorArray<T>;

public:
	VectorArrayIterator(const VectorArrayIterator &iterator)
		: mOwnerArray(iterator.mOwnerArray)
		, mCurrentIndex(iterator.mCurrentIndex)
	{}

	VectorArrayIterator& operator++()
	{
		mCurrentIndex++;
		// Mark end
		if (mCurrentIndex >= mOwnerArray->GetSize()) {
			mOwnerArray = NULL;
			mCurrentIndex = SIZE_MAX;
		}
		return *this;
	}
	VectorArrayIterator operator++(int)
	{
		VectorArrayInterator result = this;
		mCurrentIndex++;
		// Mark end
		if (mCurrentIndex >= mOwnerArray->GetSize()) {
			mOwnerArray = NULL;
			mCurrentIndex = SIZE_MAX;
		}
		return result;
	}

	T& operator*() { static T DummyObject = T(); return (!IsValid()) ? (*mOwnerArray)[mCurrentIndex] : DummyObject; }

	bool operator==(const VectorArrayIterator &iterator) const { return mOwnerArray == iterator.mOwnerArray && mCurrentIndex == iterator.mCurrentIndex; }
	bool operator!=(const VectorArrayIterator &iterator) const { return mOwnerArray != iterator.mOwnerArray || mCurrentIndex != iterator.mCurrentIndex; }

	bool IsValid() const { return mCurrentIndex == SIZE_MAX || mOwnerArray == NULL || mOwnerArray->GetSize() <= mCurrentIndex; }
private: // Non-creatable by user
	VectorArrayIterator()
		: mOwnerArray(NULL)
		, mCurrentIndex(SIZE_MAX)
	{}
	VectorArrayIterator(VectorArray<T> *owner, size_t index)
		: mOwnerArray(owner)
		, mCurrentIndex(index)
	{}

private:
	VectorArray<T> *mOwnerArray; // Onwer VectorArray
	size_t mCurrentIndex;		 // Index of VectorArray
};
template <typename T> class VectorArray : public Object<LimitEngineMemoryCategory_Common>
{
#define VECTORARRAY_INIT          _size(0)\
                                , _data(0)\
                                , _reserved(0)
public:
    VectorArray() : VECTORARRAY_INIT                {}
    VectorArray(size_t s) : VECTORARRAY_INIT        {}
    VectorArray(const VectorArray &v) : VECTORARRAY_INIT
    {   // Copy
        Resize(v.size());
        for(uint32 dtidx=0;dtidx<v.size();dtidx++)
            _data[dtidx] = v[dtidx];
    }
    ~VectorArray()
    {
        if (_data) 
        {
            for (size_t i=0;i<_size;i++) _data[i].~T();
            free(_data);
        }
        _size = 0;
        _data = NULL;
    }

    inline size_t GetSize()    const                { return _size; }
    inline T& GetStart()                            { return *(_data); }
    inline T& GetLast()                             { return *(_data + _size - 1); }
    
    inline void Reserve(size_t n)
    {
        if (_size >= n)
        {
            _reserved = n;
        }
        else
        {
            T *newData = (T*)malloc(sizeof(T) * n);
            memset(newData, 0, sizeof(T) * n);
            if (_size > 0)
                memcpy(newData, _data, sizeof(T) * _size);
            free(_data);
            _data = newData;
            _reserved = n;
        }
    }

    inline void Resize(size_t n)
    {
        if (n <= _reserved)
        {
            _size = n;
            return;
        }

        T *newData = (T*)malloc(sizeof(T) * n);
        memset(newData, 0, sizeof(T) * n);
        if (n <= _size)
            memcpy(newData, _data, sizeof(T) * n);
        else
            memcpy(newData, _data, sizeof(T) * _size);
        _size = n;
        _reserved = _size;
        if (_data) free(_data);
        _data = newData;
    }

    inline T& Add() {
        Resize(GetSize()+1);
        return _data[GetSize()-1];
    }

    inline void Add(const T& d)
    {
        Resize(GetSize()+1);
        LEASSERT(_data);
        _data[GetSize()-1] = d;
    }

    inline void Delete(size_t n)
    {
        LEASSERT(_size);
        if (!_size) return;
        _data[n].~T();
        --_size;
        if (_reserved < _size)
        {
            _reserved = _size;
        }
        for(size_t i=n;i<_size;i++)
        {
            memcpy(&_data[i], &_data[i+1], sizeof(T));
        }
    }
    
    inline void Delete(T t)
    {
        for(size_t datacnt=0;datacnt<_size;++datacnt)
        {
            if (_data[datacnt] == t) {
                Delete(datacnt);
                break;
            }
        }
    }
    
    inline void Sort(std::function<bool(const T &t1, const T &t2)> func)
    {
        if(_data == NULL || _size == 0) return;

        T *newData = (T*)malloc(sizeof(T) * _size);
        ::memset(newData, 0, sizeof(T) * _size);
        newData[0] = _data[0];
        uint32 newDataSize = 1;
        T *prevData = &newData[0];
        for(uint32 idx=1;idx<_size;idx++, newDataSize++) {
            uint32 nidx=0;
            for(;nidx<newDataSize;nidx++) {
                if(func(newData[nidx], _data[idx]) == false) {
                    for(uint32 nnidx=newDataSize;nnidx>nidx;nnidx--) {
                        newData[nnidx] = newData[nnidx-1];
                    }
                    break;
                }
            }
            newData[nidx] = _data[idx];
        }
        for (size_t i=0;i<_size;i++) _data[i].~T();
        free(_data);
        _data = newData;
    }

    // For STL
    inline void push_back(const T& d)       { Add(d); }
    inline size_t count() const             { return _size; }
    inline size_t size() const              { return _size; }
    inline void erase(size_t n)             { Delete(n); }
    
    void Clear()
    {
        if (_data) 
        {
            for(size_t i=0;i<_size;i++) _data[i].~T();
            memset(_data, 0, sizeof(T) * _reserved);
        }    
        _size = 0;
        _reserved = 0;
    }

    T& operator [] (size_t n)                { LEASSERT(n < _size); return *(_data + n); }
    const T& operator [] (size_t n) const   { LEASSERT(n < _size); return *(_data + n); }
    void operator=(const VectorArray<T> &t)
    {
        Resize(t.size());
        for(size_t i=0;i<_size;i++)
        {
            _data[i] = t[i];
        }
    }

	typedef VectorArrayIterator<T> Iterator;
	Iterator begin() { return Iterator(this, 0); }
	Iterator end() { return Iterator(); }
private:
    size_t    _size;
    size_t  _reserved;
    T*        _data;
};
}

#endif