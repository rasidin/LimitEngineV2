/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ModelInstance.cpp
 @brief Model Instance Class
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#include "Renderer/ModelInstance.h"

#include "Renderer/Model.h"

namespace LimitEngine {
    void ModelInstance::Draw(const RenderState &rs)
    {
        if (mModel.IsValid() == false)
            return;

        mModel->Draw(rs, mTransform.ToMatrix4x4());
    }
}