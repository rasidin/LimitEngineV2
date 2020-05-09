/***********************************************************
LIMITEngine Header File
Copyright (C), LIMITGAME, 2020
-----------------------------------------------------------
@file  CommandBuffer.h
@brief CommandBuffer for rendering
@author minseob (leeminseob@outlook.com)
***********************************************************/
#pragma once

#include <LERenderer>
#include <LEManagers>

#include <LEFloatVector2.h>
#include <LEFloatVector4.h>
#include <LEIntVector4.h>

#include "Core/Singleton.h"
#include "Core/Mutex.h"

namespace LimitEngine {
class CommandImpl : public Object<LimitEngineMemoryCategory_Graphics>
{
public:
    CommandImpl() {}
    virtual ~CommandImpl() {}

    virtual void Init(void *Parameter) = 0;
    virtual void Term() = 0;

    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual bool PrepareForDrawing() = 0;
    virtual bool PrepareForDrawingModel() = 0;
    virtual void ClearScreen(const LEMath::FloatColorRGBA &Color) = 0;
    virtual void BindVertexBuffer(void *Handle, void *Buffer, uint32 Offset, uint32 Size, uint32 Stride) = 0;
    virtual void BindIndexBuffer(void *Handle) = 0;
    virtual void BindShader(Shader *Shd) = 0;
    virtual void BindTargetTexture(uint32 Index, Texture *Tex) = 0;
    virtual void BindTexture(uint32 Index, Texture *Tex) = 0;
    virtual void Dispatch(int X, int Y, int Z) = 0;
    virtual void DrawPrimitive(uint32 Primitive, uint32 Offset, uint32 Count) = 0;
    virtual void DrawPrimitiveUp(uint32 Primitive, uint32 Count, void *Data, uint32 Stride) = 0;
    virtual void DrawIndexedPrimitive(uint32 Primitive, uint32 VertexCount, uint32 Count) = 0;
    virtual void SetFVF(uint32 FVF) = 0;
    virtual void SetCulling(uint32 Culling) = 0;
    virtual void SetBlendFunc(uint32 rt, RendererFlag::BlendFlags Func) = 0;
    virtual void SetDepthFunc(uint32 Func) = 0;
    virtual void SetEnabled(uint32 Flag) = 0;
    virtual void SetDisable(uint32 Flag) = 0;
    virtual void SetRenderTarget(uint32 Index, Texture *Color, Texture *Depth, uint32 SurfaceIndex) = 0;
    //virtual void SetUniformFloat1(int Location, float Value) = 0;
    //virtual void SetUniformFloat2(int Location, const LEMath::FloatVector2 &Value) = 0;
    //virtual void SetUniformFloat4(int Location, const LEMath::FloatVector4 &Value) = 0;
    //virtual void SetUniformMatrix4(int Location, const LEMath::FloatMatrix4x4 &Value) = 0;
};
class CommandBuffer : public Object<LimitEngineMemoryCategory_Graphics>
{
    friend DrawManager;
    friend DrawCommand;
private:            // Private Definitions
    static const uint32 CommandReservedMemorySize = 1 * (1 << 20); // 10MB

private:            // Private Structure
                    // Render command list
    typedef struct _COMMAND_COMMON
    {
        enum Commands
        {
            cBeginScene = 0,
            cEndScene,
            cPresent,
            cBeginDrawing,
            cEndDrawing,
            cClearScreen,
            cBindShader,
            cBindTexture,
			cBindTargetTexture,
            cBindVertexBuffer,
            cBindIndexBuffer,
            cDispatch,
			cDrawPrimitive,
            cDrawPrimitiveUp,
            cDrawIndexedPrimitive,
            cSetShaderUniformFloat1,
            cSetShaderUniformFloat2,
            cSetShaderUniformFloat4,
            cSetShaderUniformInt1,
            cSetShaderUniformInt4,
            cSetShaderUniformMatrix4,
            cSetFVF,
            cSetCulling,
            cSetEnable,
            cSetDisable,
            cSetDepthFunc,
            cSetBlendFunc,
            cSetRenderTarget,
            CommandNum,
        };
        uint16       commandType;               //!<Type of command                  [ 2 ]
        uint16       nextOffset;                // Offset to next command            [ 2 ]

        _COMMAND_COMMON() : nextOffset(1) {}
        _COMMAND_COMMON(uint16 t) : commandType(t), nextOffset(1) {}
        _COMMAND_COMMON(uint16 t, uint16 o) : commandType(t), nextOffset(o) {}

        virtual size_t GetSize() const { return 64u; } // Get Command Size
    } COMMAND_COMMON;    // [ 4 ]

    // Empty command
    typedef struct _COMMAND : public _COMMAND_COMMON
    {
        char                data[48];        //!<Data of command                    [ 60 ]
    } COMMAND;    // [ 64 | 64 ]
    // Begin scene
    typedef struct _COMMAND_BEGINSCENE : public _COMMAND_COMMON
    {
        _COMMAND_BEGINSCENE() : _COMMAND_COMMON(cBeginScene) {}
    } COMMAND_BEGINSCENE;
    // End scene
    typedef struct _COMMAND_ENDSCENE : public _COMMAND_COMMON
    {
        _COMMAND_ENDSCENE() : _COMMAND_COMMON(cEndScene) {}
    } COMMAND_ENDSCENE;
    // Present screen   
    typedef struct _COMMAND_PRESENT : public _COMMAND_COMMON
    {
        _COMMAND_PRESENT() : _COMMAND_COMMON(cPresent) {}
    } COMMAND_PRESENT;
    // Begin drawing
    typedef struct _COMMAND_BEGINDRAWING : public _COMMAND_COMMON
    {
        _COMMAND_BEGINDRAWING() : _COMMAND_COMMON(cBeginDrawing) {}
    } COMMAND_BEGINDRAWING;
    // End drawing
    typedef struct _COMMAND_ENDDRAWING : public _COMMAND_COMMON
    {
        _COMMAND_ENDDRAWING() : _COMMAND_COMMON(cEndDrawing) {}
    } COMMAND_ENDDRAWING;
    // Clear screen
    typedef struct _COMMAND_CLEARSCREEN : public _COMMAND_COMMON
    {
        _COMMAND_CLEARSCREEN(const LEMath::FloatColorRGBA &c)
            : _COMMAND_COMMON(cClearScreen)
            , color(c)
        {
        }
        LEMath::FloatColorRGBA       color;
    } COMMAND_CLEARSCREEN;
    // Bind vertex buffer before drawing model
    typedef struct _COMMAND_BINDVERTEXBUFFER : public _COMMAND_COMMON
    {
        _COMMAND_BINDVERTEXBUFFER(void *hd, void *bf, uint32 of, uint32 sz, uint32 st)
            : _COMMAND_COMMON(cBindVertexBuffer)
            , handle(hd)
            , buffer(bf)
            , offset(of)
            , size(sz)
            , stride(st)
        {
        }
        void *handle;
        void *buffer;
        uint32 offset;
        uint32 size;
        uint32 stride;
    } COMMAND_BINDVERTEXBUFFER;
    // Bind index buffer before drawing model
    typedef struct _COMMAND_BINDINDEXBUFFER : public _COMMAND_COMMON
    {
        _COMMAND_BINDINDEXBUFFER(void *hd)
            : _COMMAND_COMMON(cBindIndexBuffer)
            , handle(hd)
        {
        }
        void *handle;
    } COMMAND_BINDINDEXBUFFER;
	// Bind target(RW) texture
	typedef struct _COMMAND_BINDTARGETTEXTURE : public _COMMAND_COMMON
	{
		_COMMAND_BINDTARGETTEXTURE(uint32 i, Texture *t)
			: _COMMAND_COMMON(cBindTargetTexture)
			, index(i)
			, texture(t)
		{}
		uint32          index;
		Texture        *texture;
	} COMMAND_BINDTARGETTEXTURE;
    // Bind texture before drawing model
    typedef struct _COMMAND_BINDTEXTURE : public _COMMAND_COMMON
    {
        _COMMAND_BINDTEXTURE(uint32 i, Texture *t)
            : _COMMAND_COMMON(cBindTexture)
            , index(i)
            , texture(t)
        {
        }
        uint32           index;
        Texture         *texture;
    } COMMAND_BINDTEXTURE;
    // Set float value to shader
    typedef struct _COMMAND_SETSHADERUNIFORMFLOAT1 : public _COMMAND_COMMON
    {
        _COMMAND_SETSHADERUNIFORMFLOAT1(Shader *sh, int l, const float v)
            : _COMMAND_COMMON(cSetShaderUniformFloat1)
            , shader(sh)
            , location(l)
            , value(v)
        {
        };
        Shader          *shader;
        int              location;
        float            value;
    } COMMAND_SETSHADERUNIFORMFLOAT1;
    // Set float2 value to shader
    typedef struct _COMMAND_SETSHADERUNIFORMFLOAT2 : public _COMMAND_COMMON
    {
        _COMMAND_SETSHADERUNIFORMFLOAT2(Shader *sh, int l, const LEMath::FloatVector2 &v)
            : _COMMAND_COMMON(cSetShaderUniformFloat2)
            , shader(sh)
            , location(l)
            , value(v)
        {
        };
        Shader          *shader;
        int              location;
        LEMath::FloatVector2         value;
    } COMMAND_SETSHADERUNIFORMFLOAT2;
    // Set float4 value to shader
    typedef struct _COMMAND_SETSHADERUNIFORMFLOAT4 : public _COMMAND_COMMON
    {
        _COMMAND_SETSHADERUNIFORMFLOAT4(Shader *sh, int l, const LEMath::FloatVector4 &v)
            : _COMMAND_COMMON(cSetShaderUniformFloat4)
            , shader(sh)
            , location(l)
            , value(v)
        {
        }
        Shader                 *shader;
        int                     location;
        LEMath::FloatVector4    value;
    } COMMAND_SETSHADERUNIFORMFLOAT4;
    typedef struct _COMMAND_SETSHADERUNIFORMINT1 : public _COMMAND_COMMON
    {
        _COMMAND_SETSHADERUNIFORMINT1(Shader *sh, int l, const int32 v)
            : _COMMAND_COMMON(cSetShaderUniformInt1)
            , shader(sh)
            , location(l)
            , value(v)
        {
        };
        Shader                  *shader;
        int                      location;
        int32                    value;
    } COMMAND_SETSHADERUNIFORMINT1;
    typedef struct _COMMAND_SETSHADERUNIFORMINT4 : public _COMMAND_COMMON
    {
        _COMMAND_SETSHADERUNIFORMINT4(Shader *sh, int l, const LEMath::IntVector4 &v)
            : _COMMAND_COMMON(cSetShaderUniformInt4)
            , shader(sh)
            , location(l)
            , value(v)
        {
        };
        Shader                  *shader;
        int                      location;
        LEMath::IntVector4       value;
    } COMMAND_SETSHADERUNIFORMINT4;
    // Set matrix4 value to shader
    typedef struct _COMMAND_SETSHADERUNIFORMMATRIX4 : public _COMMAND_COMMON
    {
        _COMMAND_SETSHADERUNIFORMMATRIX4(Shader *sh, int l, int s, float *p)
            : _COMMAND_COMMON(cSetShaderUniformMatrix4)
            , shader(sh)
            , location(l)
            , size(s)
            , pointer(p)
        {
        }
        Shader         *shader;
        int             location;
        int             size;
        float          *pointer;
    } COMMAND_SETSHADERUNIFORMMATRIX4;
	// Dispatch
	typedef struct _COMMAND_DISPATCH : public _COMMAND_COMMON
	{
		_COMMAND_DISPATCH(int x, int y, int z)
			: _COMMAND_COMMON(cDispatch)
			, gridx(x)
			, gridy(y)
			, gridz(z)
		{}
		int gridx;
		int gridy;
		int gridz;
	} COMMAND_DISPATCH;
    // Draw primitive
    typedef struct _COMMAND_DRAWPRIMITIVE : public _COMMAND_COMMON
    {
        _COMMAND_DRAWPRIMITIVE(RendererFlag::PrimitiveTypes t, uint32 o, uint32 c)
            : _COMMAND_COMMON(cDrawPrimitive)
            , primitive(static_cast<uint32>(t))
            , offset(o)
            , count(c)
        {
        }
        uint32              primitive;
        uint32              offset;
        uint32              count;
    } COMMAND_DRAWPRIMITIVE;
    // Draw primitive up
    typedef struct _COMMAND_DRAWPRIMITIVEUP : public _COMMAND_COMMON
    {
        _COMMAND_DRAWPRIMITIVEUP(RendererFlag::PrimitiveTypes t, uint32 c, void *d, uint32 s)
            : _COMMAND_COMMON(cDrawPrimitiveUp)
            , primitive(static_cast<uint32>(t))
            , count(c)
            , data(d)
            , stride(s)
        {
        }
        uint32                 primitive;
        uint32                 count;
        void                  *data;
        uint32                 stride;
    } COMMAND_DRAWPRIMITIVEUP;
    // Draw indexed primitive
    typedef struct _COMMAND_DRAWINDEXEDPRIMITIVE : public _COMMAND_COMMON
    {
        _COMMAND_DRAWINDEXEDPRIMITIVE(RendererFlag::PrimitiveTypes t, uint32 v, uint32 c)
            : _COMMAND_COMMON(cDrawIndexedPrimitive)
            , primitive(static_cast<uint32>(t))
            , vtxcount(v)
            , count(c)
        {
        }
        uint32                 primitive;
        uint32               vtxcount;
        uint32                 count;
    } COMMAND_DRAWINDEXEDPRIMITIVE;
    // Set FVF    
    typedef struct _COMMAND_SETFVF : public _COMMAND_COMMON
    {
        _COMMAND_SETFVF(uint32 f)
            : _COMMAND_COMMON(cSetFVF)
            , fvf(f)
        {
        }
        uint32                fvf;
    } COMMAND_SETFVF;
    // Set culling
    typedef struct _COMMAND_SETCULLING : public _COMMAND_COMMON
    {
        _COMMAND_SETCULLING(uint32 cull)
            : _COMMAND_COMMON(cSetCulling)
            , culling(cull)
        {
        }
        uint32                culling;
    } COMMAND_SETCULLING;
    // Set enable render options
    typedef struct _COMMAND_SETENABLE : public _COMMAND_COMMON
    {
        _COMMAND_SETENABLE(uint32 f)
            : _COMMAND_COMMON(cSetEnable)
            , flags(f)
        {
        }
        uint32              flags;
    } COMMAND_SETENABLE;
    // Set disable render options
    typedef struct _COMMAND_SETDISABLE : public _COMMAND_COMMON
    {
        _COMMAND_SETDISABLE(uint32 f)
            : _COMMAND_COMMON(cSetDisable)
            , flags(f)
        {
        }
        uint32              flags;
    } COMMAND_SETDISABLE;
    // Set blend function
    typedef struct _COMMAND_SETBLENDFUNC : public _COMMAND_COMMON
    {
        _COMMAND_SETBLENDFUNC(uint32 rtnum, RendererFlag::BlendFlags f)
            : _COMMAND_COMMON(cSetBlendFunc)
            , rt(rtnum)
            , func(static_cast<uint32>(f))
        {
        }
        uint32              rt;
        uint32              func;
    } COMMAND_SETBLENDFUNC;
    // Set depth function
    typedef struct _COMMAND_SETDEPTHFUNC : public _COMMAND_COMMON
    {
        _COMMAND_SETDEPTHFUNC(RendererFlag::TestFlags f)
            : _COMMAND_COMMON(cSetDepthFunc)
            , flags(static_cast<uint32>(f))
        {
        }
        uint32              flags;
    } COMMAND_SETDEPTHFUNC;
    // Bind shader before drawing model
    typedef struct _COMMAND_BINDSHADER : public _COMMAND_COMMON
    {
        _COMMAND_BINDSHADER(Shader *sh)
            : _COMMAND_COMMON(cBindShader)
            , shader(sh)
        {
        }
        Shader                *shader;
    } COMMAND_BINDSHADER;
    // Set render target for drawing
    typedef struct _COMMAND_SETRENDERTARGET : public _COMMAND_COMMON
    {
        _COMMAND_SETRENDERTARGET(uint32 idx, Texture *c, Texture *d, uint32 suridx)
            : _COMMAND_COMMON(cSetRenderTarget)
            , index(idx)
            , surfaceIndex(suridx)
            , color(c)
            , depthstencil(d)
        {
        }
        uint32               index;
        uint32               surfaceIndex;
        Texture             *color;
        Texture             *depthstencil;
    } COMMAND_SETRENDERTARGET;
public:
    // Ctor & Dtor
    CommandBuffer(size_t bufferSize = CommandReservedMemorySize);
    virtual ~CommandBuffer();

    void Init(void *Parameter) { if (mImpl) mImpl->Init(Parameter); }
    void Term() { if (mImpl) mImpl->Term(); }

    uint32 CalculateCommandOffset(COMMAND *cmd);

    void SetCommand(COMMAND_COMMON *cmd);
    void AddCommand(COMMAND_COMMON *cmd);

    void LockWriteCommandBuffer() { mMutex.Lock(); }
    void Flush(RenderState *rs);
private:
    void* allocateFromCommandBuffer(size_t size);
    float* copyMatrixToBuffer(float *ptr);

    COMMAND* popPushCommandBuffer();
    COMMAND* nextPushCommandBuffer(int count = 1);
    COMMAND* nextPullCommandBuffer();
private:
    CommandImpl             *mImpl;

    void                    *mCommandBuffer;
    void                    *mPushCommandBufferPointer;
    void                    *mPullCommandBufferPointer;

    Mutex                    mMutex;
};
}
