/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Vertex.h
 @brief Vertex Unit
 @author minseob (https://github.com/rasidin)
 ***********************************************************/
#pragma once

#include <LERenderer>

#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>

#include "Core/Common.h"
#include "Core/Object.h"
#include "Renderer/ByteColorRGBA.h"

#define FVF_P		(FVF_TYPE_POSITION)
#define FVF_PN		(FVF_TYPE_POSITION|FVF_TYPE_NORMAL)
#define FVF_PC		(FVF_TYPE_POSITION|FVF_TYPE_COLOR)
#define FVF_PT      (FVF_TYPE_POSITION|FVF_TYPE_TEXCOORD)
#define FVF_PCT		(FVF_TYPE_POSITION|FVF_TYPE_COLOR|FVF_TYPE_TEXCOORD)
#define FVF_PCTT	(FVF_TYPE_POSITION|FVF_TYPE_COLOR|FVF_TYPE_TEXCOORD|FVF_TYPE_TANGENT)
#define FVF_PNCT	(FVF_TYPE_POSITION|FVF_TYPE_NORMAL|FVF_TYPE_COLOR|FVF_TYPE_TEXCOORD)
#define FVF_PNCTTB  (FVF_TYPE_POSITION|FVF_TYPE_NORMAL|FVF_TYPE_COLOR|FVF_TYPE_TEXCOORD|FVF_TYPE_TANGENT|FVF_TYPE_BINORMAL)

#define SIZE_P		(FVF_SIZE_POSITION)
#define SIZE_PN		(FVF_SIZE_POSITION+FVF_SIZE_NORMAL)
#define SIZE_PT     (FVF_SIZE_POSITION*FVF_SIZE_TEXCOORD)
#define SIZE_PC		(FVF_SIZE_POSITION+FVF_SIZE_COLOR)
#define SIZE_PCT	(FVF_SIZE_POSITION+FVF_SIZE_COLOR+FVF_SIZE_TEXCOORD)
#define SIZE_PCTT	(FVF_SIZE_POSITION+FVF_SIZE_COLOR+FVF_SIZE_TEXCOORD+FVF_SIZE_TANGENT)
#define SIZE_PNCT	(FVF_SIZE_POSITION+FVF_SIZE_NORMAL+FVF_SIZE_COLOR+FVF_SIZE_TEXCOORD)
#define SIZE_PNCTTB (FVF_SIZE_POSITION+FVF_SIZE_NORMAL+FVF_SIZE_COLOR+FVF_SIZE_TEXCOORD+FVF_SIZE_TANGENT+FVF_SIZE_BINORMAL)

#define VERTEXTYPE(s)		FVF_##s, SIZE_##s

namespace LimitEngine {
enum FVF_TYPE
{
	FVF_TYPE_NONE		= 0,
	FVF_TYPE_POSITION	= 1,
	FVF_TYPE_NORMAL		= 1 << 1,
	FVF_TYPE_COLOR		= 1 << 2,
	FVF_TYPE_TEXCOORD	= 1 << 3,
	FVF_TYPE_WEIGHT		= 1 << 4,
    FVF_TYPE_TANGENT    = 1 << 5,
    FVF_TYPE_BINORMAL   = 1 << 6,
};
enum FVF_INDEX
{
    FVF_INDEX_NONE      = 0,
    FVF_INDEX_POSITION,
    FVF_INDEX_NORMAL,
    FVF_INDEX_COLOR,
    FVF_INDEX_TEXCOORD,
    FVF_INDEX_WEIGHT,
    FVF_INDEX_TANGENT,
    FVF_INDEX_BINORMAL,
    FVF_INDEX_MAX,
};
extern const RendererFlag::BufferFormat FVF_FORMATS[FVF_INDEX_MAX];
extern const char* FVF_NAMES[FVF_INDEX_MAX];
enum FVF_SIZE
{
	FVF_SIZE_NONE		= 0,
	FVF_SIZE_POSITION	= 12,
	FVF_SIZE_NORMAL		= 12,
	FVF_SIZE_COLOR		= 4,
	FVF_SIZE_TEXCOORD	= 8,
    FVF_SIZE_WEIGHT     = 4,
    FVF_SIZE_TANGENT    = 12,
    FVF_SIZE_BINORMAL   = 12,
};
template <uint32 tFVF, size_t tSize> class Vertex
{	// P | N |C | T | W...
public:
	size_t GetSize()		{ return tSize; }
	uint32 GetFVF()			{ return tFVF; }
		
    uint8* GetPtr(FVF_TYPE t)
    {
        uint8 *ptr = (uint8 *)&mBuffer[0];
        if (t == FVF_TYPE_POSITION) return ptr;
        if (tFVF & FVF_TYPE_POSITION) ptr += FVF_SIZE_POSITION;
        if (t == FVF_TYPE_NORMAL) return ptr;
        if (tFVF & FVF_TYPE_NORMAL) ptr += FVF_SIZE_NORMAL;
        if (t == FVF_TYPE_COLOR) return ptr;
        if (tFVF & FVF_TYPE_COLOR) ptr += FVF_SIZE_COLOR;
        if (t == FVF_TYPE_TEXCOORD) return ptr;
        if (tFVF & FVF_TYPE_TEXCOORD) ptr += FVF_SIZE_TEXCOORD;
        if (t == FVF_TYPE_WEIGHT) return ptr;
        if (tFVF & FVF_TYPE_WEIGHT) ptr += FVF_SIZE_WEIGHT;
        if (t == FVF_TYPE_TANGENT) return ptr;
        if (tFVF & FVF_TYPE_TANGENT) ptr += FVF_SIZE_TANGENT;
        if (t == FVF_TYPE_BINORMAL) return ptr;
        if (tFVF & FVF_TYPE_BINORMAL) ptr += FVF_SIZE_BINORMAL;
        return NULL;
    }
        
    LEMath::FloatVector3    GetPosition()   { return *(LEMath::FloatVector3 *)&mBuffer[0]; }
    LEMath::FloatVector3    GetNormal()     { return *(LEMath::FloatVector3 *)GetPtr(FVF_TYPE_NORMAL); }
    LEMath::FloatColorRGBA  GetColor()      { return *(LEMath::FloatColorRGBA *)GetPtr(FVF_TYPE_COLOR); }
    LEMath::FloatVector2    GetTexcoord()   { return *(LEMath::FloatVector2 *)GetPtr(FVF_TYPE_TEXCOORD); }
    LEMath::FloatVector3    GetTangent()    { return *(LEMath::FloatVector3 *)GetPtr(FVF_TYPE_TANGENT); }
    LEMath::FloatVector3    GetBinormal()   { return *(LEMath::FloatVector3 *)GetPtr(FVF_TYPE_BINORMAL); }
        
	void SetPosition(const LEMath::FloatVector3 &pos)
	{ *((LEMath::FloatVector3 *)&mBuffer[0]) = pos; }
	void SetNormal(const LEMath::FloatVector3 &nor)
	{ *((LEMath::FloatVector3 *)GetPtr(FVF_TYPE_NORMAL)) = nor; }
	void SetColor(const ByteColorRGBA &col)
	{ *((ByteColorRGBA *)GetPtr(FVF_TYPE_COLOR)) = col; }
	void SetTexcoord(const LEMath::FloatVector2 &uv)
	{ *((LEMath::FloatVector2 *)GetPtr(FVF_TYPE_TEXCOORD)) = uv; }
    void SetTangent(const LEMath::FloatVector3 &tan)
    { *((LEMath::FloatVector3 *)GetPtr(FVF_TYPE_TANGENT)) = tan; }
    void SetBinormal(const LEMath::FloatVector3 &bin)
    { *((LEMath::FloatVector3 *)GetPtr(FVF_TYPE_BINORMAL)) = bin; }
private:
	char mBuffer[tSize];
}; // Vertex
} // LimitEngine
