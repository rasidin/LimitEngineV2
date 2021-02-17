/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  TextParser.h
 @brief Text Parser (Data using text)
 @author minseob (https://github.com/rasidin)
 ***********************************************************/

#pragma once

#include <LEIntVector3.h>
#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

#include "Core/String.h"
#include "Core/ReferenceCountedObject.h"
#include "Containers/VectorArray.h"
#include "Renderer/ByteColorRGBA.h"

namespace LimitEngine {
    class TextParser : public ReferenceCountedObject<LimitEngineMemoryCategory::Common>
    {
    public:
        typedef struct _NODE
        {
            String                   name;
            _NODE                   *parent;
            VectorArray<String>      values;
            VectorArray<_NODE*>      children;
            
            // ------------------------------------------
            // Ctor & Dtor
            // ------------------------------------------
            _NODE()
            {
                name = "";
                parent = NULL;
                values.Clear();
                children.Clear();
            }
            ~_NODE()
            {
                values.Clear();
                for(uint32 i=0;i<children.count();i++)
                {
                    delete children[i];
                }
                children.Clear();
            }
            // ------------------------------------------
            // Operators
            // ------------------------------------------
            _NODE* operator[](const String &name)   { return FindChild(name); }
            // ------------------------------------------
            // Interface
            // ------------------------------------------
            _NODE* FindChild(const String &name)
            {
                for(uint32 n=0;n<children.count();n++)
                {
                    if (children[n]->name == name) return children[n];
                }
                return NULL;
            }
            bool IsValueNumber(int n) const
            {
                for(uint32 i=0;i<values[n].GetLength();i++)
                {
                    char v = values[n][i];
                    if (v != 0x2e && v != 0x2d && (v < 0x30 || v > 0x39))
                        return false;
                }
                return true;
            }
            uint32 ToInt() const
            {
                return values[0].ToInt();
            }
            LEMath::IntVector3 ToIntVector3() const
            {
                return LEMath::IntVector3(values[0].ToInt(),
                                values[1].ToInt(),
                                values[2].ToInt());
            }
            ByteColorRGBA ToByteColorRGBA() const
            {
                return ByteColorRGBA(static_cast<uint8>(values[0].ToInt()), 
                                     static_cast<uint8>(values[1].ToInt()),
                                     static_cast<uint8>(values[2].ToInt()),
                                     static_cast<uint8>(values[3].ToInt()));
            }
            LEMath::FloatVector2 ToFloatVector2() const
            {
                return LEMath::FloatVector2(values[0].ToFloat(),
                                            values[1].ToFloat());
            }
            LEMath::FloatVector3 ToFloatVector3() const
            {
                return LEMath::FloatVector3(values[0].ToFloat(),
                                            values[1].ToFloat(),
                                            values[2].ToFloat());
            }
            LEMath::FloatVector4 ToFloatVector4() const
            {
                return LEMath::FloatVector4(values[0].ToFloat(),
                                            values[1].ToFloat(),
                                            values[2].ToFloat(),
                                            values[3].ToFloat());
            }
            LEMath::FloatColorRGBA ToFloatColorRGBA() const
            {
                return LEMath::FloatColorRGBA(values[0].ToFloat(), 
                                              values[1].ToFloat(), 
                                              values[2].ToFloat(), 
                                              values[3].ToFloat());
            }
            LEMath::FloatMatrix4x4 ToFloatMatrix4x4() const
            {
                return LEMath::FloatMatrix4x4(
                    values[ 0].ToFloat(), values[ 1].ToFloat(), values[ 2].ToFloat(), values[ 3].ToFloat(),
                    values[ 4].ToFloat(), values[ 5].ToFloat(), values[ 6].ToFloat(), values[ 7].ToFloat(),
                    values[ 8].ToFloat(), values[ 9].ToFloat(), values[10].ToFloat(), values[11].ToFloat(),
                    values[12].ToFloat(), values[13].ToFloat(), values[14].ToFloat(), values[15].ToFloat()
                );
            }
            void Save(FILE *fp, int indent);
        } NODE;
    public:
        TextParser();
        TextParser(const char *text);
        virtual ~TextParser();
        
        bool Parse(const char *text);
        bool Save(const char *filename);

        NODE* GetNode(const char *name);
    private:
        void addNewNode(NODE *Parent, const char *NodeName, NODE **OutNode);
        void inputData(NODE *parent, NODE **node, char *buf, uint32 length, bool sequneceIn);
        
    private:
        VectorArray<NODE*>       mNodes;
    };
}
