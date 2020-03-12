/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_Platform_Win32.h
@brief Definitions for platform
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2017/4/19 Created by minseob
***********************************************************/
#include <Windows.h>
#pragma once

typedef HANDLE THREADHANDLE;

#ifdef _DEBUG
#define DEBUG_MESSAGE(str, ...) { char text[0xfff]; sprintf_s<0xfff>(text, str, __VA_ARGS__); OutputDebugString(text); }
#else
#define DEBUG_MESSAGE(str, ...)
#endif

#define	WINDOW_HANDLE HWND
#define EVENT_HANDLE HANDLE

typedef unsigned int frame_time;
typedef unsigned int uint_ptr;
typedef unsigned __int64 uint64;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef __int32 int32;
typedef unsigned __int16 uint16;
typedef __int16 int16;
typedef unsigned __int8 uint8;
typedef __int8 int8;

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif
