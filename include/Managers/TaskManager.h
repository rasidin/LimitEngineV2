/*****************************************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file	LE_TaskManager.h
 * @brief	LIMITEngine Task Manager
 * @author	minseob
 *****************************************************************************/
#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"
#include "Core/Function.h"
#include "Core/Mutex.h"
#include "Core/Thread.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
class TaskManager;
typedef Singleton<TaskManager, LimitEngineMemoryCategory_Common> SingletonTaskManager;
class TaskManager : public SingletonTaskManager
{
private:			// Private Structure
	typedef struct _TASK
	{
        uint32			id;                     //!<ID of task                      [ 4 ]
		String			name;					//!<Name of task                    [ 4 ]
		uint32			priority;				//!<Priority of task                [ 4 ]
		Function<void>	func;					//!<Function						[ 8 ]
        float			elapsedFromLast;        //!<Elapsed time from last execute  [ 4 ]
        float			startTime;              //!<Task Start Time                 [ 4 ]
		uint32			reserved[2];			//                                  [ 8 ]
	} TASK;		// [ 32 ]

public:				// Public Definition
	typedef uint32 TaskID;

public:				// Public Functions
	// ==================================================
	// Static Functions
	// ==================================================

	// ==================================================
	// Public Functions
	// ==================================================
    static TaskID GetIDfromName(const char *name);
	static TaskID AddTask(const String& name, uint32 prior, void (*func)());
	template<typename TypeName>
	static TaskID AddTask(const String& name, uint32 prior, TypeName *ptr, void (TypeName::*func)())
	{
		TASK task;
		task.name = name;
		task.priority = prior;
		task.func = Function<void>(ptr, func);
		task.startTime = 0;
		task.elapsedFromLast = 0;
		return mInstance->addTask(task);
	}

	static void RemoveTask(TaskID id);
    static float GetElapsedTime(TaskID id);
    static float GetElapsedTime(const char *name);
    
	// ==================================================
	// Ctor & Dtor
	// ==================================================
	TaskManager();
	~TaskManager();

	void Init();
	void Term();

    void Run();
private:			// Private Functions
    void runTasks();
    TaskID getIDfromName(const char *name);
    TaskID addTask(TASK &task);
	void removeTask(TaskID id);
    float getElapsedTime(uint32 id);
    
	bool compareTaskPriority(const TASK &t1, const TASK &t2);

private:			// Private Members
    uint32                      mId_counter;
	VectorArray<TASK>			mTasks;

    Mutex                        mMutex;                   //!< Mutex
    Thread                      *mWorkThread;              //!< Thread for run tasks
    bool                         mWorkThreadExitCode;      //!< Exitcode for work thread
};
#define LE_TaskManager TaskManager::GetSingleton()
}
