/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderParameterParser.h
 @brief Get parameter in shader source code
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include "Core/Object.h"
#include "Core/String.h"
#include "Containers/MapArray.h"
#include "Containers/VectorArray.h"

namespace LimitEngine {
	class ShaderParameterParser : public Object<LimitEngineMemoryCategory::Common> {
	public:
		struct Parameter {
			MapArray<String, String> attributes;
			String type;
			String value;
		};
		typedef MapArray<String, Parameter> ParameterMap;
		ParameterMap mParameters;

		bool Parse(const char *text);
	};
} // namespace LimitEngine
