/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Common.h
 @brief Common Definitions
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#include <LEPlatform>

#pragma once

#if defined(WINDOWS)
#include <cassert>
#include <Windows.h>
#elif defined(IOS)
#include <assert.h>
#endif
#include <stdio.h>

#define PI		3.14159265f

// Define common utils
#ifndef MAX
#define MAX(a, b) ((a>b)?a:b)
#endif
#ifndef MIN
#define MIN(a, b) ((a<b)?a:b)
#endif
#ifndef CLAMP
#define CLAMP(v, mi, ma) ((v < mi)?mi:((v>ma)?ma:v))
#endif

#ifndef LEASSERT
#ifdef _DEBUG
#define LEASSERT(s)		assert(s)
#else
#define LEASSERT(s)
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef nullptr
#define nullptr 0
#endif
