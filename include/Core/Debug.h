/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Debug.h
 @brief for Debug
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/26 Created by minseob
 ***********************************************************/

#ifndef _LE_DEBUG_H_
#define _LE_DEBUG_H_

#include "Singleton.h"

#define _CRT_SECURE_NO_WARNINGS

namespace LimitEngine {
    class DebugImpl : public Object<LimitEngineMemoryCategory::Debug>
    {
    public:
        DebugImpl() {}
        virtual ~DebugImpl() {}

        virtual void PrintLog(const char *s) = 0;
        virtual void PrintGUI(const char *s) = 0;
        virtual void Clear() = 0;
    }; // DebugImpl

    class Debug;
    typedef Singleton<Debug, LimitEngineMemoryCategory::Debug> SingletonDebug;
    class Debug : public SingletonDebug
    {
    public:
        enum PRINTFLAG
        {
            PRINTFLAG_LOG        = 1,
            PRINTFLAG_GUI        = 1 << 1,
            PRINTFLAG_SAVELOG    = 1 << 2,
        };

		static const size_t TextBufferSize = 0x1fff;
        static const char EndL = '\n';

    public:
        Debug();
        virtual ~Debug();

        inline Debug& operator << (const char *s)
        {
            _print(s);
            return *this;
        }

        inline Debug& operator << (char c)
        {
            char s[2] = { c, 0 };
            _print(s);
            return *this;
        }

        inline Debug& operator << (float f)
        {
            char s[0xff];
            sprintf_s(s, 0xff, "%f", f);
            _print(s);
            return *this;
        }
        
        inline Debug& operator << (int n)
        {
            char s[0xff];
            sprintf_s(s, 0xff, "%d", n);
            _print(s);
            return *this;
        }
        
        static void Print(const char* str, ...);
        static void Warning(const char* str, ...);
        static void Error(const char* str, ...);
        static void Clear();

    private:
        static void _print(const char *s);

    private:
        uint32        mPrintflag;

		char		  mTextBuffer[TextBufferSize];
        DebugImpl    *mImpl;
    }; 
#define gDebug Debug::GetSingleton()
}

#endif // _LE_DEBUG_H_