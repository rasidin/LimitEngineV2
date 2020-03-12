/***********************************************************
LIMITEngine Header File
Copyright(C), LIMITGAME, 2020
-----------------------------------------------------------
@file  PrivateDefinitions_DirectX.h
@brief Private definitions for DirectX
@author minseob(leeminseob@outlook.com)
***********************************************************/
#pragma once
#include <d3d11.h>

#include "Core/Object.h"

namespace LimitEngine {
struct CommandInit_Parameter : public Object<LimitEngineMemoryCategory_Graphics>
{
    ID3D11Device			*mD3DDevice = nullptr;
    ID3D11DeviceContext		*mD3DDeviceContext = nullptr;
    ID3D11RenderTargetView	*mBaseRenderTargetView = nullptr;
    ID3D11DepthStencilView	*mBaseDepthStencilView = nullptr;
};
}