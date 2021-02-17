/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderManager.h
 @brief Shader Manager
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include "Core/Singleton.h"
#include "Containers/VectorArray.h"
#include "Renderer/Shader.h"
#include "Core/ReferenceCountedPointer.h"

#define LE_ShaderManager LimitEngine::ShaderManager::GetSingleton()

namespace LimitEngine {
	class ShaderManager;
	typedef Singleton<ShaderManager, LimitEngineMemoryCategory::Graphics> SingletonShaderManager;
    class ShaderManager : public SingletonShaderManager
    {
    public:
        ShaderManager();
        virtual ~ShaderManager();

        void Init();
        void Term();
        
        template<typename T> ShaderRefPtr GetShader() { return ShaderRefPtr(findshader(T::GetHash())); }

        void LoadShaderSet(const char *filename);
        void AddShader(Shader *shader);
        uint32 GetShaderID(const char *shaderName);
        ShaderRefPtr GetShader(const char *name);
        
    private:
        Shader* findshader(const ShaderHash& hash) const;

    private:
		uint32						mShaderID;
        VectorArray<ShaderRefPtr>   mShaders;
    }; // ShaderManager
}
