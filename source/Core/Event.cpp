/***********************************************************
LIMITEngine Source File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  Event.cpp
@brief Event
@author minseob (https://github.com/rasidin)
***********************************************************/
#include "Core/Event.h"
#ifdef WINDOWS
#include "../Platform/Windows/EventImpl_Windows.inl"
#else
#error No implementation for Event
#endif