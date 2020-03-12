/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_ShaderManager.h
 @brief Shader Manager
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/19 Created by minseob
 ***********************************************************/

#ifndef _LE_SHADERMANAGER_H_
#define _LE_SHADERMANAGER_H_

#include "LE_Singleton.h"
#include "LE_VectorArray.h"
#include "LE_Shader.h"

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
        Shader* GetShader(const char *name);
        
    private:
		uint32						_shader_id;
        VectorArray<Shader *>       _shaders;
    }; // ShaderManager
}

#endif
