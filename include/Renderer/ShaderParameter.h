/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  LE_Material.h
@brief Shader parameter(float, vector, matrix, texture)
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

#include "Core/Object.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
class ShaderParameter : Object<LimitEngineMemoryCategory_Graphics>
{
public:
    enum Type 
    {
        Type_None = 0,
        
        Type_Integer,
        Type_Float,
        Type_Float2,
        Type_Float3,
        Type_Float4,
        Type_Matrix4x4,
        Type_Texture,

        Type_Count,
    };
public:
    ShaderParameter()
        : m_Type(Type_None)
        , m_Data(NULL)
    {
        for (uint32 rpIndex = 0, rpCount = (uint32)RenderPass::NumOfRenderPass; rpIndex < rpCount; rpIndex++)
            m_IndexOfShaderParameter[rpIndex] = -1;
    }
    virtual ~ShaderParameter()
    {
    }
    inline void Release()
    {
		if (m_Data && m_Type != Type_Texture) {
			free(m_Data);
			m_Data = NULL;
		}
        m_Type = Type_None;
    }
    void SetType(Type t)
    {
        Release();
        m_Type = t;
		if (t != Type_Texture) {
			m_Data = malloc(GetDataSize());
		}
    }
    Type GetType() const { return m_Type; }
    int IndexOfShaderParameter(uint32 InRenderPass) const { return m_IndexOfShaderParameter[InRenderPass]; }
    void SetIndexOfShaderParameter(uint32 InRenderPass, uint32 m) { m_IndexOfShaderParameter[InRenderPass] = m; }
    ShaderParameter& operator=(int v)
    {
        SetType(Type_Integer);
        *static_cast<int*>(m_Data) = v;
        return *this;
    }
    ShaderParameter& operator=(float v)
    {
        SetType(Type_Float);
        *static_cast<float*>(m_Data) = v;
        return *this;
    }
    ShaderParameter& operator=(const LEMath::FloatVector2 &v)
    {
        SetType(Type_Float2);
        *static_cast<LEMath::FloatVector2*>(m_Data) = v;
        return *this;
    }
    ShaderParameter& operator=(const LEMath::FloatVector3 &v)
    {
        SetType(Type_Float3);
        *static_cast<LEMath::FloatVector3*>(m_Data) = v;
        return *this;
    }
    ShaderParameter& operator=(const LEMath::FloatVector4 &v)
    {
        SetType(Type_Float4);
        *static_cast<LEMath::FloatVector4*>(m_Data) = v;
        return *this;
    }
    ShaderParameter& operator=(const LEMath::FloatMatrix4x4 &v)
    {
        SetType(Type_Matrix4x4);
        *static_cast<LEMath::FloatMatrix4x4*>(m_Data) = v;
        return *this;
    }
    ShaderParameter& operator=(Texture *v)
    {
        SetType(Type_Texture);
        m_Data = v;
        return *this;
    }
    size_t GetDataSize() const
    {
        switch (m_Type)
        {
        case Type_Integer: return sizeof(int);
        case Type_Float: return sizeof(float);
        case Type_Float2: return sizeof(LEMath::FloatVector2);
        case Type_Float3: return sizeof(LEMath::FloatVector3);
        case Type_Float4: return sizeof(LEMath::FloatVector4);
        case Type_Matrix4x4: return sizeof(LEMath::FloatMatrix4x4);
        case Type_Texture: return sizeof(Texture*);
        default: return 0u;
        }
    }
    operator int() {
        switch (m_Type) {
            case Type_Integer: return *static_cast<int*>(m_Data);
            case Type_Float:
            case Type_Float2:
            case Type_Float3:
            case Type_Float4: 
            case Type_Matrix4x4: return static_cast<int>(*static_cast<float*>(m_Data));
            default: return 0;
        }
    }
    operator float() {
        switch (m_Type) {
            case Type_Integer: return static_cast<float>(*static_cast<int*>(m_Data));
            case Type_Float:
            case Type_Float2:
            case Type_Float3:
            case Type_Float4:
            case Type_Matrix4x4: return *static_cast<float*>(m_Data);
            default: return 0.0f;
        }
    }
    operator LEMath::FloatVector2() {
        switch (m_Type) {
        case Type_Integer: { float value = static_cast<float>(*static_cast<int*>(m_Data)); return LEMath::FloatVector3(value, value, value); }
        case Type_Float: { float value = *static_cast<float*>(m_Data); return LEMath::FloatVector3(value, value, value); }
        case Type_Float2: return *static_cast<LEMath::FloatVector2*>(m_Data);
        case Type_Float3:
        case Type_Float4:
        case Type_Matrix4x4: return *static_cast<LEMath::FloatVector2*>(m_Data);
        default: return LEMath::FloatVector2(0.0f);
        }
    }
    operator LEMath::FloatVector3() {
        switch (m_Type) {
            case Type_Integer: { float value = static_cast<float>(*static_cast<int*>(m_Data)); return LEMath::FloatVector3(value, value, value); }
            case Type_Float: { float value = *static_cast<float*>(m_Data); return LEMath::FloatVector3(value, value, value); }
            case Type_Float2: { LEMath::FloatVector2 value = *static_cast<LEMath::FloatVector2*>(m_Data); return LEMath::FloatVector3(value.X(), value.Y(), 0.0f); }
            case Type_Float3:
            case Type_Float4:
            case Type_Matrix4x4: return *static_cast<LEMath::FloatVector3*>(m_Data);
            default: return LEMath::FloatVector3(0.0f);
        }
    }
    operator LEMath::FloatVector4() {
        switch (m_Type) {
            case Type_Integer: { float value = static_cast<float>(*static_cast<int*>(m_Data)); return LEMath::FloatVector4(value, value, value, value); }
            case Type_Float: { float value = *static_cast<float*>(m_Data); return LEMath::FloatVector4(value, value, value, value); }
            case Type_Float2: { LEMath::FloatVector2 value = *static_cast<LEMath::FloatVector2*>(m_Data); return LEMath::FloatVector4(value.X(), value.Y(), 0.0f, 0.0f); }
            case Type_Float3: { LEMath::FloatVector3 value = *static_cast<LEMath::FloatVector3*>(m_Data); return LEMath::FloatVector4(value.X(), value.Y(), value.Z(), 0.0f); }
            case Type_Float4: return *static_cast<LEMath::FloatVector4*>(m_Data);
            case Type_Matrix4x4: return *static_cast<LEMath::FloatVector4*>(m_Data);
            default: return LEMath::FloatVector4(0.0f);
        }
    }
    operator LEMath::FloatMatrix4x4() {
        switch (m_Type) {
            case Type_Integer: { float v = static_cast<float>(*static_cast<int*>(m_Data)); return LEMath::FloatMatrix4x4(v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v); }
            case Type_Float: { float v = *static_cast<float*>(m_Data); return LEMath::FloatMatrix4x4(v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v); }
            case Type_Matrix4x4: return *static_cast<LEMath::FloatMatrix4x4*>(m_Data);
            default: return LEMath::FloatMatrix4x4::Identity;
        }
    }
    operator Texture*() {
        return (m_Type == Type_Texture) ? (Texture*)m_Data : NULL;
    }
private:
    Type    m_Type;
    int     m_IndexOfShaderParameter[(uint32)RenderPass::NumOfRenderPass];
    void   *m_Data;
};
}
