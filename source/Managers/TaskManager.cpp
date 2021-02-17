/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  TaskManager.cpp
 @brief TaskManager Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#include "Core/Common.h"
#include "Core/Debug.h"
#include "Core/Timer.h"
#include "Core/Util.h"
#include "Managers/TaskManager.h"

namespace LimitEngine {

// =================================================
// Static Functions
// =================================================
#ifdef WIN32
TaskManager* SingletonTaskManager::mInstance = NULL;
#endif
TaskManager::TaskID TaskManager::GetIDfromName(const char *name)
{
    return mInstance->getIDfromName(name);
}

TaskManager::TaskID TaskManager::AddTask(const String &name, uint32 prior, void (*func)())
{
    TASK *task = new TASK();
    task->name = name;
    task->priority = prior;
	task->func = Function<void>(func);
    task->startTime = 0;
    task->elapsedFromLast = 0;
    task->transient = 0;
    return mInstance->addTask(task);
}

void TaskManager::RemoveTask(TaskManager::TaskID id) 
{
	mInstance->removeTask(id);
}

float TaskManager::GetElapsedTime(uint32 id)
{
    return mInstance->getElapsedTime(id);
}

float TaskManager::GetElapsedTime(const char *name)
{
    return mInstance->getElapsedTime(mInstance->getIDfromName(name));
}

void TaskManager::Run()
{
    for (uint32 taskidx = 0; taskidx < mTasks.size(); taskidx++) {
        Mutex::ScopedLock scopedLock(mMutex);
        mTasks[taskidx]->elapsedFromLast = static_cast<float>(Timer::GetTimeDoubleSecond()) - mTasks[0]->startTime;
        mTasks[taskidx]->startTime = static_cast<float>(Timer::GetTimeDoubleSecond());
        mTasks[taskidx]->func();
    }
}

void TaskManager::ParallelTask::Init(const String &ThreadName)
{
    ThreadParam param;
    param.func = ThreadFunction(this, &TaskManager::ParallelTask::Run);
    param.name = ThreadName;
    mThread.Create(param);
}

void TaskManager::ParallelTask::Join()
{
    mThread.Join();
}

void TaskManager::ParallelTask::Exit()
{
    mExitCode = true;
    Join();
}

void TaskManager::ParallelTask::Run()
{
    while (!mExitCode)
    {
        VectorArray<uint32> removeTasks;
        for (uint32 taskIndex = 0; taskIndex < mTasks.count(); taskIndex++) {
            mTasks[taskIndex]->func();
            if (mTasks[taskIndex]->transient) {
                removeTasks.Add(taskIndex);
            }
        }
        if (removeTasks.count())
        for (uint32 removeIdx = 0; removeIdx < removeTasks.count(); removeIdx++) {
            const uint32 removeTarget = removeTasks[removeIdx];
            delete mTasks[removeTarget];
            mTasks.erase(removeTarget);
        }
        Sleep(1);
    }
}

TaskManager::TaskManager()
	: Singleton()
    , mWorkThread(NULL)
    , mWorkThreadExitCode(false)
	, mTasks()
    , mId_counter(0)
{
    mWorkThread = new Thread();
    for (uint32 ThreadIndex = 0; ThreadIndex < ParallelTaskCount; ThreadIndex++) {
        mParallelThreads[ThreadIndex] = new ParallelTask();
    }
}

TaskManager::~TaskManager()
{
    delete mWorkThread;
    mWorkThread = NULL;

    for (uint32 ThreadIndex = 0; ThreadIndex < ParallelTaskCount; ThreadIndex++)
    {
        delete mParallelThreads[ThreadIndex];
        mParallelThreads[ThreadIndex] = nullptr;
    }
}

void TaskManager::Init()
{
    if (mWorkThread->IsRunning() == false) {
        ThreadParam param;
        param.func = ThreadFunction(this, &TaskManager::runTasks);
        param.name = "MainTask";
        mWorkThread->Create(param);
    }
    for (uint32 ThreadIndex = 0; ThreadIndex < ParallelTaskCount; ThreadIndex++) {
        if (mParallelThreads[ThreadIndex]->IsRunning() == false) {
            String threadName = "SubTask";
            char numBuf[8];
            sprintf_s<8>(numBuf, "%1d", ThreadIndex);
            mParallelThreads[ThreadIndex]->Init(threadName + numBuf);
        }
    }
}

void TaskManager::Term()
{
    if (mWorkThread->IsRunning()) {
        mWorkThreadExitCode = true;
        mWorkThread->Join();
        for (uint32 TaskIndex = 0; TaskIndex < mTasks.count(); TaskIndex++) {
            delete mTasks[TaskIndex];
        }
        mTasks.Clear();
    }
    for (uint32 ThreadIndex = 0; ThreadIndex < ParallelTaskCount; ThreadIndex++)
    {
        if (mParallelThreads[ThreadIndex]->IsRunning()) {
            mParallelThreads[ThreadIndex]->Exit();
        }
    }
}

void TaskManager::runTasks()
{
    while (!mWorkThreadExitCode) {
        VectorArray<uint32> removeTasks;
        mMutex.Lock();
        for (uint32 taskidx = 0; taskidx < mTasks.size(); taskidx++) {
            mTasks[taskidx]->elapsedFromLast = static_cast<float>(Timer::GetTimeDoubleSecond()) - mTasks[0]->startTime;
            mTasks[taskidx]->startTime = static_cast<float>(Timer::GetTimeDoubleSecond());
            mTasks[taskidx]->func();
            if (mTasks[taskidx]->transient) {
                removeTasks.Add(taskidx);
            }
            if (mWorkThreadExitCode)
                break;
        }
        mMutex.Unlock();
        if (removeTasks.count()) {
            Mutex::ScopedLock scopedLock(mMutex);
            for (uint32 removeIdx = removeTasks.count() - 1; removeIdx > 0; removeIdx--) {
                const uint32 removeTarget = removeTasks[removeIdx];
                delete mTasks[removeTarget];
                mTasks.erase(removeTarget);
            }
        }
        Sleep(1);
    }
}

uint32 TaskManager::getIDfromName(const char *name)
{
    for(uint32 i=0;i<mTasks.GetSize();i++)
    {
        if (mTasks[i]->name == name) return mTasks[i]->id;
    }
    return 0u;
}

bool TaskManager::compareTaskPriority(const TaskManager::TASK &t1, const TaskManager::TASK &t2)
{
	return t1.priority >= t2.priority;
}

uint32 TaskManager::addTask(TaskManager::TASK *task)
{
    Mutex::ScopedLock scopedLock(mMutex);
    mId_counter++;
    task->id = mId_counter;
    mTasks.push_back(task);

	mTasks.Sort([](const TASK *t1, const TASK *t2){
		return t1->priority <= t2->priority;
	});

    return mId_counter;
}

void TaskManager::removeTask(TaskManager::TaskID id)
{
    Mutex::ScopedLock scopedLock(mMutex);
    for(uint32 taskidx=0;taskidx<mTasks.count();taskidx++) {
		if(mTasks[taskidx]->id == id) {
            delete mTasks[taskidx];
			mTasks.Delete(taskidx);
		}
	}
}

float TaskManager::getElapsedTime(TaskManager::TaskID id)
{
    if (!id) 
		return 0;
    for(uint32 i=0;i<mTasks.GetSize();i++)
    {
        if (mTasks[i]->id == id) return mTasks[i]->elapsedFromLast;
    }
    return 0;
}
} // namespace LimitEngine