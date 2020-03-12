/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2012
-----------------------------------------------------------
@file  LE_ModelFactory.h
@brief Factory for creating models
@author minseob (leeminseob@outlook.com)
-----------------------------------------------------------
History:
- 2017/5/19 Created by minseob
***********************************************************/
#pragma once
#ifndef _LE_MODELFACTORY_H_
#define _LE_MODELFACTORY_H_

#include "LE_ResourceFactory.h"

namespace LimitEngine {
    class ModelFactory : public ResourceFactory 
    {
    public:
        ModelFactory() {}
        virtual ~ModelFactory() {}

        void* Create(const char *format, const void *data, size_t size) override;
        void Release(void *data) override;
        uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }
    private:
        void* createFromText(const char *text);
        void* createFromBinary(const void *data);
    };
}

#endif // _LE_MODELFACTORY_H_
