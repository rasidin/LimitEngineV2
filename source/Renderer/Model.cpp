/***********************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  Model.cpp
 @brief Model Class
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/

#include "Renderer/Model.h"

#include <LEFloatVector3.h>
#include <LEFloatMatrix4x4.h>

#include "Core/Object.h"
#include "Managers/ResourceManager.h"
#include "Core/TextParser.h"
#include "Core/Util.h"
#include "Renderer/Vertex.h"
#include "Managers/ShaderManager.h"
//#include "Managers/LightManager.h"
#include "Managers/DrawManager.h"
#include "Renderer/Material.h"

namespace LimitEngine {
    Model::Model()
    : mBoundingbox()
    , mMaterials()
    , mPosition(LEMath::FloatVector3::Zero)
    , mScale(LEMath::FloatVector3::One)
    , mRotation(LEMath::FloatVector3::Zero)
    , mBasePosition(LEMath::FloatVector3::Zero)
    , mBaseScale(LEMath::FloatVector3::One)
    , mBaseRotation(LEMath::FloatVector3::Zero)
    {
		setupMetaData();
    }
    Model::Model(const char *filename)
    : mBoundingbox()
    , mMaterials()
    , mPosition(LEMath::FloatVector3::Zero)
    , mScale(LEMath::FloatVector3::One)
    , mRotation(LEMath::FloatVector3::Zero)
    , mBasePosition(LEMath::FloatVector3::Zero)
    , mBaseScale(LEMath::FloatVector3::One)
    , mBaseRotation(LEMath::FloatVector3::Zero)
    {
		setupMetaData();
		
		Load(filename);
    }
	void Model::setupMetaData()
	{
		AddMetaDataVariable("Position", "LEMath::FloatVector3", METADATA_POINTER(mPosition));
		AddMetaDataVariable("Scale",    "LEMath::FloatVector3", METADATA_POINTER(mScale));
		AddMetaDataVariable("Rotation", "LEMath::FloatVector3", METADATA_POINTER(mRotation));
	}
    Model::~Model()
    {
        for (size_t i=0;i<mMeshes.count();i++) {
            if(mMeshes[i])
                delete mMeshes[i];
        }
        mMeshes.Clear();
        
        for(size_t i=0;i<mMaterials.count();i++) {
            if (mMaterials[i])
                delete mMaterials[i];
        }
        mMaterials.Clear();
    }
    void Model::Load(const char *text)
    {
        TextParser parser;
        parser.Parse(text);
        TextParser::NODE *node = NULL;
        if ((node = parser.GetNode("FILETYPE")) && node->values[0] == "MODEL")
        {
            TextParser::NODE *nodemName = parser.GetNode("NAME");
            if (nodemName) mName = nodemName->values[0].GetCharPtr();
            Load(parser.GetNode("DATA"));
        }
    }
    void Model::Load(TextParser::NODE *root)
    {
        if (!root) return;
        TextParser::NODE *node = NULL;
        if ((node = root->FindChild("MATERIALS")))
        {
            for (size_t i=0;i<node->children.count();i++) {
                Material *material = new Material();
                TextParser::NODE *matNode = node->children[i];
                material->Load(matNode);
                mMaterials.Add(material);
            }
        }
        if ((node = root->FindChild("TRANSFORM")))
        {
            for (size_t i=0;i<node->children.size();i++) {
                TextParser::NODE *transNode = node->children[i];
                if (transNode) {
                    if (transNode->name == "POSITION") {
                        mBasePosition = transNode->ToFloatVector3();
                    }
                    if (transNode->name == "SCALE") {
                        mBaseScale = transNode->ToFloatVector3();
                    }
                    if (transNode->name == "ROTATION") {
                        mBaseRotation = transNode->ToFloatVector3();
                    }
                }
            }
            mBaseMatrix = LEMath::FloatMatrix4x4::GenerateTransform(mBasePosition) * LEMath::FloatMatrix4x4::GenerateRotationXYZ(mBaseRotation) * LEMath::FloatMatrix4x4::GenerateScaling(mBaseScale);
        }
        if ((node = root->FindChild("ELEMENTS")))
        {
            for (size_t j=0;j<node->children.count();j++) {
                if (TextParser::NODE *eleNode = node->children[j]) {
                    if (eleNode->name == "MESH") {
                        MESH *mesh = new MESH();
                        mMeshes.push_back(mesh);
                        TextParser::NODE *tn = NULL;
                        if ((tn = eleNode->FindChild("TRANSFORM")))
                        {
                            for (size_t i=0;i<tn->children.size();i++) {
                                TextParser::NODE *transNode = tn->children[i];
                                if (transNode) {
                                    if (transNode->name == "POSITION") {
                                        mesh->pos = transNode->ToFloatVector3();
                                    }
                                    if (transNode->name == "SCALE") {
                                        mesh->scl = transNode->ToFloatVector3();
                                    }
                                    if (transNode->name == "ROTATION") {
                                        mesh->rot = transNode->ToFloatVector3();
                                    }
                                }
                            }
                            mesh->Preprocess();
                        }
                        TextParser::NODE *verticesNode = eleNode->FindChild("VERTICES");
                        if (verticesNode) { // Make vertices
                            mesh->vertexbuffer = new RigidVertexBuffer();
                            RigidVertexBuffer *vtxbuf = (VertexBuffer<FVF_PNCTTB, SIZE_PNCTTB>*)mesh->vertexbuffer;
                            RigidVertex *vtxptr = new RigidVertex[verticesNode->children.count()]();
                            for (size_t i=0;i<verticesNode->children.count();i++) {
                                TextParser::NODE *vtxNode = verticesNode->children[i];
                                TextParser::NODE *vtxPos = vtxNode->FindChild("POSITION");
                                if (vtxPos) {
                                    LEMath::FloatVector3 vPos(vtxPos->ToFloatVector3());
                                    vtxptr[i].SetPosition(vPos);
                                    mBoundingbox |= vPos;
                                }
                                TextParser::NODE *vtxNrm = vtxNode->FindChild("NORMAL");
                                if (vtxNrm) {
                                    vtxptr[i].SetNormal(vtxNrm->ToFloatVector3());
                                }
                                TextParser::NODE *vtxTan = vtxNode->FindChild("TANGENT");
                                if (vtxTan) {
                                    vtxptr[i].SetTangent(vtxTan->ToFloatVector3());
                                }
                                TextParser::NODE *vtxBN = vtxNode->FindChild("BINORMAL");
                                if (vtxBN) {
                                    vtxptr[i].SetBinormal(vtxBN->ToFloatVector3());
                                }
                                TextParser::NODE *vtxUV = vtxNode->FindChild("TEXCOORD");
                                if (vtxUV) {
                                    vtxptr[i].SetTexcoord(vtxUV->ToFloatVector2());
                                }
                                TextParser::NODE *vtxCol = vtxNode->FindChild("COLOR");
                                if (vtxCol) {
                                    vtxptr[i].SetColor(vtxCol->ToByteColorRGBA());
                                }
                            }
                            vtxbuf->Create(verticesNode->children.count(), vtxptr, 0);
                            delete[] vtxptr;
                        }
                        TextParser::NODE *indicesNode = eleNode->FindChild("INDICES");
                        if (indicesNode) { // Make indices
                            for(size_t i=0;i<indicesNode->children.count();i++) {
                                TextParser::NODE *idxNode = indicesNode->children[i];
                                DRAWGROUP *drawgroup = NULL;
                                if (TextParser::NODE *materialNode = idxNode->FindChild("MATERIAL")) {
                                    String matname = materialNode->values[0];
                                    for(size_t l=0;l<mesh->drawgroups.count();l++)
                                    {
                                        if (mesh->drawgroups[l]->material->GetID() == matname)
                                        {
                                            drawgroup = mesh->drawgroups[l];
                                            break;
                                        }
                                    }
                                    if (!drawgroup)
                                    {
                                        Material *material = NULL;
                                        for(size_t l=0;l<mMaterials.count();l++)
                                        {
                                            if (mMaterials[l]->GetID() == matname)
                                            {
                                                material = mMaterials[l];
                                                break;
                                            }
                                        }
                                        if (material)
                                        {
                                            drawgroup = mesh->AddDrawGroup();
                                            drawgroup->material = material;
                                        }
                                    }
                                }
                                if (!drawgroup) continue;
                                if (TextParser::NODE *polygonNode = idxNode->FindChild("POLYGON")) {
                                    drawgroup->indices.push_back(polygonNode->ToIntVector3());
                                }
                            }
                        }
                        // Set IndexBuffer
                        for (size_t i=0;i<mesh->drawgroups.count();i++)
                        {
                            mesh->drawgroups[i]->indexBuffer = new IndexBuffer();
                            mesh->drawgroups[i]->indexBuffer->Create(mesh->drawgroups[i]->indices.count()*3, &mesh->drawgroups[i]->indices[0]);
                        }
                    }
                }
            }
        }
        // Postprocess
        calcTangentBinormal();
        setupMaterialShaderParameters();
    }
    void Model::Draw(const RenderState &rs)
    {
        DrawCommand::SetCulling(static_cast<uint32>(RendererFlag::Culling::CounterClockWise));
        DrawCommand::BeginDrawing();
        // Calculate Matrix
        LEMath::FloatMatrix4x4 globalTransMatrix = getTransformMatrix();
        LEMath::FloatMatrix4x4 modelWvpMat = LEMath::FloatMatrix4x4(rs.GetViewProjMatrix()) * globalTransMatrix;

        for (size_t i=0;i<mMeshes.size();i++)
        {
            MESH *mesh = mMeshes[i];
            for(size_t j=0;j<mesh->drawgroups.count();j++)
            {
                DRAWGROUP *drawGroup = mesh->drawgroups[j];
                // Set material
                if (Material *material = drawGroup->material)
                {
                    // Set RenderState
                    RenderState rsCopied(rs);
                    rsCopied.SetWorldMatrix(mesh->worldMatrix.Transpose());
                    rsCopied.SetWorldViewProjMatrix((modelWvpMat * mMeshes[i]->worldMatrix).Transpose());
                    //LE_LightManager.ApplyLight(rsCopied, NULL);

                    // Bind material
                    material->Bind(rsCopied);
                }
                // Draw
                ((RigidVertexBuffer *)mesh->vertexbuffer)->BindToDrawManager();
                if (drawGroup->indexBuffer)
                    drawGroup->indexBuffer->Bind();
                DrawCommand::DrawIndexedPrimitive( RendererFlag::PrimitiveTypes::TRIANGLELIST,
                                                   static_cast<uint32>(((RigidVertexBuffer *)mesh->vertexbuffer)->GetSize()), 
                                                   static_cast<uint32>(drawGroup->indexBuffer->GetSize()));
            }
        }
        DrawCommand::EndDrawing();
    }
    bool Model::IsInBoundingBox(const LEMath::FloatVector3 &v)
    {
        AABB transformedBB = mBoundingbox.Transform(getTransformMatrix());
        return transformedBB.IsIn(v);
    }
    fPolygon::INTERSECT_RESULT
    Model::Intersect(const fRay &ray)
    {
        AABB transformedBB = mBoundingbox.Transform(getTransformMatrix());
        AABB::INTERSECT_RESULT result = transformedBB.Intersect(ray);
        if (result.key && result.value.X() > 0) {
            if (ray.GetLength() > result.value.X()) {
                return fPolygon::INTERSECT_RESULT(true, result.value.X());
            }
        }
        return fPolygon::INTERSECT_FAIL;
    }
    fPolygon::INTERSECT_RESULT 
    Model::IntersectSphere(const fRay &ray, float radius)
    {
        AABB transformedBB = mBoundingbox.Transform(getTransformMatrix());
        AABB::INTERSECT_RESULT result = transformedBB.Intersect(ray);
        if (result.key && result.value.X() > 0) {
            LEMath::FloatVector3 normalBB = transformedBB.GetNormal(ray.org + ray.GetDirection() * result.value.x);
            float distFromPoint = radius / (normalBB | ray.GetDirection());
            float finalDist = result.value.X() + distFromPoint;
            if (finalDist < ray.GetLength())
                return fPolygon::INTERSECT_RESULT(true, finalDist);
        }
        return fPolygon::INTERSECT_FAIL;
    }
    LEMath::FloatMatrix4x4 Model::getTransformMatrix()
    {
        LEMath::FloatMatrix4x4 transMatrix = LEMath::FloatMatrix4x4::GenerateTransform(mPosition).Transpose();
        LEMath::FloatMatrix4x4 rotXMatrix = LEMath::FloatMatrix4x4::GenerateRotationX(mRotation.X());
        LEMath::FloatMatrix4x4 rotYMatrix = LEMath::FloatMatrix4x4::GenerateRotationY(mRotation.Y());
        LEMath::FloatMatrix4x4 rotZMatrix = LEMath::FloatMatrix4x4::GenerateRotationZ(mRotation.Z());
        LEMath::FloatMatrix4x4 sclMatrix = LEMath::FloatMatrix4x4::GenerateScaling(mScale);
        return mBaseMatrix * sclMatrix * rotZMatrix * rotXMatrix * rotYMatrix * transMatrix;
    }
    void Model::setupMaterialShaderParameters()
    {
        for (uint32 matidx = 0; matidx < mMaterials.size();matidx++) {
            mMaterials[matidx]->SetupShaderParameters();
        }
    }
    void Model::calcTangentBinormal()
    {
        for (size_t i=0;i<mMeshes.size();i++)
        {
            RigidVertex *vtxptr = (RigidVertex*)((RigidVertexBuffer *)mMeshes[i]->vertexbuffer)->GetBuffer();
            if (vtxptr == NULL || vtxptr[0].GetBinormal() != LEMath::FloatVector3::Zero && vtxptr[0].GetTangent() != LEMath::FloatVector3::Zero)
                continue;
            for (size_t l=0;l<mMeshes[i]->drawgroups.size();l++)
            {
                for (size_t m=0;m<mMeshes[i]->drawgroups[l]->indices.size();m++)
                {
                    int idx1 = mMeshes[i]->drawgroups[l]->indices[m].x;
                    int idx2 = mMeshes[i]->drawgroups[l]->indices[m].y;
                    int idx3 = mMeshes[i]->drawgroups[l]->indices[m].z;
                    LEMath::FloatVector3 p1 = vtxptr[idx1].GetPosition();
                    LEMath::FloatVector3 p2 = vtxptr[idx2].GetPosition();
                    LEMath::FloatVector3 p3 = vtxptr[idx3].GetPosition();
                    LEMath::FloatVector2 t1 = vtxptr[idx1].GetTexcoord();
                    LEMath::FloatVector2 t2 = vtxptr[idx2].GetTexcoord();
                    LEMath::FloatVector2 t3 = vtxptr[idx3].GetTexcoord();
                    LEMath::FloatVector3 p21 = p2 - p1;
                    LEMath::FloatVector3 p31 = p3 - p1;
                    LEMath::FloatVector2 t21 = t2 - t1;
                    LEMath::FloatVector2 t31 = t3 - t1;
                    LEMath::FloatVector3 n1 = vtxptr[idx1].GetNormal();
                    LEMath::FloatVector3 n2 = vtxptr[idx2].GetNormal();
                    LEMath::FloatVector3 n3 = vtxptr[idx3].GetNormal();
                    
                    LEMath::FloatVector3 tangent =  (p21 * t31.Y() - p31 * t21.Y()).Normalize();
                    LEMath::FloatVector3 binormal = (p31 * t21.X() - p21 * t31.X()).Normalize();
                    LEMath::FloatVector3 normal = tangent ^ binormal;
                    tangent = (tangent - normal * (normal | tangent)).Normalize();
                    bool rightHanded = ((tangent ^ binormal) | (n1)) >= 0;
                    binormal = (normal ^ tangent);
                    if (!rightHanded)
                    {
                        binormal = binormal * -1.0f;
                    }
                    vtxptr[idx1].SetTangent(tangent);
                    vtxptr[idx2].SetTangent(tangent);
                    vtxptr[idx3].SetTangent(tangent);
                    vtxptr[idx1].SetBinormal(binormal);
                    vtxptr[idx2].SetBinormal(binormal);
                    vtxptr[idx3].SetBinormal(binormal);
                    vtxptr[idx1].SetNormal(normal);
                    vtxptr[idx2].SetNormal(normal);
                    vtxptr[idx3].SetNormal(normal);
                }
            }
        }
    }
}