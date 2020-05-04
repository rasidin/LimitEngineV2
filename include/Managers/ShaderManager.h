/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderManager.h
 @brief Shader Manager
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Core/Singleton.h"
#include "Containers/VectorArray.h"
#include "Renderer/Shader.h"
#include "Core/ReferenceCountedPointer.h"

#define LE_ShaderManager LimitEngine::ShaderManager::GetSingleton()

namespace LimitEngine {
	class ShaderManager;
	typedef Singleton<ShaderManager, LimitEngineMemoryCategory_Graphics> SingletonShaderManager;
    class ShaderManager : public SingletonShaderManager
    {
    public:
        ShaderManager();
        virtual ~ShaderManager();

        void Init();
        void Term();
        void LoadShaderSet(const char *filename);
		void BindShader(Shader *sh);
        void BindShader(uint32 shaderID);
		void BindShader(const char *name);
        void AddShader(Shader *shader);
        uint32 GetShaderID(const char *shaderName);
        ShaderRefPtr GetShader(const char *name);
        
    private:
		uint32						mShaderID;
        VectorArray<ShaderRefPtr>   mShaders;
    }; // ShaderManager
}
