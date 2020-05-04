/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Sprite.cpp
 @brief Sprite
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/Sprite.h"

#include <LEIntVector2.h>
#include <LEIntVector4.h>
#include <LEFloatVector2.h>
#include <LEFloatVector3.h>
#include <LEFloatVector4.h>
#include <LEFloatMatrix4x4.h>

#include "Core/Util.h"
#include "Core/TextParser.h"
#include "Factories/TextureFactory.h"
#include "Managers/DrawManager.h"
#include "Managers/Draw2DManager.h"
#include "Managers/ResourceManager.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/DrawCommand.h"

namespace LimitEngine {
    // Generators
    Sprite* Sprite::GenerateFromFile(const char *ImageFilePath, const char *InfoFilePath)
    {
        Sprite *Output = new Sprite();

        if (ImageFilePath) {
            AutoPointer<ResourceManager::RESOURCE> TextureResource = LE_ResourceManager.GetResourceWithoutRegister(ImageFilePath, TextureFactory::ID);
            if (TextureResource.Exists()) {
                Texture *texture = static_cast<Texture*>(TextureResource->PopData());
                texture->CreateUsingSourceData();
                Output->SetTexture(texture);
            }
        }

        return Output;
    }

    Sprite::Sprite()
    {}
    Sprite::~Sprite()
    {
    }
    void Sprite::InitResource()
    {
        if (mTexture.IsValid()) {
            mTexture->InitResource();
        }
    }
    //void Sprite::LoadFromText(const char *text)
    //{
    //    TextParser parser;
    //    parser.Parse(text);
    //    TextParser::NODE *node = NULL;
    //    if ((node = parser.GetNode("FILETYPE")) && node->values[0] == "SPRITE")
    //    {
    //        if ((node = parser.GetNode("RESOURCE")))
    //        {
    //            _texture = static_cast<Texture*>(LE_ResourceManager.GetResourceWithRegister(node->values[0].GetCharPtr())->data);
    //        }
    //        if ((node = parser.GetNode("FRAMES")))
    //        {
    //            for (size_t i = 0; i<node->children.count(); i++)
    //            {
    //                TextParser::NODE *child = node->children[i];
    //                if (child)
    //                {
    //                    TextParser::NODE *node_rect = child->FindChild("RECT");
    //                    if (node_rect)
    //                    {
    //                        AddFrame(iRect(node_rect->values[0].ToInt(),
    //                            node_rect->values[1].ToInt(),
    //                            node_rect->values[2].ToInt(),
    //                            node_rect->values[3].ToInt()
    //                        ));
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
    uint32 Sprite::AddFrame(const LEMath::IntRect &frameRect)
    {
        if (!mTexture.IsValid()) return -1;
        const LEMath::IntSize& texSize = mTexture->GetSize();
        LEMath::FloatRect frame((float)frameRect.X()/texSize.Width(), (float)frameRect.Y()/texSize.Height(),
                    (float)frameRect.Width()/texSize.Width(), (float)frameRect.Height()/texSize.Height());
        mFrames.push_back(FRAME(frame));
        return static_cast<uint32>(mFrames.count()) - 1;
    }
  //  void Sprite::GenerateFrame(const iSize &frameSize)
  //  {
  //      uint32 pitch = _texture->GetSize().width / frameSize.width;
  //      uint32 line = _texture->GetSize().height / frameSize.height;
  //      for(uint32 y=0;y<line;y++)
  //      {
  //          for(uint32 x=0;x<pitch;x++)
  //          {
  //              AddFrame(iRect(x*frameSize.width, y*frameSize.height, 
  //                               frameSize.width,  frameSize.height));
  //          }
  //      }
  //  }
  //  void Sprite::Draw(uint32 frame, const iPoint &pos, float rotation)
  //  {
  //      Draw(frame, iRect(pos.x, pos.y, int32(_frames[frame].rect.width) * int32(_texture->GetSize().x), int32(_frames[frame].rect.height) * int32(_texture->GetSize().y)), rotation);
  //  }
  //  void Sprite::Draw(uint32 frame, const iRect &rect, float rotation)
  //  {
  //      if (frame >= _frames.GetSize()) return;
  //      if (_texture) DrawCommand::BindTexture(0, _texture);
  //      VertexDraw2D *buffer = LE_DrawManager.GetVertexBuffer2D(6);

  //      fSize screenSize(LE_DrawManager.GetVirtualScreenSize());
  //      fRect rectInProj = rect / screenSize;
  //      fPoint rightTop(float(rect.width), 0.0f);
  //      fPoint rightBottom(float(rect.width), float(rect.height));
  //      fPoint leftBottom(0.0f, float(rect.height));
  //      fMatrix4x4 rotmat = fMatrix4x4::RotationZ(rotation);
  //      rightBottom = rotmat * rightBottom / screenSize;
  //      rightBottom += rectInProj.xy();
  //      rightTop = rotmat * rightTop / screenSize;
  //      rightTop += rectInProj.xy();
  //      leftBottom = rotmat * leftBottom / screenSize;
  //      leftBottom += rectInProj.xy();

  //      buffer[0].SetPosition(rectInProj.xy());
  //      buffer[0].SetColor(0xffffffff);
  //      buffer[0].SetTexcoord(fVector2(_frames[frame].rect.x, _frames[frame].rect.y));
		//buffer[1].SetPosition(leftBottom);
  //      buffer[1].SetColor(0xffffffff);
  //      buffer[1].SetTexcoord(fVector2(_frames[frame].rect.x, _frames[frame].rect.y + _frames[frame].rect.height));
		//buffer[2].SetPosition(rightTop);
  //      buffer[2].SetColor(0xffffffff);
  //      buffer[2].SetTexcoord(fVector2(_frames[frame].rect.x + _frames[frame].rect.width, _frames[frame].rect.y));
		//buffer[3].SetPosition(leftBottom);
  //      buffer[3].SetColor(0xffffffff);
  //      buffer[3].SetTexcoord(fVector2(_frames[frame].rect.x, _frames[frame].rect.y + _frames[frame].rect.height));
		//buffer[4].SetPosition(rightBottom);
  //      buffer[4].SetColor(0xffffffff);
  //      buffer[4].SetTexcoord(fVector2(_frames[frame].rect.x + _frames[frame].rect.width, _frames[frame].rect.y + _frames[frame].rect.height));
		//buffer[5].SetPosition(rightTop);
  //      buffer[5].SetColor(0xffffffff);
  //      buffer[5].SetTexcoord(fVector2(_frames[frame].rect.x + _frames[frame].rect.width, _frames[frame].rect.y));
  //      LE_DrawManager.Draw2DFlush(GraphicsFlag::PT_TRIANGLELIST);
  //  }
    void Sprite::Draw(const LEMath::IntRect &frame, const LEMath::IntRect &rect, float rotation)
    {
        if (mTexture.IsValid()) DrawCommand::BindTexture(0, mTexture.Get());
        LEMath::FloatSize texSize(mTexture->GetSize());
        LEMath::FloatSize screenSize(LE_DrawManager.GetVirtualScreenSize());
        Vertex2D *buffer = LE_Draw2DManager.GetVertexBuffer2D(6);

        LEMath::FloatRect frameInProj = LEMath::FloatRect(frame) / LEMath::FloatVector4(texSize.X(), texSize.Y(), texSize.X(), texSize.Y());
        LEMath::FloatRect rectInProj = LEMath::FloatRect(rect) / LEMath::FloatVector4(screenSize.X(), screenSize.Y(), screenSize.X(), screenSize.Y());
        LEMath::FloatPoint rightTop(float(rect.Width()), 0.0f);
        LEMath::FloatPoint rightBottom(float(rect.Width()), float(rect.Height()));
        LEMath::FloatPoint leftBottom(0.0f, float(rect.Height()));
        LEMath::FloatMatrix4x4 rotmat = LEMath::FloatMatrix4x4::GenerateRotationZ(rotation);
        //rightBottom = rotmat * (LEMath::FloatVector4)rightBottom / (LEMath::FloatVector4)screenSize;
        rightBottom += rectInProj.XY();
        //rightTop = rotmat * rightTop / screenSize;
        rightTop += rectInProj.XY();
        //leftBottom = rotmat * leftBottom / screenSize;
        leftBottom += rectInProj.XY();
        buffer[0].SetPosition(LEMath::FloatVector3(rectInProj.XY()));
        buffer[0].SetColor(0xffffffff);
        buffer[0].SetTexcoord(LEMath::FloatVector2(frameInProj.XY()));
        buffer[1].SetPosition((LEMath::FloatVector3)leftBottom);
        buffer[1].SetColor(0xffffffff);
        buffer[1].SetTexcoord(LEMath::FloatVector2(frameInProj.X(), frameInProj.Y()+frameInProj.Height()));
        buffer[2].SetPosition((LEMath::FloatVector3)rightTop);
        buffer[2].SetColor(0xffffffff);
        buffer[2].SetTexcoord(LEMath::FloatVector2(frameInProj.X() + frameInProj.Width(), frameInProj.Y()));
        buffer[3].SetPosition((LEMath::FloatVector3)leftBottom);
        buffer[3].SetColor(0xffffffff);
        buffer[3].SetTexcoord(LEMath::FloatVector2(frameInProj.X(), frameInProj.Y() + frameInProj.Height()));
        buffer[4].SetPosition((LEMath::FloatVector3)rightBottom);
        buffer[4].SetColor(0xffffffff);
        buffer[4].SetTexcoord(LEMath::FloatVector2(frameInProj.X() + frameInProj.Width(), frameInProj.Y() + frameInProj.Height()));
        buffer[5].SetPosition((LEMath::FloatVector3)rightTop);
        buffer[5].SetColor(0xffffffff);
        buffer[5].SetTexcoord(LEMath::FloatVector2(frameInProj.X() + frameInProj.Width(), frameInProj.Y()));
        LE_Draw2DManager.FlushDraw2D(RendererFlag::PrimitiveTypes::TRIANGLELIST);
    }
    void Sprite::BeginBatchDraw()
    {
        // Do nothing
    }
    void Sprite::BatchDraw(uint32 frame, const LEMath::IntPoint &pos, CoordinateType CoordType)
    {
        if (!mTexture.IsValid() || frame >= mFrames.count()) return;

        if (!LE_DrawManager.IsReadyToRender()) return;

        DrawCommand::BindTexture(0, mTexture.Get());
        const LEMath::FloatSize texSize(mTexture->GetSize());
        const LEMath::FloatSize screenSize(CoordType==CoordinateType::VirtualCoordinate?LE_DrawManager.GetVirtualScreenSize():LE_DrawManager.GetRealScreenSize());
        Vertex2D *buffer = LE_Draw2DManager.GetVertexBuffer2D(6);

        const LEMath::FloatRect frameInProj = mFrames[frame].mRect;
        const LEMath::FloatRect rectInProj = LEMath::FloatRect(static_cast<float>(pos.X()), static_cast<float>(pos.Y()), mFrames[frame].mRect.Width() * texSize.X(), mFrames[frame].mRect.Height() * texSize.Y()) / LEMath::FloatVector4(screenSize.X(), screenSize.Y(), screenSize.X(), screenSize.Y());
        
        const LEMath::FloatPoint posLeftTop = rectInProj.XY();
        const LEMath::FloatPoint posRightTop(rectInProj.X() + rectInProj.Width(), rectInProj.Y());
        const LEMath::FloatPoint posLeftBottom(rectInProj.X(), rectInProj.Y() + rectInProj.Height());
        const LEMath::FloatPoint posRightBottom(rectInProj.X() + rectInProj.Width(), rectInProj.Y() + rectInProj.Height());

        const LEMath::FloatPoint texLeftTop = frameInProj.XY();
        const LEMath::FloatPoint texRightTop(frameInProj.X() + frameInProj.Width(), frameInProj.Y());
        const LEMath::FloatPoint texLeftBottom(frameInProj.X(), frameInProj.Y() + frameInProj.Height());
        const LEMath::FloatPoint texRightBottom(frameInProj.X() + frameInProj.Width(), frameInProj.Y() + frameInProj.Height());

        buffer[0].SetPosition((LEMath::FloatVector3)posLeftTop);
        buffer[0].SetColor(0xffffffff);
        buffer[0].SetTexcoord(texLeftTop);

        buffer[1].SetPosition((LEMath::FloatVector3)posLeftBottom);
        buffer[1].SetColor(0xffffffff);
        buffer[1].SetTexcoord(texLeftBottom);

        buffer[2].SetPosition((LEMath::FloatVector3)posRightTop);
        buffer[2].SetColor(0xffffffff);
        buffer[2].SetTexcoord(texRightTop);

        buffer[3].SetPosition((LEMath::FloatVector3)posLeftBottom);
        buffer[3].SetColor(0xffffffff);
        buffer[3].SetTexcoord(texLeftBottom);

        buffer[4].SetPosition((LEMath::FloatVector3)posRightBottom);
        buffer[4].SetColor(0xffffffff);
        buffer[4].SetTexcoord(texRightBottom);

        buffer[5].SetPosition((LEMath::FloatVector3)posRightTop);
        buffer[5].SetColor(0xffffffff);
        buffer[5].SetTexcoord(texRightTop);
    }
    void Sprite::EndBatchDraw(Shader *InShader)
    {
        LE_Draw2DManager.FlushDraw2D(RendererFlag::PrimitiveTypes::TRIANGLELIST, InShader);
    }
    bool Sprite::Serialize(Archive &OutArchive)
    {
        OutArchive << (SerializableResource*)&mFrames;

        if (mTexture.IsValid() == false)
            mTexture = new Texture();
        OutArchive << (SerializableResource*)mTexture.Get();
        return true;
    }
  //  
  //  fRect Sprite::GetFrameRect(uint32 f)
  //  { return fRect(_frames[f].rect * _texture->GetSize()); }
}