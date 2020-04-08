/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Object.h
 @brief Object Class (BaseObject) Overwrite function for memory
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Object.h"

namespace LimitEngine {
    template<uint32 category = LimitEngineMemoryCategory_Unknown>
    class ReferenceCountedObject : public Object<category>
    {
    public:
        uint32 GetReferenceCounter() const { return mReferenceCounter; }

        uint32 AddReferenceCounter() { mReferenceCounter++; return mReferenceCounter; }
        uint32 SubReferenceCounter() { LEASSERT(mReferenceCounter > 0); mReferenceCounter--; return mReferenceCounter; }
    protected:
        ReferenceCountedObject() : mReferenceCounter(0u) {}
        virtual ~ReferenceCountedObject() {}
    private:
        uint32 mReferenceCounter;
    };
}