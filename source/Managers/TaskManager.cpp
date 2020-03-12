/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  TaskManager.cpp
 @brief TaskManager Class
 @author minseob (leeminseob@outlook.com)
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
    TASK task;
    task.name = name;
    task.priority = prior;
	task.func = Function<void>(func);
    task.startTime = 0;
    task.elapsedFromLast = 0;
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
        mTasks[taskidx].elapsedFromLast = static_cast<float>(Timer::GetTimeDoubleSecond()) - mTasks[0].startTime;
        mTasks[taskidx].startTime = static_cast<float>(Timer::GetTimeDoubleSecond());
        mTasks[taskidx].func();
    }
}
// =================================================
// Private
// =================================================
TaskManager::TaskManager()
	: Singleton()
    , mWorkThread(NULL)
    , mWorkThreadExitCode(false)
	, mTasks()
    , mId_counter(0)
{
    mWorkThread = new Thread();
}

TaskManager::~TaskManager()
{
    if (mWorkThread->IsRunning()) {
        mWorkThreadExitCode = true;
        mWorkThread->Join();
    }
    delete mWorkThread;
    mWorkThread = NULL;
}

void TaskManager::Init()
{
    if (mWorkThread->IsRunning() == false) {
        ThreadParam param;
        param.func = ThreadFunction(this, &TaskManager::runTasks);
        //mWorkThread->Create(param);
    }
}

void TaskManager::Term()
{
    mMutex.Lock();
    mTasks.Clear();
    mMutex.Unlock();

    if (mWorkThread->IsRunning()) {
        mWorkThreadExitCode = true;
        mWorkThread->Join();
    }
}

void TaskManager::runTasks()
{
    while (1) {
        for (uint32 taskidx = 0; taskidx < mTasks.size(); taskidx++) {
            Mutex::ScopedLock scopedLock(mMutex);
            mTasks[taskidx].elapsedFromLast = static_cast<float>(Timer::GetTimeDoubleSecond()) - mTasks[0].startTime;
            mTasks[taskidx].startTime = static_cast<float>(Timer::GetTimeDoubleSecond());
            mTasks[taskidx].func();
            if (mWorkThreadExitCode)
                break;
        }
        if (mWorkThreadExitCode)
            break;
    }
}

uint32 TaskManager::getIDfromName(const char *name)
{
    for(size_t i=0;i<mTasks.GetSize();i++)
    {
        if (mTasks[i].name == name) return mTasks[i].id;
    }
    return 0u;
}

bool TaskManager::compareTaskPriority(const TaskManager::TASK &t1, const TaskManager::TASK &t2)
{
	return t1.priority >= t2.priority;
}

uint32 TaskManager::addTask(TaskManager::TASK &task)
{
    Mutex::ScopedLock scopedLock(mMutex);
    mId_counter++;
    task.id = mId_counter;
    mTasks.push_back(task);

	mTasks.Sort([](const TASK &t1, const TASK &t2){
		return t1.priority <= t2.priority;
	});

    return mId_counter;
}

void TaskManager::removeTask(TaskManager::TaskID id)
{
	for(uint32 taskidx=0;taskidx<mTasks.count();taskidx++) {
		if(mTasks[taskidx].id == id) {
			mTasks.Delete(taskidx);
		}
	}
}

float TaskManager::getElapsedTime(TaskManager::TaskID id)
{
    if (!id) 
		return 0;
    for(size_t i=0;i<mTasks.GetSize();i++)
    {
        if (mTasks[i].id == id) return mTasks[i].elapsedFromLast;
    }
    return 0;
}
} // namespace LimitEngine