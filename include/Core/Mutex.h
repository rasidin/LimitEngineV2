/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Mutex.h
 @brief Mutex for threading
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#pragma once

namespace LimitEngine
{
    class Mutex
    {
	public:
		class ScopedLock
		{
		public:
			ScopedLock(Mutex& mutex):mMutex(mutex){ mMutex.Lock();}
			ScopedLock( const ScopedLock& );
			~ScopedLock(){ mMutex.Unlock();}
            
		private:
			const ScopedLock& operator=(const ScopedLock&) {}

			Mutex& mMutex;
		};
    public:
		Mutex()
		{
			InitializeCriticalSection(&mCriticalSection);
		}
        
		~Mutex()
		{
			DeleteCriticalSection(&mCriticalSection);
		}
		
		void Lock()
		{
			EnterCriticalSection(&mCriticalSection);
		}
		
		bool TryLock()
		{
			return TryEnterCriticalSection(&mCriticalSection) != 0;
		}
		
		void Unlock()
		{
			LeaveCriticalSection(&mCriticalSection);
		}
        
	private:
		CRITICAL_SECTION mCriticalSection;
    };
}
