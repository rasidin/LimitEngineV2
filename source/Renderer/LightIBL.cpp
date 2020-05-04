/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  LightIBL.cpp
 @brief Image based lighting Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Renderer/LightIBL.h"

#include "Managers/DrawManager.h"

namespace LimitEngine {
    LightIBL::LightIBL()
        : Light()
        , mMadeIBLMipmaps(false)
    {
        // Add variable for metadata
        //AddMetaDataVariable("IBLSpeuclarTexturePath", "String", (uint_ptr)&this->mIBLSpecularTexturePath - (uint_ptr)this);
        //AddMetaDataVariable("IBLDiffuseTexturePath", "String", (uint_ptr)&this->mIBLDiffuseTexturePath - (uint_ptr)this);
    }
    LightIBL::~LightIBL()
    {
    }
    void LightIBL::VariableChanged(const MetaDataVariable &mdv)
    {
        //if(mdv.dataOffset == ((uint_ptr)&this->mIBLSpecularTexturePath - (uint_ptr)this)) { // Changed IBLTexturePath
        //    // Check file path
        //    if(this->mIBLSpecularTexturePath.GetLength() == 0)
        //        return;
        //    if(LE_ResourceManager.IsExist(this->mIBLSpecularTexturePath.GetCharPtr()) == false)
        //        return;
        //    if(mIBLSpecularTexture)
        //        delete mIBLSpecularTexture;
        //    mIBLSpecularTexture = static_cast<Texture*>(LE_ResourceManager.GetResourceWithRegister(this->mIBLSpecularTexturePath)->data);
        //}
        //if(mdv.dataOffset == ((uint_ptr)&this->mIBLDiffuseTexturePath - (uint_ptr)this)) { // Changed IBLTexturePath
        //    // Check file path
        //    if(this->mIBLDiffuseTexturePath.GetLength() == 0)
        //        return;
        //    if(LE_ResourceManager.IsExist(this->mIBLDiffuseTexturePath.GetCharPtr()) == false)
        //        return;
        //    if(mIBLDiffuseTexture)
        //        delete mIBLDiffuseTexture;
        //    mIBLDiffuseTexture = static_cast<Texture*>(LE_ResourceManager.GetResourceWithRegister(this->mIBLDiffuseTexturePath)->data);
        //}
    }
    void LightIBL::Update()
    {
    }
} // LimitEngine