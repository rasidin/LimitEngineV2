/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  AutoPointer.h
@brief Auto Release Pointer
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include "MemoryAllocator.h"

namespace LimitEngine {
	template<typename T>
	class AutoPointer
	{
	public:
		AutoPointer() : mBuffer(nullptr) {}
        // Constructure for copying...
		AutoPointer(T *buf) : mBuffer(buf) {}
		AutoPointer(const AutoPointer<T>& t) //!< Not copy, but move
		{
			mBuffer = t.mBuffer;
			t.mBuffer = nullptr;
		}
		~AutoPointer()
		{
			if (mBuffer) {
                mBuffer->Release();
				mBuffer = nullptr;
			}
		}
        // When coping pointer
		inline void operator = (AutoPointer &ptr)
		{
			mBuffer = ptr.mBuffer;
			ptr.mBuffer = nullptr;
		}
		inline void operator = (T* ptr)
		{
			if (mBuffer == ptr) return;
			if (mBuffer) {
				mBuffer->Release();
			}
			mBuffer = ptr;
		}
        inline T* Pop() {
            T *output = mBuffer;
            mBuffer = nullptr;
            return output;
        }
        // Get pointer
		inline operator T* ()
		{
			return mBuffer;
		}
		inline T* operator->()
		{
			return mBuffer;
		}
		inline T* Get() const { return mBuffer; }
        // Does pointer exist?
		bool Exists() { return mBuffer != nullptr; }
	protected:
		T *mBuffer;     // Target pointer
	}; // AutoPointer
} // LimitEngine
