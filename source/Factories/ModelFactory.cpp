/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  ModelFactory.h
@brief Factory for creating models
@author minseob (leeminseob@outlook.com)
***********************************************************/
#include "Factories/ModelFactory.h"
#include "Factories/ResourceSourceFactory.h"
#include "Renderer/Model.h"
#include "Core/TextParser.h"

namespace LimitEngine {
void* ModelFactory::Create(const ResourceSourceFactory *SourceFactory, const void *data, size_t size)
{
    static ResourceSourceFactory::ID TextParserID = GENERATE_RESOURCEFACTORY_ID("TEPA");

    if (SourceFactory->GetID() == TextParserID) {
        if (TextParser *Parser = (TextParser*)SourceFactory->ConvertRawData(data, size)) {
            return dynamic_cast<void*>(Model::GenerateFromTextParser(Parser));
        }
    }

    //if (strcmp(format, "text") == 0)
    //    return createFromText(static_cast<const char*>(data));
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
}