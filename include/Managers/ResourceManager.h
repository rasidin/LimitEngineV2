/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ResourceManager.h
 @brief Resource Manager for LimitEngine
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once
#include <LERenderer>

#include "Core/AutoPointer.h"
#include "Core/SerializableResource.h"
#include "Core/Singleton.h"
#include "Core/String.h"
#include "Core/ReferenceCountedObject.h"
#include "Containers/MapArray.h"
#include "Containers/VectorArray.h"
#include "Factories/ResourceFactory.h"

/// Input filename format : filename.[type].[format]    ex > testModel.model.text
namespace LimitEngine {

class ResourceLoader : public Object<LimitEngineMemoryCategory::Common>
{
public:
    ResourceLoader() {}
    virtual ~ResourceLoader() {}
    
    virtual bool IsExist(const char *filename) = 0;
    virtual void* GetResource(const char *filename, size_t *size) = 0;
    virtual bool WriteToResource(const char *Filename, uint32 FileType, uint32 FileVersion, void *Data, size_t Size) = 0;
};

class ResourceManager;
typedef Singleton<ResourceManager, LimitEngineMemoryCategory::Common> SingletonResourceManager;
class ResourceManager : public SingletonResourceManager
{
public:
    typedef struct _RESOURCE : public Object<LimitEngineMemoryCategory::Common>
    {
        friend ResourceManager;

        String                      id;
        ResourceFactory*            factory;
        uint32                      type;
        size_t                      size;
        IReferenceCountedObject    *data;
        
        _RESOURCE() : type(0), data(NULL) {}
        _RESOURCE(const String &i, ResourceFactory *f, size_t s, IReferenceCountedObject *d)
            : id(i), factory(f), type(f ? f->GetResourceTypeCode() : 0), size(s), data(d)
        {
            if (data) data->AddReferenceCounter();
        }

        void* PopData() { void *output = data; data = nullptr; return output; }
        void Release();
        void ReleaseResource();
    private:
        virtual ~_RESOURCE()
        {
            ReleaseResource();
        }
        void ForceReleaseResource();
    } RESOURCE;
    
public:
	ResourceManager();
	virtual ~ResourceManager();
    
	// ---------------------------------------------------
	// Get & Set
	// ----------------------------------------------------
	static void  SetRootPath(const char* path) { mRootPath = path; }

    static char* GetConvertedPath(const char* filename);
    static void  SetPathTag(const char *key, const char *value);
    
    AutoPointer<RESOURCE> GetResourceWithoutRegister(const char* Filename, ResourceFactory *Factory)
    { return AutoPointer<RESOURCE>(getResource(Filename, false, Factory)); }
    AutoPointer<RESOURCE> GetResourceWithoutRegister(const char* Filename, ResourceFactory::ID ID)
    { return AutoPointer<RESOURCE>(getResource(Filename, false, findFactory(ID))); }
    const RESOURCE* GetResourceWithRegister(const char *Filename, ResourceFactory *Factory)
    { return getResource(Filename, true, Factory); }
    const RESOURCE* GetResourceWithRegister(const char *Filename, ResourceFactory::ID ID)
    { return getResource(Filename, true, findFactory(ID)); }

    void SaveResource(const char *FilePath, SerializableResource *Resource);

    bool IsExist(const char *filename);

    void AddFactory(ResourceFactory::ID ID, ResourceFactory *Factory) { mFactories.Add(ID, Factory); }
    void AddSourceFactory(const char *Extension, ResourceSourceFactory *Factory) { mSourceFactories.Add(Extension, Factory); }

    ResourceFactory* GetFactory(ResourceFactory::ID ID) { return findFactory(ID); }
    ResourceSourceFactory* GetSourceFactory(const String &ext) { return mSourceFactories.Find(ext); }

protected:
    RESOURCE* getResource(const char* Filename, bool NeedRegister, ResourceFactory *Factory);
    ResourceFactory* findFactory(ResourceFactory::ID ID);
private:
    void registerFactories();
    void unregisterFactories();

private:
    static MapArray<String, String>                                mPathTable;
    static String					                               mRootPath;

    VectorArray<RESOURCE*>                                         mResources;
    ResourceLoader                                                *mLoader;
    MapArray<ResourceFactory::ID, ResourceFactory*>                mFactories;
    MapArray<String, ResourceSourceFactory*>                       mSourceFactories;
};
#define LE_ResourceManager ResourceManager::GetSingleton()
}
