/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  XMLSourceFactory.inl
@brief XML resource source factory
@author minseob (https://github.com/rasidin)
**********************************************************************/
#pragma once
#include "Factories/ResourceSourceFactory.h"

#include "rapidxml/rapidxml.hpp"

namespace LimitEngine {
class XMLSourceFactory : public ResourceSourceFactory
{
public:
    static constexpr ResourceSourceFactory::ID FactoryID = GENERATE_RESOURCEFACTORY_ID("XMLR");

    XMLSourceFactory() {}
    virtual ~XMLSourceFactory() {}

    virtual ID GetID() const override { return FactoryID; }

    virtual void* ConvertRawData(const void *Data, size_t Size) const
    {
        rapidxml::xml_document<char> *Output = new rapidxml::xml_document<char>();
        Output->parse<0>((char*)Data);
        return (void*)Output;
    }
};
}