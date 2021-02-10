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

#include "PrivateDefinitions_DirectX12.h"

#include "Renderer/CommandBuffer.h"

namespace LimitEngine {
    class TextureImpl_DirectX12 : public TextureImpl
    {
    public:
        TextureImpl_DirectX12(Texture *InOwner)
            : TextureImpl(InOwner)
        {
            mRenderTargetView = {};
            mDepthStencilView = {};
            mShaderResourceView = {};
        }
        virtual ~TextureImpl_DirectX12()
        {}
        const LEMath::IntSize& GetSize() const override { return mSize; }
        const RendererFlag::BufferFormat& GetFormat() const override { return mFormat; }
        void* GetHandle() const override { return nullptr; }
        void* GetDepthSurfaceHandle() const override { return nullptr; }

        void Load(const char* filename) { LEASSERT(0); }
        void LoadFromMemory(const void* data, size_t size) override
        {
            UNIMPLEMENTED_ERROR
        }
        bool Create(const LEMath::IntSize &size, const RendererFlag::BufferFormat &format, uint32 usage, uint32 mipLevels, void *initData, size_t initDataSize) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
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
            resourceDesc.MipLevels = mipLevels;
            resourceDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Alignment = 0;

            TextureRendererAccessor(mOwner).SetResourceState(ResourceState::GenericRead);

            HRESULT hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                initData ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mResource));
            if (FAILED(hr)) {
                LEASSERT(0);
            }
            else if (initData) {
                ID3D12Device* device = static_cast<ID3D12Device*>(LE_DrawManagerRendererAccessor.GetDeviceHandle());
                ID3D12GraphicsCommandList* cmdlist = static_cast<ID3D12GraphicsCommandList*>(LE_DrawManagerRendererAccessor.GetImmediateCommandList());

                const uint32 SubResourceCount = mipLevels;
                SIZE_T buffersize = (sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * SubResourceCount;
                void* buffer = HeapAlloc(GetProcessHeap(), 0, buffersize);
                D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts = static_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(buffer);
                UINT64* rowsizesinbytes = reinterpret_cast<UINT64*>(layouts + SubResourceCount);
                UINT* rowcounts = reinterpret_cast<UINT*>(rowsizesinbytes + SubResourceCount);
                UINT64 requiredsize = 0u;
                device->GetCopyableFootprints(&resourceDesc, 0, SubResourceCount, 0, layouts, rowcounts, rowsizesinbytes, &requiredsize);

                ID3D12Resource* resource = static_cast<ID3D12Resource*>(LE_DrawManagerRendererAccessor.AllocateGPUBuffer(requiredsize));
                D3D12_RANGE range = { 0, 0 };
                void* mappeddata = nullptr;
                if (SUCCEEDED(resource->Map(0, &range, &mappeddata))) {
                    uint8* initdataptr = (uint8*)initData;
                    if (requiredsize != initDataSize) {
                        for (uint32 mipidx = 0; mipidx < mipLevels; mipidx++) {
                            size_t srcrowsize = size.Width() * RendererFlag::BufferFormatByteSize[static_cast<int>(mFormat)];
                            for (uint64 y = 0u; y < size.Height(); y++) {
                                ::memcpy((uint8*)mappeddata + layouts[mipidx].Offset + layouts[mipidx].Footprint.RowPitch * y, (uint8*)initdataptr, srcrowsize);
                                initdataptr += srcrowsize;
                            }
                        }
                    }
                    else {
                        ::memcpy(mappeddata, initData, initDataSize);
                    }
                    resource->Unmap(0, nullptr);
                }

                D3D12_TEXTURE_COPY_LOCATION dstlocation = { mResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };
                D3D12_TEXTURE_COPY_LOCATION srclocation = { resource, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, *layouts };
                cmdlist->CopyTextureRegion(&dstlocation, 0, 0, 0, &srclocation, nullptr);

                D3D12_RESOURCE_BARRIER barrier;
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = mResource;
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                cmdlist->ResourceBarrier(1, &barrier);

                LE_DrawManagerRendererAccessor.ExecuteImmediateCommandList(cmdlist, true);

                resource->Release();
            }

            mShaderResourceView.ptr = reinterpret_cast<SIZE_T>(LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
            D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
            SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            SRVDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            SRVDesc.Texture2D.MipLevels = 1;
            SRVDesc.Texture2D.MostDetailedMip = 0;
            device->CreateShaderResourceView(mResource, &SRVDesc, mShaderResourceView);

            return true;
        }
        bool Create3D(const LEMath::IntSize3& size, const RendererFlag::BufferFormat &format, uint32 usage, uint32 mipLevels, void* initData, size_t initDataSize) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
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
            resourceDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Alignment = 0;

            TextureRendererAccessor(mOwner).SetResourceState(ResourceState::GenericRead);

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
                //DrawCommand::ResourceBarrier(mOwner, ResourceState::CopyDest);
                //DrawCommand::CopyBuffer(mResource, 0, initData, 0, initDataSize);
                //DrawCommand::ResourceBarrier(mOwner, ResourceState::GenericRead);
            }

            mShaderResourceView.ptr = reinterpret_cast<SIZE_T>(LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
            D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
            SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            SRVDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            SRVDesc.Texture2D.MipLevels = 1;
            SRVDesc.Texture2D.MostDetailedMip = 0;
            device->CreateShaderResourceView(mResource, &SRVDesc, mShaderResourceView);

            return true;
        }
        void CreateScreenColor(const LEMath::IntSize& size) override
        {
            UNIMPLEMENTED_ERROR
        }
        void CreateColor(const LEMath::IntSize& size, const ByteColorRGBA& color) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
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
            resourceDesc.Format = ConvertBufferFormatToDXGIFormat(RendererFlag::BufferFormat::R8G8B8A8_SInt);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Alignment = 0;

            TextureRendererAccessor(mOwner).SetResourceState(ResourceState::Common);

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
                for (int32 index = 0; index < pitch * size.Height(); index++) {
                    colorBuffer[index] = color;
                }
                //DrawCommand::ResourceBarrier(mOwner, ResourceState::CopyDest);
                //DrawCommand::CopyBuffer(mResource, 0, colorBuffer, 0, bufferSize);
                //DrawCommand::ResourceBarrier(mOwner, ResourceState::GenericRead);
            }
        }
        void CreateDepthStencil(const LEMath::IntSize& size, const RendererFlag::BufferFormat &format) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
            if (device == nullptr) return;

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask = 0;
            heapDesc.NumDescriptors = 1;

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
            resourceDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

            TextureRendererAccessor(mOwner).SetResourceState(ResourceState::DepthWrite);

            HRESULT hr = device->CreateCommittedResource(
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
            DSVDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            DSVDesc.Texture2D.MipSlice = 0;

            mDepthStencilView.ptr = reinterpret_cast<SIZE_T>(LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)));

            device->CreateDepthStencilView(mResource, &DSVDesc, mDepthStencilView);
        }
        void CreateRenderTarget(const LEMath::IntSize& size, const RendererFlag::BufferFormat& format, uint32 usage) override
        {
            ID3D12Device* device = (ID3D12Device*)LE_DrawManagerRendererAccessor.GetDeviceHandle();
            if (device == nullptr) return;

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask = 0;
            heapDesc.NumDescriptors = 1;

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
            resourceDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

            TextureRendererAccessor(mOwner).SetResourceState(ResourceState::RenderTarget);

            HRESULT hr = device->CreateCommittedResource(
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
            RTVDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            RTVDesc.Texture2D.MipSlice = 0;
            RTVDesc.Texture2D.PlaneSlice = 0;

            mRenderTargetView.ptr = reinterpret_cast<SIZE_T>(LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));

            device->CreateRenderTargetView(mResource, &RTVDesc, mRenderTargetView);

            mShaderResourceView.ptr = reinterpret_cast<SIZE_T>(LE_DrawManagerRendererAccessor.AllocateDescriptor(static_cast<uint32>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
            D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
            SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            SRVDesc.Format = ConvertBufferFormatToDXGIFormat(format);
            SRVDesc.Texture2D.MipLevels = 1;
            SRVDesc.Texture2D.MostDetailedMip = 0;
            device->CreateShaderResourceView(mResource, &SRVDesc, mShaderResourceView);
        }
        void GenerateMipmap() override
        {
            UNIMPLEMENTED_ERROR
        }

        void* GetResource() const override { return (void*)mResource; }

        void* GetShaderResourceView() const override
        {
            return (void*)&mShaderResourceView;
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
        ID3D12Resource* mResource;

        LEMath::IntSize mSize = LEMath::IntSize::Zero;
        RendererFlag::BufferFormat mFormat = RendererFlag::BufferFormat::Unknown;

        D3D12_CPU_DESCRIPTOR_HANDLE mRenderTargetView;
        D3D12_CPU_DESCRIPTOR_HANDLE mDepthStencilView;
        D3D12_CPU_DESCRIPTOR_HANDLE mShaderResourceView;
    };
}