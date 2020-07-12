/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LE_Texture.cpp
 @brief Texture
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include <LEFloatVector3.h>
#include <LEIntVector4.h>

#include "Renderer/Texture.h"
#include "Managers/DrawManager.h"

#ifdef USE_DX9
#include "DirectX9/LE_TextureImpl_DirectX9.h"
#elif defined(USE_DX11)
#include "../Platform/DirectX11/TextureImpl_DirectX11.inl"
#elif defined (USE_OPENGLES)
#include "OpenGLES/LE_TextureImpl_OpenGLES.h"
#else
#error No implementation for texture
#endif

namespace LimitEngine {
template<> Archive& Archive::operator << (SerializedTextureSource &InSource) {
    *this << InSource.mSize;
    *this << InSource.mRowPitch;
    *this << InSource.mMipCount;
    *this << InSource.mFormat;
    *this << (SerializableResource*)&InSource.mColorData;
    return *this;
}

SerializedTextureSource::SerializedTextureSource(const TextureSourceImage &SourceImage)
{
    mSize = LEMath::IntVector3(SourceImage.GetSize().Width(), SourceImage.GetSize().Height(), SourceImage.GetDepth());
    mRowPitch = SourceImage.GetRowPitch();
    mMipCount = SourceImage.GetMipCount();
    mFormat = SourceImage.GetFormat();
    mColorData.Resize(static_cast<uint32>(SourceImage.GetColorDataSize()));
    memcpy(&mColorData.GetStart(), SourceImage.GetColorData(), SourceImage.GetColorDataSize());
    mIsCubemap = SourceImage.IsCubemap();
}
SerializedTextureSource::~SerializedTextureSource()
{
}

class RendererTask_LoadTextureFromMemory : public RendererTask
{
public:
    RendererTask_LoadTextureFromMemory(Texture *owner, const void *data, size_t size)
        : mOwner(owner)
        , mData(NULL)
        , mSize(size)
    {
        if (size) {
            mData = malloc(size);
            ::memcpy(mData, data, size);
        }
    }
    virtual ~RendererTask_LoadTextureFromMemory()
    {
        if (mData) {
            free(mData);
            mData = NULL;
        }
    }
    void Run() override
    {
        if (mOwner->mImpl) {
            mOwner->mImpl->LoadFromMemory(mData, mSize);
            mOwner->mSize = mOwner->mImpl->GetSize();
            mOwner->mFormat = mOwner->mImpl->GetFormat();
        }
    }
private:
    Texture *mOwner;
    void *mData;
    size_t mSize;
};
class RendererTask_LoadTextureFromMERLBRDFData : public RendererTask
{
public:
	RendererTask_LoadTextureFromMERLBRDFData(Texture *owner, const void *data, size_t size)
		: mOwner(owner)
		, mData(NULL)
		, mSize(size)
	{
		if (data && size) {
			mData = malloc(size);
			::memcpy(mData, data, size);
		}
	}
	virtual ~RendererTask_LoadTextureFromMERLBRDFData()
	{
		if (mData) {
			free(mData);
			mData = NULL;
		}
	}
	void Run() override
	{
		if (mOwner->mImpl == NULL || mData == NULL) return;

		uint8 *dataPtr = static_cast<uint8*>(mData);
		// Generate texture data from MERL BRDF Binary
		int *dimensions = (int*)dataPtr;
		dataPtr += sizeof(int) * 3;

		auto GetTextureSize = [](int x) {
			int output = 1;
			while (output < x) {
				output <<= 1;
			}
			return output;
		};

		uint32 TextureSizeX = GetTextureSize(dimensions[0]);
		uint32 TextureSizeY = GetTextureSize(dimensions[1]);
		uint32 TextureSizeZ = GetTextureSize(dimensions[2]);

		if (TextureSizeX == 0 || TextureSizeY == 0 || TextureSizeZ == 0) return;

		auto SampleBRDFData = [dimensions](LEMath::FloatVector3 uvw, double *brdfSampleData) {
			float output = 0;
			LEMath::IntVector4 brdfUVWi(
				int32(uvw.X()),
				int32(uvw.Y()),
				int32(uvw.Z()),
                0);
            LEMath::IntVector4 brdfUVWi1 = (brdfUVWi + 1) % LEMath::IntVector4(dimensions[0], dimensions[1], dimensions[2], 1);
			LEMath::FloatVector3 brdfUVWf(
				uvw.X() - brdfUVWi.X(),
				uvw.Y() - brdfUVWi.Y(),
				uvw.Z() - brdfUVWi.Z());

			double brdf000 = brdfSampleData[brdfUVWi.X()  + brdfUVWi.Y()  * dimensions[0] + brdfUVWi.Z()  * dimensions[0] * dimensions[1]];
			double brdf100 = brdfSampleData[brdfUVWi1.X() + brdfUVWi.Y()  * dimensions[0] + brdfUVWi.Z()  * dimensions[0] * dimensions[1]];
			double brdf010 = brdfSampleData[brdfUVWi.X()  + brdfUVWi1.Y() * dimensions[0] + brdfUVWi.Z()  * dimensions[0] * dimensions[1]];
			double brdf110 = brdfSampleData[brdfUVWi1.X() + brdfUVWi1.Y() * dimensions[0] + brdfUVWi.Z()  * dimensions[0] * dimensions[1]];
			double brdf001 = brdfSampleData[brdfUVWi.X()  + brdfUVWi.Y()  * dimensions[0] + brdfUVWi1.Z() * dimensions[0] * dimensions[1]];
			double brdf101 = brdfSampleData[brdfUVWi1.X() + brdfUVWi.Y()  * dimensions[0] + brdfUVWi1.Z() * dimensions[0] * dimensions[1]];
			double brdf011 = brdfSampleData[brdfUVWi.X()  + brdfUVWi1.Y() * dimensions[0] + brdfUVWi1.Z() * dimensions[0] * dimensions[1]];
			double brdf111 = brdfSampleData[brdfUVWi1.X() + brdfUVWi1.Y() * dimensions[0] + brdfUVWi1.Z() * dimensions[0] * dimensions[1]];

			double brdfX00 = brdf000 * (1.0f - brdfUVWf.X()) + brdf100 * brdfUVWf.X();
			double brdfX10 = brdf010 * (1.0f - brdfUVWf.X()) + brdf110 * brdfUVWf.X();
			double brdfX01 = brdf001 * (1.0f - brdfUVWf.X()) + brdf101 * brdfUVWf.X();
			double brdfX11 = brdf011 * (1.0f - brdfUVWf.X()) + brdf111 * brdfUVWf.X();
                                                                                  
			double brdfXY0 = brdfX00 * (1.0f - brdfUVWf.Y()) + brdfX10 * brdfUVWf.Y();
			double brdfXY1 = brdfX01 * (1.0f - brdfUVWf.Y()) + brdfX11 * brdfUVWf.Y();

			output = static_cast<float>(brdfXY0 * (1.0 - static_cast<double>(brdfUVWf.Z())) + brdfXY1 * static_cast<double>(brdfUVWf.Z()));

			return output;
		};

		double *brdfOrgData = (double*)dataPtr;
		double *brdfOrgDataR = brdfOrgData;
		double *brdfOrgDataG = brdfOrgData + dimensions[0] * dimensions[1] * dimensions[2];
		double *brdfOrgDataB = brdfOrgData + dimensions[0] * dimensions[1] * dimensions[2] * 2;
		LEMath::FloatVector4* brdfData = (LEMath::FloatVector4*)::malloc(TextureSizeX * TextureSizeY * TextureSizeZ * sizeof(LEMath::FloatVector4));
		for (uint32 Z = 0; Z < TextureSizeZ; Z++) {
			for (uint32 Y = 0; Y < TextureSizeY; Y++) {
				for (uint32 X = 0; X < TextureSizeX; X++) {
					uint32 TextureOffset = X + Y * TextureSizeX + Z * TextureSizeX * TextureSizeY;
                    LEMath::FloatVector3 brdfUVW(
						float(dimensions[0]) * X / TextureSizeX,
						float(dimensions[1]) * Y / TextureSizeY,
						float(dimensions[2]) * Z / TextureSizeZ	);
					brdfData[TextureOffset].SetX(SampleBRDFData(brdfUVW, brdfOrgDataR));
					brdfData[TextureOffset].SetY(SampleBRDFData(brdfUVW, brdfOrgDataG));
					brdfData[TextureOffset].SetZ(SampleBRDFData(brdfUVW, brdfOrgDataB));
					brdfData[TextureOffset].SetW(1.0f);
				}
			}
		}

		if (mOwner) {
			mOwner->mImpl->Create3D(LEMath::IntSize3(TextureSizeX, TextureSizeY, TextureSizeZ), TEXTURE_COLOR_FORMAT_A32B32G32R32F, 0, 1, brdfData, TextureSizeX * TextureSizeY * TextureSizeZ * sizeof(LEMath::FloatVector4));
		}
	}
private:
	Texture *mOwner;
	void *mData;
	size_t mSize;
};
class RendererTask_CreateTexture : public RendererTask
{
public:
    RendererTask_CreateTexture(Texture *owner, LEMath::IntSize size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initData, size_t initDataSize)
        : mOwner(owner)
        , mSize(size)
        , mFormat(format)
        , mUsage(usage)
        , mMipLevels(mipLevels)
		, initializeData(initData)
		, initializeDataSize(initDataSize)
    {}
    void Run() override
    {
        if (mOwner) {
            mOwner->mImpl->Create(mSize, mFormat, mUsage, mMipLevels, initializeData, initializeDataSize);
        }
    }
private:
    Texture *mOwner;
    LEMath::IntSize mSize;
    TEXTURE_COLOR_FORMAT mFormat;
    uint32 mUsage;
    uint32 mMipLevels;
	void *initializeData;
	size_t initializeDataSize;
};
class RendererTask_CreateTexture3D : public RendererTask
{
public:
	RendererTask_CreateTexture3D(Texture *owner, LEMath::IntSize3 size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initData, size_t initDataSize)
		: mOwner(owner)
		, mSize(size)
		, mFormat(format)
		, mUsage(usage)
		, mMipLevels(mipLevels)
		, mInitializeData(initData)
		, mInitializeDataSize(initDataSize)
	{}
	void Run() override
	{
		if (mOwner) {
			mOwner->mImpl->Create3D(mSize, mFormat, mUsage, mMipLevels, mInitializeData, mInitializeDataSize);
		}
	}
private:
	Texture *mOwner;
	LEMath::IntSize3 mSize;
	TEXTURE_COLOR_FORMAT mFormat;
	uint32 mUsage;
	uint32 mMipLevels;
	void *mInitializeData;
	size_t mInitializeDataSize;
};
class RendererTask_CreateScreenColor : public RendererTask
{
public:
    RendererTask_CreateScreenColor(Texture *owner, const LEMath::IntSize &size)
        : mOwner(owner)
        , mSize(size)
    {}
    void Run() override
    {
        if (mOwner) {
            mOwner->mImpl->CreateScreenColor(mSize);
            mOwner->mFormat = mOwner->mImpl->GetFormat();
        }
    }
private:
    Texture *mOwner;
    LEMath::IntSize mSize;
};
class RendererTask_CreateDepthStencil : public RendererTask
{
public:
    RendererTask_CreateDepthStencil(Texture *owner, LEMath::IntSize size, TEXTURE_DEPTH_FORMAT format)
        : mOwner(owner)
        , mSize(size)
        , mFormat(format)
    {}
    void Run() override
    {
        if (mOwner) {
            mOwner->mImpl->CreateDepthStencil(mSize, mFormat);
            mOwner->mFormat = mOwner->mImpl->GetFormat();
        }
    }
private:
    Texture *mOwner;
    LEMath::IntSize mSize;
    TEXTURE_DEPTH_FORMAT mFormat;
};
class RendererTask_CreateColor : public RendererTask
{
public:
    RendererTask_CreateColor(Texture *owner, const LEMath::IntSize &size, const ByteColorRGBA &color)
        : mOwner(owner)
        , mSize(size)
        , mColor(color)
    {}
    void Run() override
    {
        if (mOwner) {
            mOwner->mImpl->CreateColor(mSize, mColor);
            mOwner->mFormat = mOwner->mImpl->GetFormat();
        }
    }
private:
    Texture *mOwner;
    LEMath::IntSize mSize;
    ByteColorRGBA mColor;
};
class RendererTask_CreateRenderTarget : public RendererTask
{
public:
    RendererTask_CreateRenderTarget(Texture *owner, const LEMath::IntSize size, TEXTURE_COLOR_FORMAT format, uint32 usage)
        : mOwner(owner)
        , mSize(size)
        , mFormat(format)
        , mUsage(usage)
    {}
    void Run() override
    {
        if (mOwner) {
            mOwner->mImpl->CreateRenderTarget(mSize, mFormat, mUsage);
        }
    }
private:
    Texture *mOwner;
    LEMath::IntSize mSize;
    TEXTURE_COLOR_FORMAT mFormat;
    uint32 mUsage;
};
Texture* Texture::GenerateFromSourceImage(const TextureSourceImage *SourceImage)
{
    Texture *output = new Texture();
    output->SetSourceImage(new SerializedTextureSource(*SourceImage));
    return output;
}
Texture::Texture()
    : mImpl(0)
    , mSize(0)
	, mDepth(1)
    , mFormat(TEXTURE_COLOR_FORMAT_UNKNOWN)
    , mSource(nullptr)
{
#ifdef USE_DX9
    mImpl = new TextureImpl_DirectX9();
#elif defined(USE_DX11)
    mImpl = new TextureImpl_DirectX11();
#elif defined(USE_OPENGLES)
    mImpl = new TextureImpl_OpenGLES();
#else
#error No implementation for texture
#endif
}
Texture::~Texture()
{
    if (mSource) delete mSource;
    mSource = nullptr;

    if (mImpl) delete mImpl;
    mImpl = nullptr;
}
void Texture::InitResource()
{
    CreateUsingSourceData();
}
void Texture::Create(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initailizeData, size_t initDataSize)
{ 
    AutoPointer<RendererTask> rt_createTexture = new RendererTask_CreateTexture(this, size, format, usage, mipLevels, initailizeData, initDataSize);
    LE_DrawManager.AddRendererTask(rt_createTexture);
    mSize = size; 
    mFormat = format; 
}
void Texture::Create3D(const LEMath::IntSize3 &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initializeData, size_t initDataSize)
{
	AutoPointer <RendererTask> rt_createTexture = new RendererTask_CreateTexture3D(this, size, format, usage, mipLevels, initializeData, initDataSize);
	LE_DrawManager.AddRendererTask(rt_createTexture);
	mSize = LEMath::IntVector2(size.X(), size.Y());
	mDepth = size.Z();
}
void Texture::CreateScreenColor(const LEMath::IntSize &size)
{ 
    AutoPointer<RendererTask> rt_createScreenColor = new RendererTask_CreateScreenColor(this, size);
    LE_DrawManager.AddRendererTask(rt_createScreenColor);
    mSize = size;
}
void Texture::CreateColor(const LEMath::IntSize &size, const ByteColorRGBA &color)
{ 
    AutoPointer<RendererTask> rt_createColor = new RendererTask_CreateColor(this, size, color);
    LE_DrawManager.AddRendererTask(rt_createColor);
    mSize = size;
}
void Texture::CreateDepthStencil(const LEMath::IntSize &size, TEXTURE_DEPTH_FORMAT format)
{ 
    AutoPointer<RendererTask> rt_createDepth = new RendererTask_CreateDepthStencil(this, size, format);
    LE_DrawManager.AddRendererTask(rt_createDepth);
    mSize = size; 
}
void Texture::CreateRenderTarget(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage /*= 0*/)
{
    AutoPointer<RendererTask> rt_createRenderTarget = new RendererTask_CreateRenderTarget(this, size, format, usage);
    LE_DrawManager.AddRendererTask(rt_createRenderTarget);
    mSize = size;
}
void Texture::CreateUsingSourceData()
{
    if (!mSource) return;
    if (mSource->GetDepth() == 1)
        Create(mSource->GetSize(), mSource->GetFormat(), 0u, mSource->GetMipCount(), mSource->GetColorData(), mSource->GetColorDataSize());
    else if (mSource->GetDepth() > 1)
        Create3D(LEMath::IntSize3(mSource->GetSize().X(), mSource->GetSize().Y(), mSource->GetDepth()), mSource->GetFormat(), 0u, mSource->GetMipCount(), mSource->GetColorData(), mSource->GetColorDataSize());
}
void Texture::LoadFromMemory(const void *data, size_t size)
{
    AutoPointer<RendererTask> rt_loadTextureFromMemory = new RendererTask_LoadTextureFromMemory(this, data, size);
    LE_DrawManager.AddRendererTask(rt_loadTextureFromMemory);
}
void Texture::LoadFromMERLBRDFData(const void *data, size_t size)
{
	AutoPointer<RendererTask> rt_loaddTextureFromMERGLBRDFData = new RendererTask_LoadTextureFromMERLBRDFData(this, data, size);
	LE_DrawManager.AddRendererTask(rt_loaddTextureFromMERGLBRDFData);
}
bool Texture::Serialize(Archive &OutArchive)
{
    if (!mSource && OutArchive.IsSaving())
        return false;

    if (OutArchive.IsLoading()) {
        mSource = new SerializedTextureSource();
    }
    OutArchive << *mSource;
    return false;
}
void Texture::SetDebugName(const char *InDebugName)
{
    uint32 CopyLength = min(DebugNameLength - 1, strlen(InDebugName) + 1);
    ::memcpy(mDebugName, InDebugName, CopyLength);
    mDebugName[CopyLength - 1] = 0;
}
}