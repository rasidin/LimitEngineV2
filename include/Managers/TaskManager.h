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
        uint32			id;                     //!<ID of task                              [ 4 ]
		String			name;					//!<Name of task                            [ 4 ]
		uint32			priority;				//!<Priority of task                        [ 4 ]
		Function<void>	func;					//!<Function						        [ 8 ]
        float			elapsedFromLast;        //!<Elapsed time from last execute          [ 4 ]
        float			startTime;              //!<Task Start Time                         [ 4 ]
        uint32          transient:1;            //!<Trasient task (remove after running)    [ 4:1 ]
		uint32			reserved[1];			//                                          [ 4 ]
	} TASK;		// [ 32 ]

    template<typename L>
    struct ParallelForTask : public TASK
    {
        ParallelForTask(L &&LambdaFunc, uint32 StepBegin, uint32 StepEnd) 
            : mLambdaFunc(Forward<L>(LambdaFunc))
            , mStepBegin(StepBegin)
            , mStepEnd(StepEnd)
        {
            func = ThreadFunction(this, &ParallelForTask::RunLambda);
            transient = 1;
        }

        void RunLambda()
        {
            mLambdaFunc(mStepBegin, mStepEnd);
        }

        L mLambdaFunc;
        uint32 mStepBegin;
        uint32 mStepEnd;
    };

    class ParallelTask : public Object<LimitEngineMemoryCategory_Common>
    {
        VectorArray<TASK*> mTasks;
        Thread mThread;
        bool mExitCode = false;
    public:
        ParallelTask() {}
        virtual ~ParallelTask() {}

        inline bool IsRunning() const { return mThread.IsRunning(); }
        inline bool IsIdle() const { return mTasks.count() == 0; }

        void Init(const String &ThreadName);
        void Join();
        void AddTask(TASK *task) { mTasks.Add(task); }

        void Run();

        void Exit();
    };
    
public:				// Public Definition
	typedef uint32 TaskID;
    static constexpr uint32 ParallelTaskCount = 5;

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
		TASK *task = new TASK();
		task->name = name;
		task->priority = prior;
		task->func = Function<void>(ptr, func);
		task->startTime = 0;
		task->elapsedFromLast = 0;
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

    template<typename L>
    void ParallelFor(int LoopCount, L &&Func) {
        VectorArray<ParallelTask*> idleParallels;
        for (uint32 Index = 0; Index < ParallelTaskCount; Index++) {
            if (mParallelThreads[Index]->IsIdle()) {
                idleParallels.Add(mParallelThreads[Index]);
            }
        }

        uint32 step = max(1, LoopCount / (idleParallels.count() + 1));
        uint32 currentLoopNum = 0u;
        uint32 usedParallelTaskCount = 0u;
        for (uint32 Index = 0; Index < idleParallels.count(); Index++, usedParallelTaskCount++) {
            idleParallels[Index]->AddTask(new ParallelForTask<L>(Forward<L>(Func), currentLoopNum, currentLoopNum + step - 1));
            currentLoopNum += step;
            if (currentLoopNum >= LoopCount) {
                usedParallelTaskCount++;
                break;
            }
        }
        if (currentLoopNum < LoopCount)
            Func(currentLoopNum, static_cast<uint32>(LoopCount - 1));
        while (1) {
            bool bAllClear = true;
            for (uint32 Index = 0; Index < usedParallelTaskCount; Index++) {
                bAllClear &= idleParallels[Index]->IsIdle();
            }
            if (bAllClear) break;
        }
    }

    void Run();
private:			// Private Functions
    void runTasks();
    TaskID getIDfromName(const char *name);
    TaskID addTask(TASK *task);
	void removeTask(TaskID id);
    float getElapsedTime(uint32 id);
    
	bool compareTaskPriority(const TASK &t1, const TASK &t2);

private:			// Private Members
    uint32                      mId_counter;
	VectorArray<TASK*>			mTasks;

    Mutex                        mMutex;                   //!< Mutex
    Thread                      *mWorkThread;              //!< Thread for run tasks
    bool                         mWorkThreadExitCode;      //!< Exitcode for work thread
    ParallelTask                *mParallelThreads[ParallelTaskCount];           //!< Thread for parallel
};
#define LE_TaskManager TaskManager::GetSingleton()
}
