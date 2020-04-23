/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  SceneManager.cpp
 @brief SceneManager Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Managers/SceneManager.h"

#include <LEFloatVector4.h>

#include "Core/Debug.h"
#include "Core/TaskPriority.h"
#include "Core/TextParser.h"
#include "Managers/DrawManager.h"
#include "Managers/Draw2DManager.h"
//#include "Managers/LightManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/ShaderManager.h"
#include "Renderer/Camera.h"
#include "Renderer/DrawCommand.h"
#include "Renderer/Model.h"
#include "Renderer/Light.h"
#include "Renderer/Texture.h"

namespace LimitEngine {
#ifdef WIN32
SceneManager* SingletonSceneManager::mInstance = NULL;
#endif
SceneManager::SceneManager()
    : mCamera(new Camera())
    , mOwnCamera(true)
{
}

SceneManager::~SceneManager()
{
    if (mCamera && mOwnCamera) delete mCamera; mCamera = NULL;
}

void SceneManager::Init(const InitializeOptions &InitOptions)
{
    mBackgroundShaders.Resize(static_cast<uint32>(BackgroundImageType::Num));
    mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Fullscreen)] = LE_ShaderManager.GetShader("DrawFullscreen");
    mBackgroundShaders[static_cast<uint32>(BackgroundImageType::Longlat)] = LE_ShaderManager.GetShader("Draw2D");

    TEXTURE_COLOR_FORMAT SceneColorFormat = (InitOptions.SceneColorSpace == InitializeOptions::ColorSpace::Linear) ? TEXTURE_COLOR_FORMAT_A16B16G16R16F : TEXTURE_COLOR_FORMAT_A8R8G8B8;
    mSceneColor = LE_RenderTargetPoolManager.GetRenderTarget(InitOptions.Resolution, 1, SceneColorFormat);
    mSceneDepth = LE_RenderTargetPoolManager.GetDepthStencil(InitOptions.Resolution, TEXTURE_DEPTH_FORMAT_D32F);
}

void SceneManager::SetBackgroundImage(Texture *BackgroundImage, BackgroundImageType Type)
{
    mBackgroundImage = BackgroundImage;
    mBackgroundType = Type;
}

void SceneManager::LoadFromText(const char *text)
{
    TextParser parser;
    parser.Parse(text);
    TextParser::NODE *node = NULL;
    if ((node = parser.GetNode("FILETYPE")) && node->values[0] == "SCENE")
    {
        TextParser::NODE *node_data = parser.GetNode("DATA");
        if (node_data)
        {
            // Models
            TextParser::NODE *node_models = node_data->FindChild("MODELS");
            if (node_models)
            {
                for(uint32 i=0;i<node_models->children.size();i++)
                {
                    TextParser::NODE *child = node_models->children[i];
                    TextParser::NODE *node_filename = NULL;
                    if ( child->name == "MODEL" && 
                        (node_filename = child->FindChild("FILENAME")))
                    {   // MODEL
                        Model *model = new Model(node_filename->values[0].GetCharPtr());
                        TextParser::NODE *node_position = child->FindChild("POSITION");
                        if (node_position) model->SetPosition(node_position->ToFloatVector3());
                        TextParser::NODE *node_scale = child->FindChild("SCALE");
                        if (node_scale) model->SetScale(node_scale->ToFloatVector3());
                        TextParser::NODE *node_rotation = child->FindChild("ROTATION");
                        if (node_rotation) model->SetRotation(node_rotation->ToFloatVector3());
                        mModels.push_back(model);
                    }
                }
            }

            // Lights
            TextParser::NODE *node_lights = node_data->FindChild("LIGHTS");
            if (node_lights)
            {
                for(uint32 i=0;i<node_lights->children.size();i++)
                {
                    Light *light = NULL;
                    TextParser::NODE *child = node_lights->children[i];
                    TextParser::NODE *node_type = child->FindChild("TYPE");
                    if (node_type->values[0] == "Point")
                    {
                        light = new PointLight();
                    }
                    else if (node_type->values[0] == "Directional")
                    {
                        light = new DirectionalLight();
                    }
                    else if (node_type->values[0] == "Ambient")
                    {
                        light = new AmbientLight();
                    }
                    if (!light) continue;
                    TextParser::NODE *node_color = child->FindChild("COLOR");
                    if (node_color)
                    {
                        light->SetColor(node_color->ToFloatVector3());
                    }
                    TextParser::NODE *node_intensity = child->FindChild("INTENSITY");
                    if (node_intensity)
                    {
                        light->SetIntensity(node_intensity->values[0].ToFloat());
                    }
                    TextParser::NODE *node_pos = child->FindChild("POSITION");
                    if (node_pos && light->GetType() == Light::TYPE_POINT)
                    {
                        ((PointLight*)light)->SetPosition(node_pos->ToFloatVector3());
                    }
                    TextParser::NODE *node_scl = child->FindChild("SCALE");
                    if (node_scl && light->GetType() == Light::TYPE_POINT)
                    {
                        ((PointLight*)light)->SetScale(node_scl->ToFloatVector3());
                    }
                    switch(light->GetType())
                    {
                    case Light::TYPE_POINT:
                        {
                            TextParser::NODE *node_range = child->FindChild("RANGE");
                            if (node_range)
                            {
                                ((PointLight*)light)->SetRange(node_range->values[0].ToFloat());
                            }
                            TextParser::NODE *node_exp = child->FindChild("EXPONENT");
                            if (node_exp)
                            {
                                ((PointLight*)light)->SetExponent(node_exp->values[0].ToFloat());
                            }
                        } break;
                    }
                    AddLight(light);
                }
            }
        }
    }
}

void SceneManager::AddLight(Light *light)
{
    //LE_LightManager.AddLight(light);
	mOnChangeEvent();
}

void SceneManager::Update()
{
    mCamera->Update();
    LE_DrawManager.SetViewMatrix(mCamera->GetViewMatrix());
    LE_DrawManager.SetProjectionMatrix(mCamera->GetProjectionMatrix());
    LE_DrawManager.UpdateMatrices();
}

void SceneManager::drawBackground()
{
    switch (mBackgroundType) {
    case BackgroundImageType::None:
        DrawCommand::ClearScreen(LEMath::FloatColorRGBA(0.0f, 0.0f, 0.0f, 1.0f));
        break;
    case BackgroundImageType::Fullscreen: 
    case BackgroundImageType::Longlat:
    {
        if (mBackgroundImage.IsValid()) DrawCommand::BindTexture(0, mBackgroundImage.Get());
        DrawCommand::SetBlendFunc(0, RendererFlag::BlendFlags::SOURCE);
        DrawCommand::SetDepthFunc(RendererFlag::TestFlags::ALWAYS);

        Vertex2D *buffer = LE_Draw2DManager.GetVertexBuffer2D(6);
        buffer[0].SetPosition(LEMath::FloatVector3(0.0f, 0.0f, 0.0f));
        buffer[0].SetColor(0xffffffff);
        buffer[0].SetTexcoord(LEMath::FloatVector2(0.0f, 0.0f));
        buffer[1].SetPosition(LEMath::FloatVector3(0.0f, 1.0f, 0.0f));
        buffer[1].SetColor(0xffffffff);
        buffer[1].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
        buffer[2].SetPosition(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
        buffer[2].SetColor(0xffffffff);
        buffer[2].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
        buffer[3].SetPosition(LEMath::FloatVector3(0.0f, 1.0f, 0.0f));
        buffer[3].SetColor(0xffffffff);
        buffer[3].SetTexcoord(LEMath::FloatVector2(0.0f, 1.0f));
        buffer[4].SetPosition(LEMath::FloatVector3(1.0f, 1.0f, 0.0f));
        buffer[4].SetColor(0xffffffff);
        buffer[4].SetTexcoord(LEMath::FloatVector2(1.0f, 1.0f));
        buffer[5].SetPosition(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
        buffer[5].SetColor(0xffffffff);
        buffer[5].SetTexcoord(LEMath::FloatVector2(1.0f, 0.0f));
        LE_Draw2DManager.FlushDraw2D(RendererFlag::PrimitiveTypes::TRIANGLELIST);
    }   break;
    }
}

void SceneManager::Draw()
{
    DrawCommand::SetRenderTarget(0, mSceneColor.Get(), mSceneDepth.Get());
    DrawCommand::BeginScene();
    drawBackground();
    //LE_LightManager.DrawBackground(LE_DrawManager.GetRenderState());
    for(uint32 mdlidx=0;mdlidx<mModels.count();mdlidx++) {
        mModels[mdlidx]->Draw(LE_DrawManager.GetRenderState());
    }
    //LE_LightManager.DrawForeground(LE_DrawManager.GetRenderState());
    DrawCommand::EndScene();
    DrawCommand::SetRenderTarget(0, nullptr, nullptr);
}
    
void SceneManager::SetCamera(Camera *c)
{
    if (mCamera && mOwnCamera) delete mCamera;
    mCamera = c;
    mOwnCamera = false;
	mOnChangeEvent();
}
}