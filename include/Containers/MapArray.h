/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_MapVector.h
 @brief Map Vector
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/17 Created by minseob
 ***********************************************************/

#ifndef _LE_MAPARRAY_H_
#define _LE_MAPARRAY_H_

#include "Containers/Pair.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
    template<typename T1, typename T2> class MapArray : public Object<LimitEngineMemoryCategory_Common>
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
        T2& Find(const T1 &key, T2 defValue = NULL)
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

    protected:
        VectorArray<Pair<T1, T2>*>   mData;
    };
}

#endif
