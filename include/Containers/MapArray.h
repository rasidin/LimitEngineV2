/*******************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------
@file  MapArray.h
@brief Vector array
@author minseob(leeminseob@outlook.com)
********************************************************************/
#ifndef _LE_MAPARRAY_H_
#define _LE_MAPARRAY_H_

#include "Containers/Pair.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
template<typename MapT, typename T1, typename T2> class MapArrayIterator
{
public:
    MapArrayIterator(MapT& owner, uint32 index = 0u)
        : mOwnerMapArray(owner)
        , mCurrentIndex(index)
    {}

    MapArrayIterator& operator++()
    {
        mCurrentIndex++;
        return *this;
    }
    MapArrayIterator operator++(int)
    {
        MapArrayIterator output(*this);
        ++mCurrentIndex;
        return output;
    }
    MapArrayIterator& operator--()
    {
        mCurrentIndex--;
        return *this;
    }
    MapArrayIterator operator--(int)
    {
        MapArrayIterator output(*this);
        --mCurrentIndex;
        return output;
    }

    Pair<T1, T2>& operator*() const { return mOwnerMapArray.GetAt(mCurrentIndex); }
    Pair<T1, T2>* operator->() const { return &mOwnerMapArray.GetAt(mCurrentIndex); }

    operator bool() const { return mOwnerMapArray.size() > mCurrentIndex; }

    bool operator==(const MapArrayIterator& iterator) const { return mOwnerMapArray == iterator.mOwnerMapArray && mCurrentIndex == iterator.mCurrentIndex; }
    bool operator!=(const MapArrayIterator& iterator) const { return &mOwnerMapArray != &iterator.mOwnerMapArray || mCurrentIndex != iterator.mCurrentIndex; }

private:
    MapT&  mOwnerMapArray;
    uint32 mCurrentIndex;
};
template<typename T1, typename T2> class MapArray : public Object<LimitEngineMemoryCategory::Common>
{
public:
    typedef Pair<T1, T2> MapArrayItem;

public:
    MapArray()              {}
    MapArray(const MapArray &map)
    {
        mData.Resize(map.mData.count());
        for(uint32 dtidx=0;dtidx<mData.count();dtidx++) {
            mData[dtidx] = new Pair<T1, T2>();
            *mData[dtidx] = *map.mData[dtidx];
        }
    }
    virtual ~MapArray()
    {
        Clear();
    }
    void Clear()
    {
        for(uint32 i=0;i<mData.GetSize();i++)
        {
            delete mData[i];
        }
        mData.Clear();
    }
    void Add(const T1 &key, const T2 &value)
    {
        mData.Add(new MapArrayItem(key, value));
    }
    size_t GetSize() const      { return mData.GetSize(); }
    size_t size() const         { return mData.size(); }
    int FindIndex(const T1 &key) const
    {
        for(uint32 i=0;i<mData.GetSize();i++)
        {
            MapArrayItem *d = mData[i];
            if (key == d->key) return static_cast<int>(i);
        }
        return -1;
    }
    T2* Find(const T1& key)
    {
        for (uint32 i = 0; i < mData.GetSize(); i++)
        {
            MapArrayItem* d = mData[i];
            if (d->key == key) return &d->value;
        }
        return nullptr;
    }
    T2& FindOrCreate(const T1 &key, T2 defValue)
    {
        for (uint32 i = 0; i<mData.GetSize(); i++)
        {
            MapArrayItem *d = mData[i];
            if (d->key == key) return d->value;
        }
        MapArrayItem *newPair = new Pair<T1, T2>();
        newPair->key = key;
        newPair->value = defValue;
        mData.Add(newPair);
        return newPair->value;
    }
    Pair<T1, T2>& GetAt(uint32 index) const { return *mData[index]; }
    T2& operator [] (const T1 &key)
    {
        for (uint32 i=0;i<mData.GetSize();i++)
        {
            MapArrayItem *d = mData[i];
            if (d->key == key) return d->value;
        }
        MapArrayItem *newPair = new Pair<T1, T2>();
        newPair->key = key;
        mData.Add(newPair);
        return newPair->value;
    }
    const T2& operator [] (const T1 &key) const
    {
        for (uint32 i=0;i<mData.GetSize();i++)
        {
            MapArrayItem *d = mData[i];
            if (d->key == key) return d->value;
        }
        LEASSERT(false);
        return mData[0]->value;
    }
    void operator=(const MapArray<T1, T2> &map)
    {
        mData.Resize(map.mData.count());
        for(uint32 dtidx=0;dtidx<mData.count();dtidx++) {
            mData[dtidx] = new MapArrayItem();
            *mData[dtidx] = *map.mData[dtidx];
        }
    }

    typedef MapArrayIterator<MapArray<T1, T2>, T1, T2> Iterator;
    Iterator begin() { return Iterator(*this); }
    Iterator end() { return Iterator(*this, mData.size()); }

protected:
    VectorArray<Pair<T1, T2>*>   mData;
};
}

#endif
