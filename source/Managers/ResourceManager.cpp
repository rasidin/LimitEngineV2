/*****************************************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file	ResourceManager.cpp
 * @brief	LIMITEngine Resource Manager
 * @author	minseob
 *****************************************************************************/

#include "Managers/ResourceManager.h"
#include "Core/Data.h"
#include "Core/Util.h"
#include "Core/SerializableResource.h"

#include "Factories/ResourceFactory.h"
#include "Factories/ResourceSourceFactory.h"

#include "Factories/ArchiveFactory.h"
#include "Factories/ModelFactory.h"
#include "Factories/TextureFactory.h"
#include "Factories/TextParserFactory.h"
#include "Factories/ShaderFactory.h"

#include "../Factories/SourceFactories/TGASourceFactory.inl"
#include "../Factories/SourceFactories/TextParserSourceFactory.inl"

#ifdef IOS
#include "IOS/LE_ResourceLoader_IOS.h"
#elif defined(WINDOWS)
#include "../Platform/Windows/ResourceLoader_Windows.inl"
#else
#error no implemented ResourceLoader for this platform.
#endif

namespace LimitEngine {
	String ResourceManager::mRootPath;
#ifdef WIN32
	ResourceManager* SingletonResourceManager::mInstance = NULL;
#endif
	MapArray<String, String> ResourceManager::mPathTable;
    
    void ResourceManager::_RESOURCE::ForceRelease()
    {
        if (useCount) {
            useCount = 0;
            Release();
        }
    }
    void ResourceManager::_RESOURCE::Release()
    {
        if (useCount) useCount--;
        if (useCount == 0) {
            if (data) {
                if (factory) {
                    factory->Release(data);
                }
                else {
                    free(data);
                }
                data = NULL;
            }

            if (orgData) {
                free(orgData);
                orgData = NULL;
            }
            delete this;
        }
    }

    ResourceManager::ResourceManager()
    : mLoader(0)
    {
#if	  defined(IOS)
        m_Loader = new ResourceLoader_IOS();
#elif defined(WINDOWS)
        mLoader = new ResourceLoader_Windows();
#else
#error no implemented ResourceLoader for this platform.
#endif

        // Register factories....
        registerFactories();
    }
    ResourceManager::~ResourceManager()
    {
        for(uint32 i=0;i<mResources.GetSize();i++)
        {
            mResources[i]->Release();
        }
		mResources.Clear();
        if (mLoader) delete mLoader;
        unregisterFactories();
    }
    void ResourceManager::registerFactories()
    {
        mFactories.Add(ArchiveFactory::ID,    new ArchiveFactory());
        mFactories.Add(ModelFactory::ID,      new ModelFactory());
        mFactories.Add(ShaderFactory::ID,     new ShaderFactory());
        mFactories.Add(TextureFactory::ID,    new TextureFactory());
        mFactories.Add(TextParserFactory::ID, new TextParserFactory());

        mSourceFactories.Add("tga",  new TGASourceFactory());
        mSourceFactories.Add("text", new TextParserSourceFactory());
    }
    void ResourceManager::unregisterFactories()
    {
        for (uint32 facidx = 0; facidx < mFactories.size(); facidx++) {
            ResourceFactory *factory = mFactories.GetAt(facidx).value;
            if (factory == NULL)
                continue;
            for (uint32 sfidx = facidx + 1; sfidx < mFactories.size(); sfidx++) {
                if (mFactories.GetAt(sfidx).value == factory) {
                    mFactories.GetAt(sfidx).value = NULL;
                }
            }
            delete factory;
        }
        mFactories.Clear();
    }
    void ResourceManager::SetPathTag(const char *key, const char *value)
    {
        mPathTable[key] = value;
    }
    char* ResourceManager::GetConvertedPath(const char *filename)
    {
        String convertedPath = mRootPath;
        size_t length = ::strlen(filename);
        String convertWord;
        bool inTag = false;
        if (!mRootPath.IsEmpty()) convertedPath += "\\";
        for (size_t i=0;i<length;i++)
        {
            if (filename[i] == '<')
            {
                inTag = true;
                continue;
            }
            else if (filename[i] == '>')
            {
                inTag = false;
                int tagIndex = 0;
                if ((tagIndex = mPathTable.FindIndex(convertWord)) >= 0)
                {
                    convertedPath += mPathTable.GetAt(tagIndex).value;
					convertedPath += "\\";
                }
                convertWord = "";
                continue;
            }
            else 
            {
                if (inTag)
                {
                    convertWord += filename[i];
                }
                else 
                {
                    convertedPath += filename[i];
                }
            }
        }
        return convertedPath.GetCopiedCharPtr();
    }
    bool ResourceManager::IsExist(const char *filename)
    {
        if(mLoader == NULL) return false;
        return mLoader->IsExist(filename);
    }
    ResourceManager::RESOURCE* ResourceManager::getResource(const char* Filename, bool NeedRegister, ResourceFactory *Factory)
    {
        LEASSERT(mLoader);
        if (!mLoader) return nullptr;
        if (!Factory) return nullptr;

        // Find loaded resources
        if (NeedRegister) {
            for (uint32 residx = 0; residx < mResources.count(); residx++) {
                if (mResources[residx]->id == Filename) {
                    mResources[residx]->useCount++;
                    return mResources[residx];
                }
            }
        }

        char *filenameonly = static_cast<char*>(malloc(strlen(Filename) + 1));
        ::memset(filenameonly, 0, strlen(Filename) + 1);
        GetFileName(Filename, true, filenameonly);

        char fileFormat[16]; fileFormat[0] = 0;
        GetFormatFromFileName(filenameonly, fileFormat);

        char *convertedPath = GetConvertedPath(Filename);
        size_t size = 0;
        if (void *data = mLoader->GetResource(convertedPath, &size)) {
            void *createdData = NULL;
            ResourceSourceFactory *sourceFactory = mSourceFactories.Find(fileFormat);
            if (createdData = Factory->Create(sourceFactory, data, size)) {
                RESOURCE *newRes = new RESOURCE(Filename, Factory, size, createdData, data);
                if (NeedRegister)
                    mResources.Add(newRes);
                free(convertedPath);
                return newRes;
            }
        }
        free(filenameonly);
		free(convertedPath);
		return NULL;
    }
    ResourceFactory* ResourceManager::findFactory(ResourceFactory::ID ID)
    {
        return mFactories.Find(ID);
    }
    void ResourceManager::ReleaseResource(const char* filename)
    {
        for(uint32 i=0;i<mResources.GetSize();i++)
        {
            if (mResources[i]->id == filename)
            {
                mResources[i]->useCount--;
                if (mResources[i]->useCount == 0)
                {
                    mResources[i]->Release();
                    mResources.Delete(i);
                }
            }
        }
    }
    void ResourceManager::ReleaseResource(void *data)
    {
        for(uint32 i=0;i<mResources.GetSize();i++)
        {
            if (mResources[i]->data == data)
            {
                mResources[i]->useCount--;
                if (mResources[i]->useCount == 0)
                {
					mResources[i]->Release();
                    mResources.Delete(i);
                }
            }
        }
    }
    void ResourceManager::ReleaseAll()
    {
        for (uint32 i = 0; i < mResources.GetSize(); i++)
        {
            mResources[i]->Release();
        }
        mResources.Clear();
    }
    void ResourceManager::SaveResource(const char *FilePath, SerializableResource *Resource)
    {
        if (!Resource || !FilePath) return;

        Archive OutArchive;
        OutArchive << Resource;
        
        char *convertedPath = GetConvertedPath(FilePath);
        mLoader->WriteToResource(convertedPath, Resource->GetFileType(), Resource->GetFileType(), OutArchive.mData, OutArchive.mDataSize);
        free(convertedPath);
    }
}