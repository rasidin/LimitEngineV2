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
    class IReferenceCountedObject
    {
    public:
        virtual uint32 GetReferenceCounter() const = 0;
        virtual uint32 AddReferenceCounter() = 0;
        virtual uint32 SubReferenceCounter() = 0;
    };

    template<uint32 category = LimitEngineMemoryCategory_Unknown>
    class ReferenceCountedObject : public IReferenceCountedObject, public Object<category>
    {
    public:
        uint32 GetReferenceCounter() const override { return mReferenceCounter; }

        uint32 AddReferenceCounter() override { return ++mReferenceCounter; }
        uint32 SubReferenceCounter() override { LEASSERT(mReferenceCounter > 0); return --mReferenceCounter; }
    protected:
        ReferenceCountedObject() : mReferenceCounter(0u) {}
        virtual ~ReferenceCountedObject() {}
    private:
        uint32 mReferenceCounter;
    };
}