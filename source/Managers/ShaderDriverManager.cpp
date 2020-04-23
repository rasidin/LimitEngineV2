/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderDriverManager.h
 @brief Shader driver manager
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Managers/ShaderDriverManager.h"

#include "Renderer/ShaderParameterParser.h"

#include "Renderer/ShaderDriverGeneral.h"
#include "Renderer/ShaderDriverParameter.h"
#include "Renderer/ShaderDriverLight.h"
#include "Renderer/ShaderDriverBakedBRDF.h"
#include "Renderer/Shader.h"

namespace LimitEngine {
	ShaderDriverManager* SingletonShaderDriverManager::mInstance = NULL;
	ShaderDriverManager::ShaderDriverManager()
	{
		mDrivers.Add(new ShaderDriverGeneral);
		//mDrivers.Add(new ShaderDriverParameter);
        mDrivers.Add(new ShaderDriverLight);
		//mDrivers.Add(new ShaderDriverBakedBRDF);
	}
	ShaderDriverManager::~ShaderDriverManager()
	{
		for(uint32 sdidx=0;sdidx<mDrivers.count();sdidx++) {
			delete mDrivers[sdidx];
		}
		mDrivers.Clear();
	}
    void ShaderDriverManager::SetupShaderDriver(Shader *shader, const char *code /*= nullptr*/)
	{
        if (code == NULL) {
            for (uint32 sdidx = 0; sdidx < mDrivers.count(); sdidx++) {
                if (mDrivers[sdidx]->IsValid(shader)) {
                    shader->AddDriver(mDrivers[sdidx]->Create(shader));
                }
            }
        }
        else {
            ShaderParameterParser parser;
            if (parser.Parse(code)) {
                for (uint32 sdidx = 0; sdidx < mDrivers.count(); sdidx++) {
                    if (mDrivers[sdidx]->IsValid(parser.mParameters)) {
                        shader->AddDriver(mDrivers[sdidx]->Create(shader, parser.mParameters));
                    }
                }
            }
        }
	}
}