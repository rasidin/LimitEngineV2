/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Debug.cpp
 @brief for Debug
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Core/Debug.h"

#ifdef WINDOWS
#include "../Platform/Windows/DebugImpl_Windows.inl"
#endif

namespace LimitEngine {
#ifdef WINDOWS
	Debug* SingletonDebug::mInstance = NULL;
#endif
	Debug::Debug()
		: mPrintflag(PRINTFLAG_LOG | PRINTFLAG_GUI)
		, mImpl(NULL)
	{
#ifdef WINDOWS
		mImpl = new DebugImpl_Windows();
#else
#error No implementation for Debug
#endif
	}

	Debug::~Debug()
	{
		if (mImpl) delete mImpl; mImpl = NULL;
	}

	void Debug::Print(const char *str, ...)
	{
		va_list list;
		va_start(list, str);
		if (mInstance->mImpl) {
			vsprintf_s(mInstance->mTextBuffer, TextBufferSize, str, list);
			_print(mInstance->mTextBuffer);
		}
		va_end(list);
	}

	void Debug::Warning(const char *str, ...)
	{
		va_list list;
		va_start(list, str);
		if (mInstance->mImpl) {
			vsprintf_s(mInstance->mTextBuffer, TextBufferSize, str, list);
            _print(mInstance->mTextBuffer);
            _print("\n");
		}
		va_end(list);
	}

	void Debug::Error(const char *str, ...)
	{
		va_list list;
		va_start(list, str);
		if (mInstance->mImpl) {
			vsprintf_s(mInstance->mTextBuffer, TextBufferSize, str, list);
            _print(mInstance->mTextBuffer);
			_print("\n");
		}
		va_end(list);
	}

	void Debug::_print(const char *s)
	{
		if (!mInstance->mImpl) return;
		if (mInstance->mPrintflag & PRINTFLAG_LOG) mInstance->mImpl->PrintLog(s);
		if (mInstance->mPrintflag & PRINTFLAG_GUI) mInstance->mImpl->PrintGUI(s);
	}
}