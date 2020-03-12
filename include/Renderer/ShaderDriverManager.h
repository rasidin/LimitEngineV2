/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_ShaderDriverManager.h
 @brief Shader driver manager
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/10/31 Created by minseob
 ***********************************************************/
#pragma once
#ifndef _LE_SHADERDRIVERMANAGER_H_
#define _LE_SHADERDRIVERMANAGER_H_

#include <LE_Graphics>

#include "LE_Singleton.h"
#include "LE_VectorArray.h"

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

#endif // _LE_SHADERDRIVERMANAGER_H_