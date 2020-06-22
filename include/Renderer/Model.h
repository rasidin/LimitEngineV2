/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Model.h
 @brief Model Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#pragma once 

#include <LEIntVector3.h>
#include <LEFloatVector3.h>
#include <LEFloatMatrix4x4.h>

#include "Core/ReferenceCountedObject.h"
#include "Core/TextParser.h"
#include "Core/SerializableResource.h"
#include "Renderer/AABB.h"
#include "Renderer/FPolygon.h"
#include "Renderer/FRay.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/RenderState.h"
#include "Containers/VectorArray.h"
#include "Renderer/Vertex.h"
#include "Renderer/VertexBuffer.h"
#include "Core/String.h"
#include "Core/MetaData.h"

namespace LimitEngine {
    typedef Vertex<FVF_PNCTTB, SIZE_PNCTTB> RigidVertex;
    typedef VertexBuffer<FVF_PNCTTB, SIZE_PNCTTB> RigidVertexBuffer;
    class ModelFactory;
    class IndexBuffer;
    class Material;
    class Shader;
    class Model : public ReferenceCountedObject<LimitEngineMemoryCategory_Graphics>, public SerializableResource, public MetaData
    {
        friend ModelFactory;
    public:
        static constexpr uint32 FileType = GENERATE_SERIALIZABLERESOURCE_ID("MODL");

        typedef struct _DRAWGROUP
        {
            String                  materialID;

            Material                *material;
            VectorArray<LEMath::IntVector3>    indices;
            IndexBuffer             *indexBuffer;

            ~_DRAWGROUP()
            {
                if (indexBuffer) delete indexBuffer;
            }
            void InitResource();
        } DRAWGROUP;

        typedef struct _MESH
        {
            LEMath::FloatVector3                 pos;
            LEMath::FloatVector3                 scl;
            LEMath::FloatVector3                 rot;
            LEMath::FloatMatrix4x4               worldMatrix;
            
            VertexBufferGeneric*     vertexbuffer;
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
				if(vertexbuffer)
					delete vertexbuffer;
				vertexbuffer = NULL;
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

        virtual void InitResource() override;

    protected: // For serialization
        virtual bool Serialize(Archive &OutArchive) override;
        virtual SerializableResource* GenerateNew() const override { return new Model(); }

        virtual uint32 GetFileType() const override { return FileType; }
        virtual uint32 GetVersion() const override  { return 1u; }

    private:
		void setupMetaData();

        void Load(const char *text);
        void Load(TextParser::NODE *node);

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
