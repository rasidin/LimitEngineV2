/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file Model.h
@brief Model
@author minseob (https://github.com/rasidin)
**********************************************************************/
#ifndef LIMITENGINEV2_RENDERER_MODEL_H_
#define LIMITENGINEV2_RENDERER_MODEL_H_

#include <LERenderer>

#include <LEIntVector3.h>
#include <LEFloatVector3.h>
#include <LEFloatMatrix4x4.h>

#include "Containers/VectorArray.h"
#include "Core/ReferenceCountedObject.h"
#include "Core/TextParser.h"
#include "Core/SerializableResource.h"
#include "Renderer/AABB.h"
#include "Renderer/FPolygon.h"
#include "Renderer/FRay.h"
#include "Renderer/PipelineState.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/RenderState.h"
#include "Renderer/Vertex.h"
#include "Renderer/VertexBuffer.h"
#include "Core/String.h"
#include "Core/MetaData.h"

#include "../externals/rapidxml/rapidxml.hpp"

namespace LimitEngine {
    typedef Vertex<FVF_PNCTTB, SIZE_PNCTTB> RigidVertex;
    typedef VertexBuffer<FVF_PNCTTB, SIZE_PNCTTB> RigidVertexBuffer;
    class ModelFactory;
    class IndexBuffer;
    class Material;
    class Shader;
    class Model : public ReferenceCountedObject<LimitEngineMemoryCategory::Graphics>, public SerializableResource, public MetaData
    {
        friend ModelFactory;
    public:
        static constexpr uint32 FileType = GENERATE_SERIALIZABLERESOURCE_ID("MODL");

        typedef struct _DRAWGROUP
        {
            String                              materialID;

            Material                           *material;
            VectorArray<LEMath::IntVector3>     indices;
            IndexBufferRefPtr                   indexBuffer;

            PipelineStateRefPtr                 pipelinestates[static_cast<int>(RenderPass::NumOfRenderPass)];

            ~_DRAWGROUP()
            {
                indexBuffer = nullptr;
                for (int psidx = 0; psidx < static_cast<int>(RenderPass::NumOfRenderPass); psidx++) {
                    pipelinestates[psidx].Release();
                }
            }
            void InitResource();
        } DRAWGROUP;

        typedef struct _MESH
        {
            LEMath::FloatVector3     pos;
            LEMath::FloatVector3     scl;
            LEMath::FloatVector3     rot;
            LEMath::FloatMatrix4x4   worldMatrix;
            
            VertexBufferRefPtr       vertexbuffer;
            VectorArray<DRAWGROUP*>  drawgroups;
            _MESH()
            {
                pos = LEMath::FloatVector3::Zero;
                scl = LEMath::FloatVector3::One;
                rot = LEMath::FloatVector3::Zero;
                vertexbuffer = NULL;
                drawgroups.Clear();
            }
            ~_MESH()
            {
                for(uint32 i=0;i<drawgroups.size();i++)
                    delete drawgroups[i];
                drawgroups.Clear();
				vertexbuffer = nullptr;
            }
            void Preprocess()
            {
                worldMatrix = LEMath::FloatMatrix4x4::GenerateTransform((LEMath::FloatVector4)pos) * LEMath::FloatMatrix4x4::GenerateRotationXYZ((LEMath::FloatVector4)rot) * LEMath::FloatMatrix4x4::GenerateScaling((LEMath::FloatVector4)scl);
            }
            void InitResource();
            DRAWGROUP* AddDrawGroup() {
                DRAWGROUP *out = new DRAWGROUP();
                drawgroups.push_back(out);
                return out;
            }
        } MESH;
    public:
        Model();
        Model(const char *filename);
        virtual ~Model();

        AABB GetBoundingBox() { return mBoundingbox; }

        void Draw(const RenderState &rs, const LEMath::FloatMatrix4x4 &Transform);

        void SetName(const String &name)        { mName = name; }
        String GetName()                        { return mName; }
        uint32 GetMeshCount()                   { return mMeshes.size(); }
        MESH* GetMesh(uint32 n)                 { return mMeshes[n]; }
        uint32 GetMaterialCount()               { return mMaterials.size(); }
        Material* GetMaterial(uint32 n)         { return mMaterials[n]; }

		void SetPosition(const LEMath::FloatVector3 &p)     { mPosition = p; }
        void SetScale(const LEMath::FloatVector3 &s)        { mScale = s; }
        void SetRotation(const LEMath::FloatVector3 &r)     { mRotation = r; }

        bool IsInBoundingBox(const LEMath::FloatVector3 &v);
        
        fPolygon::INTERSECT_RESULT Intersect(const fRay &r);
		fPolygon::INTERSECT_RESULT IntersectSphere(const fRay &r, float radius);

    public: // Generator
        static Model* GenerateFromTextParser(const ReferenceCountedPointer<TextParser> &Parser);
        static Model* GenerateFromXML(const rapidxml::xml_document<const char> *XMLDoc);

        virtual void InitResource() override;

    protected: // For serialization
        virtual bool Serialize(Archive &OutArchive) override;
        virtual SerializableResource* GenerateNew() const override { return new Model(); }

        virtual uint32 GetFileType() const override { return FileType; }
        virtual uint32 GetVersion() const override  { return 1u; }

    private:
		void setupMetaData();

        void Load(const char *text);
        Model* Load(TextParser::NODE *node);
        Model* Load(const rapidxml::xml_node<const char> *XMLNode);

        void calcTangentBinormal();
        void setupMaterialShaderParameters();
        LEMath::FloatMatrix4x4 getTransformMatrix();
    private:
        AABB                     mBoundingbox;
        
        VectorArray<MESH*>       mMeshes;
        
        LEMath::FloatVector3     mBasePosition;
        LEMath::FloatVector3     mBaseScale;
        LEMath::FloatVector3     mBaseRotation;
        LEMath::FloatMatrix4x4   mBaseMatrix;
        
        String                   mName;
        LEMath::FloatVector3     mPosition;
        LEMath::FloatVector3     mScale;
        LEMath::FloatVector3     mRotation;
        VectorArray<Material*>   mMaterials;
    };
}
#endif // LIMITENGINEV2_RENDERER_MODEL_H_