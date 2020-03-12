/*****************************************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
* @file    LE_EventListener.h
* @brief    Event listener
* @author    minseob
*****************************************************************************/
#pragma once

#include "Core/Function.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
	typedef Function<void> EventFunctionType;
	class EventListener
	{
	public:
		void* operator+=(const EventFunctionType &eventFunction)
		{
			mEventFunctions.Add(eventFunction);
			return this;
		}
		void* operator-=(const EventFunctionType &eventFunction)
		{
			mEventFunctions.Delete(eventFunction);
			return this;
		}
		void operator()() {
			for (int32 funcidx = 0; funcidx < mEventFunctions.count(); funcidx++) {
				mEventFunctions[funcidx]();
			}
		}
	private:
		VectorArray<EventFunctionType> mEventFunctions;
	};
}
