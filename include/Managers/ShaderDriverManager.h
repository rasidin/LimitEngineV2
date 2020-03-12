/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriverManager.h
 @brief Shader driver manager
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>
#include <LEManagers>

#include "Core/Singleton.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
	typedef Singleton<ShaderDriverManager, LimitEngineMemoryCategory_Graphics> SingletonShaderDriverManager;
	class ShaderDriverManager : public SingletonShaderDriverManager
	{
	public:
		ShaderDriverManager();
		~ShaderDriverManager();

		void SetupShaderDriver(Shader *shader, const char *code);
	private:
		VectorArray<ShaderDriver*>		mDrivers;
	};
}
