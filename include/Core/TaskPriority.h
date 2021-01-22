/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_TaskPriority.h
 @brief Task Priority
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/11/1 Created by minseob
 ***********************************************************/
#pragma once
#ifndef _LE_TASKPRIORITY_H_
#define _LE_TASKPRIORITY_H_

namespace LimitEngine {
	enum TaskPriority {
		TaskPriority_Routine = 0x100,
		TaskPriority_Renderer_Ready = 0x200,
		TaskPriority_Renderer_UpdateScene,
        TaskPriority_Renderer_UpdateLight,
		TaskPriority_Renderer_DrawScene,
        TaskPriority_Renderer_PostProcess,
        TaskPriority_Renderer_DrawDebugUI,
		TaskPriority_Renderer_DrawEnd,
        TaskPriority_Renderer_DrawManager_Run,
	};
}

#endif // _LE_TASKPRIORITY_H_