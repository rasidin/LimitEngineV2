/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_ShaderDriverGeneral.h
 @brief General driver for shader
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2016/10/28 Created by minseob
 ***********************************************************/
#pragma once

#include <LERenderer>
#include "Material.h"
#include "RenderState.h"
#include "ShaderDriver.h"
#include "ShaderParameterParser.h"

namespace LimitEngine {
    class ShaderDriverGeneral : public ShaderDriver
    {
    public:
        bool IsValid(const ShaderParameterParser::ParameterMap &paramMap) override;
        void Apply(const RenderState &rs, const Material *material) override;
        const char* GetName() const override { return "ShaderDriverGeneral"; }
    private:
        ShaderDriver* create() override { return new ShaderDriverGeneral; }
        void setup(const ShaderParameterParser::ParameterMap &paramMap) override;
    private:
        int mParameter_wvpMat;
        int mParameter_invViewMat;
        int mParameter_worldMat;
        int mParameter_eyePos;
    };
} // namespace LimitEngine
