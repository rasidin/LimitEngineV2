/*****************************************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file	ResourceManager.h
 * @brief	LIMITEngine Resource Manager
 * @author	minseob
 *****************************************************************************/

#include "Managers/ResourceManager.h"
#include "Core/Data.h"
#include "Core/Util.h"
#include "Core/SerializableResource.h"

#include "Factories/ArchiveFactory.h"
#include "Factories/ModelFactory.h"
#include "Factories/TextureFactory.h"
#include "Factories/ShaderFactory.h"

#ifdef IOS
#include "IOS/LE_ResourceLoader_IOS.h"
#elif defined(WINDOWS)
#include "../Platform/Windows/ResourceLoader_Windows.inl"
#else
#error no implemented ResourceLoader for this platform.
#endif

namespace LimitEngine {
	String ResourceManager::m_RootPath;
#ifdef WIN32
	ResourceManager* SingletonResourceManager::mInstance = NULL;
#endif
	MapArray<String, String> ResourceManager::m_PathTable;
    
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
    : m_Loader(0)
    {
#if	  defined(IOS)
        m_Loader = new ResourceLoader_IOS();
#elif defined(WINDOWS)
        m_Loader = new ResourceLoader_Windows();
#else
#error no implemented ResourceLoader for this platform.
#endif

        // Register factories....
        registerFactories();
    }
    ResourceManager::~ResourceManager()
    {
        for(size_t i=0;i<m_Resources.GetSize();i++)
        {
            m_Resources[i]->Release();
        }
		m_Resources.Clear();
        if (m_Loader) delete m_Loader;
        unregisterFactories();
    }
    void ResourceManager::registerFactories()
    {
        m_Factories.Add(ResourceFactory::ID("", "lea"), new ArchiveFactory());
        m_Factories.Add(ResourceFactory::ID("model", ""), new ModelFactory());
		m_Factories.Add(ResourceFactory::ID("shader", ""), new ShaderFactory());
        TextureFactory *textureFactory = new TextureFactory();
        m_Factories.Add(ResourceFactory::ID("", "png"), textureFactory);
        m_Factories.Add(ResourceFactory::ID("", "tga"), textureFactory);
        m_Factories.Add(ResourceFactory::ID("", "tif"), textureFactory);
        m_Factories.Add(ResourceFactory::ID("", "hdr"), textureFactory);
        m_Factories.Add(ResourceFactory::ID("", "dds"), textureFactory);
		m_Factories.Add(ResourceFactory::ID("brdf", ""), textureFactory);
    }
    void ResourceManager::unregisterFactories()
    {
        for (uint32 facidx = 0; facidx < m_Factories.size(); facidx++) {
            ResourceFactory *factory = m_Factories.GetAt(facidx).value;
            if (factory == NULL)
                continue;
            for (uint32 sfidx = facidx + 1; sfidx < m_Factories.size(); sfidx++) {
                if (m_Factories.GetAt(sfidx).value == factory) {
                    m_Factories.GetAt(sfidx).value = NULL;
                }
            }
            delete factory;
        }
        m_Factories.Clear();
    }
    void ResourceManager::SetPathTag(const char *key, const char *value)
    {
        m_PathTable[key] = value;
    }
    char* ResourceManager::GetConvertedPath(const char *filename)
    {
        String convertedPath = m_RootPath;
        size_t length = ::strlen(filename);
        String convertWord;
        bool inTag = false;
        if (!m_RootPath.IsEmpty()) convertedPath += "\\";
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
                if ((tagIndex = m_PathTable.FindIndex(convertWord)) >= 0)
                {
                    convertedPath += m_PathTable.GetAt(tagIndex).value;
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
        if(m_Loader == NULL) return false;
        return m_Loader->IsExist(filename);
    }
    ResourceManager::RESOURCE* ResourceManager::getResource(const char* filename, bool isRegister, ResourceFactory *customFactory)
    {
        LEASSERT(m_Loader);
        if (!m_Loader) return NULL;

        // Find loaded resources
        if (isRegister) {
            for (uint32 residx = 0; residx < m_Resources.count(); residx++) {
                if (m_Resources[residx]->id == filename) {
                    m_Resources[residx]->useCount++;
                    return m_Resources[residx];
                }
            }
        }

        char *filenameonly = static_cast<char*>(malloc(strlen(filename) + 1));
        ::memset(filenameonly, 0, strlen(filename) + 1);
        GetFileName(filename, true, filenameonly);

        char fileType[16]; fileType[0] = 0;
        GetTypeFromFileName(filenameonly, fileType);
        char fileFormat[16]; fileFormat[0] = 0;
        GetFormatFromFileName(filenameonly, fileFormat);

        char *convertedPath = GetConvertedPath(filename);
        size_t size = 0;
        if (void *data = m_Loader->GetResource(convertedPath, &size)) {
            ResourceFactory *factory = customFactory;
            if(factory == NULL)
                factory = m_Factories.Find(ResourceFactory::ID(fileType, fileFormat));
            void *createdData = NULL;
            if (factory) {
                createdData = factory->Create(fileFormat, data, size);
            }
            else {
                createdData = data;
                data = NULL;
            }
            RESOURCE *newRes = new RESOURCE(filename, factory, size, createdData, data);
            if (isRegister)
                m_Resources.Add(newRes);
			free(convertedPath);
            return newRes;
        }
        free(filenameonly);
		free(convertedPath);
		return NULL;
    }
    void ResourceManager::ReleaseResource(const char* filename)
    {
        for(size_t i=0;i<m_Resources.GetSize();i++)
        {
            if (m_Resources[i]->id == filename)
            {
                m_Resources[i]->useCount--;
                if (m_Resources[i]->useCount == 0)
                {
                    m_Resources[i]->Release();
                    m_Resources.Delete(i);
                }
            }
        }
    }
    void ResourceManager::ReleaseResource(void *data)
    {
        for(size_t i=0;i<m_Resources.GetSize();i++)
        {
            if (m_Resources[i]->data == data)
            {
                m_Resources[i]->useCount--;
                if (m_Resources[i]->useCount == 0)
                {
					m_Resources[i]->Release();
                    m_Resources.Delete(i);
                }
            }
        }
    }
    void ResourceManager::ReleaseAll()
    {
        for (size_t i = 0; i < m_Resources.GetSize(); i++)
        {
            m_Resources[i]->Release();
        }
        m_Resources.Clear();
    }
    void ResourceManager::SaveResource(const char *FilePath, SerializableResource *Resource)
    {
        if (!Resource || !FilePath) return;

        Archive OutArchive;
        OutArchive << Resource;
        Resource->Serialize(OutArchive);
        
        char *convertedPath = GetConvertedPath(FilePath);
        m_Loader->WriteToResource(convertedPath, Resource->GetFileType(), Resource->GetFileType(), OutArchive.mData, OutArchive.mDataSize);
        free(convertedPath);
    }
}