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
IReferenceCountedObject* ModelFactory::Create(const ResourceSourceFactory *SourceFactory, const FileData &Data)
{
    static ResourceSourceFactory::ID TextParserID = GENERATE_RESOURCEFACTORY_ID("TEPA");

    if (SourceFactory->GetID() == TextParserID) {
        if (TextParser *Parser = (TextParser*)SourceFactory->ConvertRawData(Data.Data, Data.Size)) {
            return dynamic_cast<IReferenceCountedObject*>(Model::GenerateFromTextParser(Parser));
        }
    }
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