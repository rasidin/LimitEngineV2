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
#include "Core/SerializableResource.h"

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
			mCurrentIndex = 0xffffffffu;
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
			mCurrentIndex = 0xffffffffu;
		}
		return result;
	}

	T& operator*() { static T DummyObject = T(); return (!IsValid()) ? (*mOwnerArray)[mCurrentIndex] : DummyObject; }

	bool operator==(const VectorArrayIterator &iterator) const { return mOwnerArray == iterator.mOwnerArray && mCurrentIndex == iterator.mCurrentIndex; }
	bool operator!=(const VectorArrayIterator &iterator) const { return mOwnerArray != iterator.mOwnerArray || mCurrentIndex != iterator.mCurrentIndex; }

	bool IsValid() const { return mCurrentIndex == 0xffffffffu || mOwnerArray == NULL || mOwnerArray->GetSize() <= mCurrentIndex; }
private: // Non-creatable by user
	VectorArrayIterator()
		: mOwnerArray(NULL)
		, mCurrentIndex(0xffffffffu)
	{}
	VectorArrayIterator(VectorArray<T> *owner, uint32 index)
		: mOwnerArray(owner)
		, mCurrentIndex(index)
	{}

private:
	VectorArray<T> *mOwnerArray; // Onwer VectorArray
	uint32 mCurrentIndex;		 // Index of VectorArray
};
template <typename T> class VectorArray : public Object<LimitEngineMemoryCategory_Common>, public SerializableResource
{
#define VECTORARRAY_INIT          _size(0)\
                                , _data(0)\
                                , _reserved(0)
public:
    VectorArray() : VECTORARRAY_INIT                {}
    VectorArray(uint32 s) : VECTORARRAY_INIT        {}
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
            for (uint32 i=0;i<_size;i++) _data[i].~T();
            free(_data);
        }
        _size = 0;
        _data = NULL;
    }

    inline uint32 GetSize()    const                { return _size; }
    inline T& GetStart()                            { return *(_data); }
    inline T& GetLast()                             { return *(_data + _size - 1); }
    
    inline void Reserve(uint32 n)
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

    inline void Resize(uint32 n)
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

    inline void Delete(uint32 n)
    {
        LEASSERT(_size);
        if (!_size) return;
        _data[n].~T();
        --_size;
        if (_reserved < _size)
        {
            _reserved = _size;
        }
        for(uint32 i=n;i<_size;i++)
        {
            memcpy(&_data[i], &_data[i+1], sizeof(T));
        }
    }

    int32 IndexOf(const T &t)
    {
        for (int32 i = 0; i < static_cast<int32>(_size); i++) {
            if (_data[i] == t)
                return i;
        }
        return -1;
    }

    inline T* GetData() const { return _data; }

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
        for (uint32 i=0;i<_size;i++) _data[i].~T();
        free(_data);
        _data = newData;
    }

    virtual bool Serialize(Archive &OutArchive) override
    {
        if (OutArchive.IsLoading()) {
            uint32 size;
            OutArchive << size;
            Resize(size);
            void* archiveData = OutArchive.GetData(_size * sizeof(T));
            memcpy(_data, archiveData, _size * sizeof(T));
        }
        else {
            OutArchive << _size;
            void* archiveData = OutArchive.AddSize(_size * sizeof(T));
            memcpy(archiveData, _data, _size * sizeof(T));
        }
        return true;
    }

    // For STL
    inline void push_back(const T& d)       { Add(d); }
    inline uint32 count() const             { return _size; }
    inline uint32 size() const              { return _size; }
    inline void erase(uint32 n)             { Delete(n); }
    
    void Clear()
    {
        if (_data) 
        {
            for(uint32 i=0;i<_size;i++) _data[i].T::~T();
            memset(_data, 0, sizeof(T) * _reserved);
        }    
        _size = 0;
        _reserved = 0;
    }

    T& operator [] (uint32 n)                { LEASSERT(n < _size); return *(_data + n); }
    const T& operator [] (uint32 n) const   { LEASSERT(n < _size); return *(_data + n); }
    void operator=(const VectorArray<T> &t)
    {
        Resize(t.size());
        for(uint32 i=0;i<_size;i++)
        {
            _data[i] = t[i];
        }
    }

	typedef VectorArrayIterator<T> Iterator;
	Iterator begin() { return Iterator(this, 0); }
	Iterator end() { return Iterator(); }
private:
    uint32    _size;
    uint32  _reserved;
    T*        _data;
};
}

#endif