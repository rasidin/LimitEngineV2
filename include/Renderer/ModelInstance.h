/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ModelInstance.h
 @brief Model Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include "Core/ReferenceCountedObject.h"
#include "RenderState.h"
#include "Transform.h"

namespace LimitEngine {
    class ModelInstance : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>
    {
    public:
        explicit ModelInstance(uint32 ID, const ModelRefPtr &InModel) 
            : mID(ID)
            , mTransform()
            , mModel(InModel) 
        {}
        virtual ~ModelInstance() {}

        uint32 GetInstanceID() const { return mID; }
        void SetTransform(const Transform &InTransform) { mTransform = InTransform; }

        void Draw(const RenderState &rs);

    private:
        uint32      mID;
        Transform   mTransform;
        ModelRefPtr mModel;
    };
}