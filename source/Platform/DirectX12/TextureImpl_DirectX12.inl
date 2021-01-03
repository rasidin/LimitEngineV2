/*******************************************************************
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
-------------------------------------------------------------------
@file  TextureImpl_DirectX12.inl
@brief Texture (DirectX12)
@author minseob (leeminseob@outlook.com)
*******************************************************************/
#include <d3d12.h>

namespace LimitEngine {
    DXGI_FORMAT ConvertTextureFormat(const TEXTURE_COLOR_FORMAT &format)
    {
        switch (format)
        {
        case TEXTURE_COLOR_FORMAT_R8G8B8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TEXTURE_COLOR_FORMAT_A8R8G8B8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TEXTURE_COLOR_FORMAT_R16F:
            return DXGI_FORMAT_R16_FLOAT;
        case TEXTURE_COLOR_FORMAT_G16R16F:
            return DXGI_FORMAT_R16G16_FLOAT;
        case TEXTURE_COLOR_FORMAT_A16B16G16R16F:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case TEXTURE_COLOR_FORMAT_R32F:
            return DXGI_FORMAT_R32_FLOAT;
        case TEXTURE_COLOR_FORMAT_G32R32F:
            return DXGI_FORMAT_R32G32_FLOAT;
        case TEXTURE_COLOR_FORMAT_A32B32G32R32F:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }
    DXGI_FORMAT ConvertDepthFormat(const TEXTURE_DEPTH_FORMAT& format)
    {
        switch (format)
        {
        case TEXTURE_DEPTH_FORMAT_D32F:
            return DXGI_FORMAT_D32_FLOAT;
        case TEXTURE_DEPTH_FORMAT_D24S8:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    class TextureImpl_DirectX12 : public TextureImpl
    {
    public:
        TextureImpl_DirectX12(Texture *InOwner)
            : TextureImpl(InOwner)
        {}
        virtual ~TextureImpl_DirectX12()
        {}
        LEMath::IntSize GetSize() const override { return mSize; }
        TEXTURE_COLOR_FORMAT GetFormat() const override { return mFormat; }
        void* GetHandle() const override { return nullptr; }
        void* GetDepthSurfaceHandle() const override { return nullptr; }

        void Load(const char* filename) { LEASSERT(0); }
        void LoadFromMemory(const void* data, size_t size) override
        {
            UNIMPLEMENTED_ERROR
        }
        bool Create(const LEMath::IntSize &size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void *initData, size_t initDataSize) override
        {
            ID3D12Device* device = (ID3D12Device*)sDrawManager->GetDeviceHandle();
            if (device == nullptr) return false;

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Width = size.Width();
            resourceDesc.Height = size.Height();
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = ConvertTextureFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Alignment = 0;

            mOwner->SetResourceState(ResourceState::GenericRead);

            HRESULT hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }
            else if (initData) {
                DrawCommand::ResourceBarrier(mOwner, ResourceState::CopyDest);
                DrawCommand::CopyBuffer(mResource, 0, initData, 0, initDataSize);
                DrawCommand::ResourceBarrier(mOwner, ResourceState::GenericRead);
            }
            return true;
        }
        bool Create3D(const LEMath::IntSize3& size, TEXTURE_COLOR_FORMAT format, uint32 usage, uint32 mipLevels, void* initData, size_t initDataSize) override
        {
            ID3D12Device* device = (ID3D12Device*)sDrawManager->GetDeviceHandle();
            if (device == nullptr) return false;

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            resourceDesc.Width = size.Width();
            resourceDesc.Height = size.Height();
            resourceDesc.DepthOrArraySize = size.Depth();
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = ConvertTextureFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Alignment = 0;

            mOwner->SetResourceState(ResourceState::GenericRead);

            HRESULT hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }
            else if (initData) {
                DrawCommand::ResourceBarrier(mOwner, ResourceState::CopyDest);
                DrawCommand::CopyBuffer(mResource, 0, initData, 0, initDataSize);
                DrawCommand::ResourceBarrier(mOwner, ResourceState::GenericRead);
            }
            return true;
        }
        void CreateScreenColor(const LEMath::IntSize& size) override
        {
            UNIMPLEMENTED_ERROR
        }
        void CreateColor(const LEMath::IntSize& size, const ByteColorRGBA& color) override
        {
            ID3D12Device* device = (ID3D12Device*)sDrawManager->GetDeviceHandle();
            if (device == nullptr) return;

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Width = size.Width();
            resourceDesc.Height = size.Height();
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = ConvertTextureFormat(TEXTURE_COLOR_FORMAT_A8R8G8B8);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Alignment = 0;

            mOwner->SetResourceState(ResourceState::Common);

            HRESULT hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }
            else {
                int pitch = GetSizeAlign(size.Width(), 256);
                size_t bufferSize = pitch * size.Height() * 4;
                ByteColorRGBA *colorBuffer = (ByteColorRGBA*)malloc(bufferSize);
                for (uint32 index = 0; index < pitch * size.Height(); index++) {
                    colorBuffer[index] = color;
                }
                DrawCommand::ResourceBarrier(mOwner, ResourceState::CopyDest);
                DrawCommand::CopyBuffer(mResource, 0, colorBuffer, 0, bufferSize);
                DrawCommand::ResourceBarrier(mOwner, ResourceState::GenericRead);
            }
        }
        void CreateDepthStencil(const LEMath::IntSize& size, TEXTURE_DEPTH_FORMAT format) override
        {
            ID3D12Device* device = (ID3D12Device*)sDrawManager->GetDeviceHandle();
            if (device == nullptr) return;

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask = 0;
            heapDesc.NumDescriptors = 1;

            HRESULT hr;
            hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mHeap));
            if (FAILED(hr)) {
                LEASSERT(0);
            }

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Width = size.Width();
            resourceDesc.Height = size.Height();
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = ConvertDepthFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

            mOwner->SetResourceState(ResourceState::DepthWrite);

            hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                nullptr,
                IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }


            D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
            DSVDesc.Format = ConvertDepthFormat(format);
            DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            DSVDesc.Texture2D.MipSlice = 0;

            mDepthStencilView = mHeap->GetCPUDescriptorHandleForHeapStart();

            device->CreateDepthStencilView(mResource, &DSVDesc, mDepthStencilView);
        }
        void CreateRenderTarget(const LEMath::IntSize& size, TEXTURE_COLOR_FORMAT format, uint32 usage) override
        {
            ID3D12Device* device = (ID3D12Device*)sDrawManager->GetDeviceHandle();
            if (device == nullptr) return;

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask = 0;
            heapDesc.NumDescriptors = 1;

            HRESULT hr;
            hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mHeap));
            if (FAILED(hr)) {
                LEASSERT(0);
            }

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Width = size.Width();
            resourceDesc.Height = size.Height();
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = ConvertTextureFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

            mOwner->SetResourceState(ResourceState::RenderTarget);

            hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                nullptr, 
                IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }

            D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
            RTVDesc.Format = ConvertTextureFormat(format);
            RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            RTVDesc.Texture2D.MipSlice = 0;
            RTVDesc.Texture2D.PlaneSlice = 0;

            mRenderTargetView = mHeap->GetCPUDescriptorHandleForHeapStart();

            device->CreateRenderTargetView(mResource, &RTVDesc, mRenderTargetView);
        }
        void GenerateMipmap() override
        {
            UNIMPLEMENTED_ERROR
        }

        void* GetResource() const override { return (void*)mResource; }

        void* GetShaderResourceView() const override
        {
            UNIMPLEMENTED_ERROR
            return nullptr;
        }
        void* GetUnorderedAccessView() const override
        {
            UNIMPLEMENTED_ERROR
            return nullptr;
        }
        void* GetRenderTargetView() const override
        {
            return (void*)&mRenderTargetView;
        }
        void* GetDepthStencilView() const override
        {
            return (void*)&mDepthStencilView;
        }

        void* Lock(const LEMath::IntRect& rect, int mipLevel) override
        {
            UNIMPLEMENTED_ERROR
            return nullptr;
        }
        void Unlock(int mipLevel) override
        {
            UNIMPLEMENTED_ERROR
        }

    private:
        ID3D12DescriptorHeap* mHeap;
        ID3D12Resource* mResource;

        LEMath::IntSize mSize = LEMath::IntSize::Zero;
        TEXTURE_COLOR_FORMAT mFormat = TEXTURE_COLOR_FORMAT_UNKNOWN;

        D3D12_CPU_DESCRIPTOR_HANDLE mRenderTargetView;
        D3D12_CPU_DESCRIPTOR_HANDLE mDepthStencilView;
    };
}