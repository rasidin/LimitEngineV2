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
#include "Core/Singleton.h"
#include "Core/String.h"
#include "Containers/MapArray.h"
#include "Containers/VectorArray.h"
#include "Factories/ResourceFactory.h"

/// Input filename format : filename.[type].[format]    ex > testModel.model.text
namespace LimitEngine {

class ResourceLoader : public Object<LimitEngineMemoryCategory_Common>
{
public:
    ResourceLoader() {}
    virtual ~ResourceLoader() {}
    
    virtual bool IsExist(const char *filename) = 0;
    virtual void* GetResource(const char *filename, size_t *size) = 0;
};

class ResourceManager;
typedef Singleton<ResourceManager, LimitEngineMemoryCategory_Common> SingletonResourceManager;
class ResourceManager : public SingletonResourceManager
{
public:
    typedef struct _RESOURCE : public Object<LimitEngineMemoryCategory_Common>
    {
        friend ResourceManager;

        String               id;
        ResourceFactory*     factory;
        uint32               type;
        uint32               useCount;
        size_t               size;
        void                *data;
        void                *orgData;
        
        _RESOURCE() : useCount(1u), type(0), data(NULL), orgData(NULL) {}
        _RESOURCE(const String &i, ResourceFactory *f, size_t s, void *d, void *o)
            : id(i), factory(f), useCount(1u), type(f ? f->GetResourceTypeCode() : 0), size(s), data(d), orgData(o)
        {}

        void Release();
    private:
        void ForceRelease();

        ~_RESOURCE() {}
    } RESOURCE;
    
public:
	ResourceManager();
	virtual ~ResourceManager();
    
	// ---------------------------------------------------
	// Get & Set
	// ----------------------------------------------------
	static void  SetRootPath(const char* path) { m_RootPath = path; }

    static char* GetConvertedPath(const char* filename);
    static void  SetPathTag(const char *key, const char *value);
    
    AutoPointer<RESOURCE> GetResourceWithoutRegister(const char* filename, ResourceFactory *customFactory = NULL)
    { return AutoPointer<RESOURCE>(getResource(filename, false, customFactory)); }
    const RESOURCE* GetResourceWithRegister(const char *filename, ResourceFactory *customFactory = NULL)
    { return getResource(filename, true, customFactory); }

    bool IsExist(const char *filename);

    void  ReleaseResource(const char* filename);
    void  ReleaseResource(void *data);
    void  ReleaseAll();

protected:
    RESOURCE* getResource(const char* filename, bool isRegister, ResourceFactory *customFactory);
private:
    void registerFactories();
    void unregisterFactories();

private:
    static MapArray<String, String>                   m_PathTable;
    static String					                  m_RootPath;

    VectorArray<RESOURCE*>                            m_Resources;
    ResourceLoader                                   *m_Loader;
    MapArray<ResourceFactory::ID, ResourceFactory*>   m_Factories;
};
#define LE_ResourceManager LimitEngine::ResourceManager::GetSingleton()
}
