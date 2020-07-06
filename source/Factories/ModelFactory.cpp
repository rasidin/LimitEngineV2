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

#include "rapidxml/rapidxml.hpp"

namespace LimitEngine {
IReferenceCountedObject* ModelFactory::Create(const ResourceSourceFactory *SourceFactory, const FileData &Data)
{
    static constexpr ResourceSourceFactory::ID TextParserID = GENERATE_RESOURCEFACTORY_ID("TEPA");
    static constexpr ResourceSourceFactory::ID XMLParserID = GENERATE_RESOURCEFACTORY_ID("XMLR");

    if (SourceFactory->GetID() == TextParserID) {
        if (TextParser *Parser = (TextParser*)SourceFactory->ConvertRawData(Data.Data, Data.Size)) {
            return dynamic_cast<IReferenceCountedObject*>(Model::GenerateFromTextParser(ReferenceCountedPointer<TextParser>(Parser)));
        }
    }
    else if (SourceFactory->GetID() == XMLParserID) {
        if (rapidxml::xml_document<const char> *XMLDoc = (rapidxml::xml_document<const char>*)SourceFactory->ConvertRawData(Data.Data, Data.Size)) {
            IReferenceCountedObject *Output = dynamic_cast<IReferenceCountedObject*>(Model::GenerateFromXML(XMLDoc));
            delete XMLDoc;
            return Output;
        }
    }
    return NULL;
}
void ModelFactory::Release(IReferenceCountedObject *data)
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