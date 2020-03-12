/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  MetaData.h
 @brief Meta data of class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once
#ifndef _METADATA_H_
#define _METADATA_H_

#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>

#include "Core/String.h"
#include "Containers/VectorArray.h"

#define METADATA_POINTER(ParamName) GetPointerOffset(this, &this->##ParamName)

namespace LimitEngine {
    class MetaData {
    public:
        // Type of metadata
        enum MetaDataType {
            MetaDataType_Variable = 0,
            MetaDataType_Function,
        };
        // Data type of metadata
        enum MetaDataVarType {
            MetaDataVarType_Unknown = 0,
            MetaDataVarType_Float,
            MetaDataVarType_FVector2,
            MetaDataVarType_FVector3,
            MetaDataVarType_FVector4,
            MetaDataVarType_String,
        };
        // Metadata item
        struct MetaDataItem {
            String name;
            MetaDataType type;
            String varTypeName;
            MetaDataVarType varType;
            uint_ptr dataOffset;
            MetaDataItem(MetaDataType t)
                : type(t)
            {}
            void SetVarType(const String &tn) 
            {
                varTypeName = tn;
                if(tn == "float") {
                    varType = MetaDataVarType_Float;
                }
                else if(tn == "LEMath::FloatVector2") {
                    varType = MetaDataVarType_FVector2;
                }
                else if(tn == "LEMath::FloatVector3") {
                    varType = MetaDataVarType_FVector3;
                }
                else if(tn == "LEMath::FloatVector4") {
                    varType = MetaDataVarType_FVector4;
                }
                else if(tn == "String") {
                    varType = MetaDataVarType_String;
                }
                else {
                    varType = MetaDataVarType_Unknown;
                }
            }
        };
        // Variable metadata
        struct MetaDataVariable : public MetaDataItem {
            MetaDataVariable()
                : MetaDataItem(MetaDataType_Variable)
            {}
        };
        // Function metadata
        struct MetaDataFunction : public MetaDataItem {
            MetaDataFunction()
                : MetaDataItem(MetaDataType_Function)
            {}
        };
    public:
        uint32 GetVariableCount() const { return static_cast<uint32>(mVariables.GetSize()); }
        const MetaDataVariable& GetVariable(uint32 index) const { return mVariables[index]; }
		VectorArray<MetaDataVariable>& GetVariables() { return mVariables; }
        void SetVariable(const MetaDataVariable &mdVal, float val) 
        {
            *(float*)((uint8*)this+mdVal.dataOffset) = val;
            VariableChanged(mdVal);
        }
        void SetVariable(const MetaDataVariable &mdVal, const LEMath::FloatVector2 &val) 
        {
            *(LEMath::FloatVector2*)((uint8*)this+mdVal.dataOffset) = val;
            VariableChanged(mdVal);
        }
        void SetVariable(const MetaDataVariable &mdVal, const LEMath::FloatVector3 &val) 
        {
            *(LEMath::FloatVector3*)((uint8*)this+mdVal.dataOffset) = val;
            VariableChanged(mdVal);
        }
        void SetVariable(const MetaDataVariable &mdVal, const LEMath::FloatVector4 &val) 
        {
            *(LEMath::FloatVector4*)((uint8*)this+mdVal.dataOffset) = val;
            VariableChanged(mdVal);
        }
        void SetVariable(const MetaDataVariable &mdVal, const String &val) 
        {
            *(String*)((uint8*)this+mdVal.dataOffset) = val;
            VariableChanged(mdVal);
        }
        uint32 GetFunctionCount() const { return static_cast<uint32>(mFunctions.GetSize()); }
        const MetaDataFunction& GetFunction(uint32 index) const { return mFunctions[index]; }
    protected:
        // Function for adding metadata
        void AddMetaDataVariable(const String &name, const String &varTypeName, uint_ptr offset)
        {
            MetaDataVariable &item = mVariables.Add();
            item.name = name;
            item.SetVarType(varTypeName);
            item.dataOffset = offset;
        }
        // Event when variable is changed
        virtual void VariableChanged(const MetaDataVariable &mdv) {}
        // Utility for pointer offset
        uint_ptr GetPointerOffset(void* From, void *To)
        {
            uintptr_t origin = reinterpret_cast<uintptr_t>(From);
            uintptr_t target = reinterpret_cast<uintptr_t>(To);
            return static_cast<uint_ptr>(target - origin);
        }
    private:
        VectorArray<MetaDataVariable> mVariables;
        VectorArray<MetaDataFunction> mFunctions;
    };
} // LimitEngine
#endif //  _METADATA_H_