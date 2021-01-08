/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_LightManager.h
 @brief Manager of lights
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/11/09 Created by minseob
 ***********************************************************/
#pragma once
#ifndef _LE_LIGHTMANAGER_H_
#define _LE_LIGHTMANAGER_H_

#include <LE_Graphics>
#include "LE_Singleton.h"

#include "LE_RenderState.h"
#include "LE_VectorArray.h"

namespace LimitEngine {
    class LightManager;
    typedef Singleton<LightManager, LimitEngineMemoryCategory::Graphics> SingletonLightManager;
    class LightManager : public SingletonLightManager {
    public:
        LightManager();
        virtual ~LightManager();
        void AddLight(Light *light);
        void ApplyLight(RenderState &rs, Mesh *mesh);

        LightIBL* GetStandardIBLLight() const { return mStandardIBL; }

        void Update();

        void DrawBackground(const RenderState &rs);
        void DrawForeground(const RenderState &rs);
    private:
        VectorArray<Light*>      mLightList;

        LightIBL                *mStandardIBL;
    }; // LightManager
#define LE_LightManager LimitEngine::LightManager::GetSingleton()
} // LimitEngine

#endif // _LE_LIGHTMANAGER_H_