/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ModelFactory.h
@brief Factory for creating models
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/ModelFactory.h"
#include "Renderer/Model.h"
#include "Core/TextParser.h"

namespace LimitEngine {
void* ModelFactory::Create(const char *format, const void *data, size_t size)
{
    if (strcmp(format, "text") == 0)
        return createFromText(static_cast<const char*>(data));
    else if(strcmp(format, "bin") == 0)
        return createFromBinary(data);
    return NULL;
}
void ModelFactory::Release(void *data)
{
    if (data) {
        delete static_cast<Model*>(data);
    }
}
void* ModelFactory::createFromText(const char *text)
{
    if (text == NULL)
        return NULL;

    Model *output = new Model();
    output->Load(text);
    return output;
}
void* ModelFactory::createFromBinary(const void *data)
{ // unimplemented
    return NULL;
}
}