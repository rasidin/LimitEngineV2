/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  ModelFactory.h
@brief model factory
@author minseob(https://github.com/rasidin)
*********************************************************************/
#ifndef LIMITENGINEV2_FACTORIES_MODELFACTORY_H_
#define LIMITENGINEV2_FACTORIES_MODELFACTORY_H_

#include "ResourceFactory.h"

namespace LimitEngine {
class ModelFactory : public ResourceFactory 
{
public:
    static constexpr ResourceFactory::ID ID = GENERATE_RESOURCEFACTORY_ID("MODE");

    ModelFactory() {}
    virtual ~ModelFactory() {}

    SerializableRendererResource* Create(const ResourceSourceFactory *format, const FileData &Data) override;
    void Release(SerializableRendererResource*data) override;
    uint32 GetResourceTypeCode() override { return makeResourceTypeCode("LMDL"); }

private:
    void* createFromText(const char *text);
    void* createFromBinary(const void *data);
};
}

#endif // LIMITENGINEV2_FACTORIES_MODELFACTORY_H_