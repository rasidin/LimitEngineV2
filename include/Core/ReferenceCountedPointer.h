/***********************************************************
LIMITEngine Header File
Copyright(C), LIMITGAME, 2020
---------------------------------------------------------- -
@file  ReferenceCountedPOinter.h
@brief Pointer with reference counter
@author minseob(leeminseob@outlook.com)
***********************************************************/
#pragma once

namespace LimitEngine {
    template<typename T>
    class ReferenceCountedPointer
    {
    public:
        ReferenceCountedPointer() : mData(nullptr) {}
        ReferenceCountedPointer(const ReferenceCountedPointer &In) : mData(In.mData) {
            if (mData) {
                mData->AddReferenceCounter();
            }
        }
        ReferenceCountedPointer(T *Data) : mData(Data) {
            if (mData) {
                mData->AddReferenceCounter();
            }
        }
        ~ReferenceCountedPointer() {
            if (mData && mData->SubReferenceCounter() == 0) {
                delete mData;
            }
        }
        T* Get() const { return mData; }
        void Release() {
            if (mData) {
                if (!mData->SubReferenceCounter()) {
                    delete mData;
                }
            }
            mData = nullptr;
        }
        bool IsValid() const {
            return mData != nullptr;
        }
        ReferenceCountedPointer& operator = (const ReferenceCountedPointer &InPointer) {
            Release();
            mData = InPointer.mData;
            if (mData)
                mData->AddReferenceCounter();
            return *this;
        }
        ReferenceCountedPointer& operator = (T *InPointer) {
            Release();
            mData = InPointer;
            if (mData)
                mData->AddReferenceCounter();
            return *this;
        }
        T* operator ->() const {
            return mData;
        }
        //bool operator () const {
        //    return mData != nullptr;
        //}
    private:
        T *mData;
    };
}