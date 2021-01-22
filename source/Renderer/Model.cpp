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
    template<> Archive& Archive::operator << (Model::DRAWGROUP &InDrawGroup) {
        if (InDrawGroup.material)
            InDrawGroup.materialID = InDrawGroup.material->GetID();
        *this << InDrawGroup.materialID;
        *this << (SerializableResource*)&InDrawGroup.indices;
        return *this;
    }

    template<> Archive& Archive::operator << (Model::MESH &InMesh) {
        *this << InMesh.pos;
        *this << InMesh.scl;
        *this << InMesh.rot;
        *this << InMesh.worldMatrix;

        *this << (SerializableResource*)InMesh.vertexbuffer;

        uint32 NumDrawGroups = InMesh.drawgroups.count();
        *this << NumDrawGroups;
        if (IsLoading()) {
            InMesh.drawgroups.Reserve(NumDrawGroups);
            for (uint32 Index = 0; Index < NumDrawGroups; Index++) {
                Model::DRAWGROUP *newDrawGroup = new Model::DRAWGROUP();
                *this << *newDrawGroup;
                InMesh.drawgroups.Add(newDrawGroup);
            }
        }
        else {
            for (uint32 Index = 0; Index < NumDrawGroups; Index++) {
                *this << *InMesh.drawgroups[Index];
            }
        }
        return *this;
    }

    void Model::_DRAWGROUP::InitResource()
    {
        indexBuffer = new IndexBuffer();
        indexBuffer->Create(indices.count() * 3, &indices[0]);
    }

    void Model::_MESH::InitResource()
    {
        for (uint32 Index = 0; Index < drawgroups.count(); Index++) {
            drawgroups[Index]->InitResource();
        }
        vertexbuffer->InitResource();
    }

    Model::Model()
    : mBoundingbox()
    , mMaterials()
    , mBaseMatrix(LEMath::FloatMatrix4x4::Identity)
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
    , mBaseMatrix(LEMath::FloatMatrix4x4::Identity)
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
        for (uint32 i=0;i<mMeshes.count();i++) {
            if(mMeshes[i])
                delete mMeshes[i];
        }
        mMeshes.Clear();
        
        for(uint32 i=0;i<mMaterials.count();i++) {
            if (mMaterials[i])
                delete mMaterials[i];
        }
        mMaterials.Clear();
    }
    void Model::InitResource()
    {
        setupMaterialShaderParameters();
        for (uint32 Index = 0; Index < mMeshes.count(); Index++) {
            mMeshes[Index]->InitResource();
            for (uint32 DGIdx = 0; DGIdx < mMeshes[Index]->drawgroups.count(); DGIdx++) {
                for (uint32 MatIdx=0;MatIdx< mMaterials.count();MatIdx++) {
                    if (mMaterials[MatIdx]->GetID() == mMeshes[Index]->drawgroups[DGIdx]->materialID) {
                        mMeshes[Index]->drawgroups[DGIdx]->material = mMaterials[MatIdx];
                        break;
                    }
                }
            }
        }
    }
    Model* Model::GenerateFromTextParser(const ReferenceCountedPointer<TextParser> &Parser)
    {
        if (!Parser.IsValid()) return nullptr;
        Model *output = new Model();
        output->Load(Parser->GetNode("DATA"));
        return output;
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
    Model* Model::Load(TextParser::NODE *root)
    {
        if (!root) return nullptr;
        TextParser::NODE *node = NULL;
        if ((node = root->FindChild("MATERIALS")))
        {
            for (uint32 i=0;i<node->children.count();i++) {
                Material *material = new Material();
                material->Load(node);
                mMaterials.Add(material);
            }
        }
        if ((node = root->FindChild("TRANSFORM")))
        {
            for (uint32 i=0;i<node->children.size();i++) {
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
            mBaseMatrix = LEMath::FloatMatrix4x4::GenerateTransform((LEMath::FloatVector4)mBasePosition) * LEMath::FloatMatrix4x4::GenerateRotationXYZ((LEMath::FloatVector4)mBaseRotation) * LEMath::FloatMatrix4x4::GenerateScaling((LEMath::FloatVector4)mBaseScale);
        }
        if ((node = root->FindChild("ELEMENTS")))
        {
            for (uint32 j=0;j<node->children.count();j++) {
                if (TextParser::NODE *eleNode = node->children[j]) {
                    if (eleNode->name == "MESH") {
                        MESH *mesh = new MESH();
                        mMeshes.push_back(mesh);
                        TextParser::NODE *tn = NULL;
                        if ((tn = eleNode->FindChild("TRANSFORM")))
                        {
                            for (uint32 i=0;i<tn->children.size();i++) {
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
                            for (uint32 i=0;i<verticesNode->children.count();i++) {
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
                            for(uint32 i=0;i<indicesNode->children.count();i++) {
                                TextParser::NODE *idxNode = indicesNode->children[i];
                                DRAWGROUP *drawgroup = NULL;
                                if (TextParser::NODE *materialNode = idxNode->FindChild("MATERIAL")) {
                                    String matname = materialNode->values[0];
                                    for(uint32 l=0;l<mesh->drawgroups.count();l++)
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
                                        for(uint32 l=0;l<mMaterials.count();l++)
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
                    }
                }
            }
        }
        // Postprocess
        calcTangentBinormal();
        setupMaterialShaderParameters();

        return this;
    }
    Model* Model::GenerateFromXML(const rapidxml::xml_document<const char> *XMLDoc)
    {
        return (new Model())->Load(XMLDoc->first_node("model"));
    }
    Model* Model::Load(const rapidxml::xml_node<const char> *XMLNode)
    {
        static constexpr uint32 MaterialReserveUnit = 0xff;
        static constexpr uint32 MeshReserveUnit = 0xf;
        static constexpr uint32 MeshVerticesUnit = 0xffff;
        static constexpr uint32 MeshIndicesUnit = 0xffff;

        if (!XMLNode) return nullptr;

        mMaterials.Clear();
        mMaterials.Reserve(MaterialReserveUnit);
        mMeshes.Clear();
        mMeshes.Reserve(MeshReserveUnit);
        if (rapidxml::xml_node<const char> *materialsNode = XMLNode->first_node("materials")) {
            for (rapidxml::xml_node<const char> *materialNode = materialsNode->first_node(); materialNode; materialNode = materialNode->next_sibling()) {
                Material *material = new Material();
                material->Load(materialNode);
                mMaterials.Add(material);
            }
        }
        if (rapidxml::xml_node<const char> *elementsNode = XMLNode->first_node("elements")) {
            for (rapidxml::xml_node<const char> *meshNode = elementsNode->first_node(); meshNode; meshNode = meshNode->next_sibling()) {
                MESH *mesh = new MESH();
                mMeshes.push_back(mesh);
                if (rapidxml::xml_node<const char> *verticesNode = meshNode->first_node("vertices")) {
                    mesh->vertexbuffer = new RigidVertexBuffer();
                    RigidVertexBuffer *vtxbuf = (VertexBuffer<FVF_PNCTTB, SIZE_PNCTTB>*)mesh->vertexbuffer;
                    VectorArray<RigidVertex> RigidVertices;
                    RigidVertices.Reserve(MeshVerticesUnit);
                    for (rapidxml::xml_node<const char> *vertexNode = verticesNode->first_node(); vertexNode; vertexNode = vertexNode->next_sibling()) {
                        RigidVertex &newVertex = RigidVertices.Add();
                        if (rapidxml::xml_node<const char> *positionNode = vertexNode->first_node("position")) {
                            LEMath::FloatVector3 position;
                            if (rapidxml::xml_node<const char> *nodeX = positionNode->first_node("x")) {
                                position.SetX((float)atof(nodeX->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeY = positionNode->first_node("y")) {
                                position.SetY((float)atof(nodeY->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeZ = positionNode->first_node("z")) {
                                position.SetZ((float)atof(nodeZ->value()));
                            }
                            newVertex.SetPosition(position);
                        }
                        if (rapidxml::xml_node<const char> *normalNode = vertexNode->first_node("normal")) {
                            LEMath::FloatVector3 normal;
                            if (rapidxml::xml_node<const char> *nodeX = normalNode->first_node("x")) {
                                normal.SetX((float)atof(nodeX->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeY = normalNode->first_node("y")) {
                                normal.SetY((float)atof(nodeY->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeZ = normalNode->first_node("z")) {
                                normal.SetZ((float)atof(nodeZ->value()));
                            }
                            newVertex.SetNormal(normal);
                        }
                        if (rapidxml::xml_node<const char> *binormalNode = vertexNode->first_node("binormal")) {
                            LEMath::FloatVector3 binormal;
                            if (rapidxml::xml_node<const char> *nodeX = binormalNode->first_node("x")) {
                                binormal.SetX((float)atof(nodeX->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeY = binormalNode->first_node("y")) {
                                binormal.SetY((float)atof(nodeY->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeZ = binormalNode->first_node("z")) {
                                binormal.SetZ((float)atof(nodeZ->value()));
                            }
                            newVertex.SetBinormal(binormal);
                        }
                        if (rapidxml::xml_node<const char> *tangentNode = vertexNode->first_node("tangent")) {
                            LEMath::FloatVector3 tangent;
                            if (rapidxml::xml_node<const char> *nodeX = tangentNode->first_node("x")) {
                                tangent.SetX((float)atof(nodeX->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeY = tangentNode->first_node("y")) {
                                tangent.SetY((float)atof(nodeY->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeZ = tangentNode->first_node("z")) {
                                tangent.SetZ((float)atof(nodeZ->value()));
                            }
                            newVertex.SetTangent(tangent);
                        }
                        if (rapidxml::xml_node<const char> *texcoordNode = vertexNode->first_node("texcoord")) {
                            LEMath::FloatVector2 texcoord;
                            if (rapidxml::xml_node<const char> *nodeX = texcoordNode->first_node("x")) {
                                texcoord.SetX((float)atof(nodeX->value()));
                            }
                            if (rapidxml::xml_node<const char> *nodeY = texcoordNode->first_node("y")) {
                                texcoord.SetY((float)atof(nodeY->value()));
                            }
                            newVertex.SetTexcoord(texcoord);
                        }
                        if (RigidVertices.size() % MeshVerticesUnit == 0) {
                            RigidVertices.Reserve(RigidVertices.size() + MeshVerticesUnit);
                        }
                    }
                    vtxbuf->Create(RigidVertices.count(), RigidVertices.GetData(), 0);
                }
                if (rapidxml::xml_node<const char> *indicesNode = meshNode->first_node("indices")) {
                    DRAWGROUP *drawgroup = NULL;
                    for (rapidxml::xml_node<const char> *indexNode = indicesNode->first_node(); indexNode; indexNode = indexNode->next_sibling()) {
                        if (rapidxml::xml_node<const char>* materialNode = indexNode->first_node("material")) {
                            const char *materialName = materialNode->value();
                            if (!drawgroup || !drawgroup->material || drawgroup->material->GetID() != materialName) {
                                // Find drawgroup
                                drawgroup = nullptr;
                                for (uint32 l = 0; l < mesh->drawgroups.count(); l++) {
                                    if (mesh->drawgroups[l]->material->GetID() == materialName) {
                                        drawgroup = mesh->drawgroups[l];
                                        break;
                                    }
                                }
                            }
                            if (!drawgroup) {
                                Material *material = NULL;
                                for (uint32 l = 0; l < mMaterials.count(); l++)
                                {
                                    if (mMaterials[l]->GetID() == materialName)
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
                        if (drawgroup)
                        if (rapidxml::xml_node<const char>* polygonNode = indexNode->first_node("polygon")) {
                            LEMath::IntVector3 polygonIndex;
                            if (rapidxml::xml_node<const char>* node0 = polygonNode->first_node("i0")) {
                                polygonIndex.SetX(atoi(node0->value()));
                            }
                            if (rapidxml::xml_node<const char>* node1 = polygonNode->first_node("i1")) {
                                polygonIndex.SetY(atoi(node1->value()));
                            }
                            if (rapidxml::xml_node<const char>* node2 = polygonNode->first_node("i2")) {
                                polygonIndex.SetZ(atoi(node2->value()));
                            }
                            drawgroup->indices.push_back(polygonIndex);
                        }
                    }
                }
            }
        }
        
        // Postprocess
        calcTangentBinormal();
        setupMaterialShaderParameters();

        return this;
    }
    bool Model::Serialize(Archive &Ar)
    {
        Ar << mName;
        Ar << mBoundingbox;
        Ar << mBasePosition;
        Ar << mBaseScale;
        Ar << mBaseRotation;

        uint32 NumMaterials = mMaterials.count();
        Ar << NumMaterials;
        if (Ar.IsLoading()) {
            for (uint32 Index = 0; Index < NumMaterials; Index++) {
                Material *newMaterial = new Material();
                Ar << *newMaterial;
                mMaterials.Add(newMaterial);
            }
        }
        else {
            for (uint32 Index = 0; Index < NumMaterials; Index++) {
                Ar << *mMaterials[Index];
            }
        }

        uint32 NumMeshes = mMeshes.count();
        Ar << NumMeshes;
        if (Ar.IsLoading()) {
            mMeshes.Reserve(NumMeshes);
            for (uint32 Index = 0; Index < NumMeshes; Index++) {
                MESH *newMesh = new MESH();
                newMesh->vertexbuffer = new RigidVertexBuffer();
                Ar << *newMesh;
                mMeshes.Add(newMesh);
            }
        }
        else {
            for (uint32 Index = 0; Index < NumMeshes; Index++) {
                Ar << *mMeshes[Index];
            }
        }

        return true;
    }
    void Model::Draw(const RenderState &rs, const LEMath::FloatMatrix4x4 &Transform)
    {
        //DrawCommand::SetCulling(static_cast<uint32>(RendererFlag::Culling::ClockWise));
        DrawCommand::BeginDrawing();
        // Calculate Matrix
        LEMath::FloatMatrix4x4 modelTransformMatrix = Transform * getTransformMatrix();
        LEMath::FloatMatrix4x4 modelWvpMat = modelTransformMatrix * LEMath::FloatMatrix4x4(rs.GetViewProjMatrix());

        for (uint32 i=0;i<mMeshes.size();i++)
        {
            MESH *mesh = mMeshes[i];
            for(uint32 j=0;j<mesh->drawgroups.count();j++)
            {
                DRAWGROUP *drawGroup = mesh->drawgroups[j];
                // Set material
                if (Material *material = drawGroup->material)
                {
                    if (!material->IsEnabledRenderPass(rs.GetRenderPass())) continue;

                    // Set RenderState
                    RenderState rsCopied(rs);
                    rsCopied.SetWorldMatrix(/*mesh->worldMatrix * */modelTransformMatrix);
                    rsCopied.SetWorldViewProjMatrix(/*mesh->worldMatrix * */modelWvpMat);
                    //LE_LightManager.ApplyLight(rsCopied, NULL);

                    // Bind material
                    material->Bind(rsCopied);
                }
                // Draw
                DrawCommand::BindVertexBuffer(mesh->vertexbuffer);
                DrawCommand::BindIndexBuffer(drawGroup->indexBuffer);
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
        LEMath::FloatMatrix4x4 transMatrix = LEMath::FloatMatrix4x4::GenerateTransform((LEMath::FloatVector4)mPosition).Transpose();
        LEMath::FloatMatrix4x4 rotXMatrix = LEMath::FloatMatrix4x4::GenerateRotationX(mRotation.X());
        LEMath::FloatMatrix4x4 rotYMatrix = LEMath::FloatMatrix4x4::GenerateRotationY(mRotation.Y());
        LEMath::FloatMatrix4x4 rotZMatrix = LEMath::FloatMatrix4x4::GenerateRotationZ(mRotation.Z());
        LEMath::FloatMatrix4x4 sclMatrix = LEMath::FloatMatrix4x4::GenerateScaling((LEMath::FloatVector4)mScale);
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
        for (uint32 i=0;i<mMeshes.size();i++)
        {
            RigidVertex *vtxptr = (RigidVertex*)((RigidVertexBuffer *)mMeshes[i]->vertexbuffer)->GetBuffer();
            if (vtxptr == NULL || vtxptr[0].GetBinormal() != LEMath::FloatVector3::Zero && vtxptr[0].GetTangent() != LEMath::FloatVector3::Zero)
                continue;
            for (uint32 l=0;l<mMeshes[i]->drawgroups.size();l++)
            {
                for (uint32 m=0;m<mMeshes[i]->drawgroups[l]->indices.size();m++)
                {
                    int idx1 = mMeshes[i]->drawgroups[l]->indices[m].X();
                    int idx2 = mMeshes[i]->drawgroups[l]->indices[m].Y();
                    int idx3 = mMeshes[i]->drawgroups[l]->indices[m].Z();
                    LEMath::FloatVector3 n1 = vtxptr[idx1].GetNormal();
                    LEMath::FloatVector3 n2 = vtxptr[idx2].GetNormal();
                    LEMath::FloatVector3 n3 = vtxptr[idx3].GetNormal();
                    vtxptr[idx1].SetTangent(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
                    vtxptr[idx2].SetTangent(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
                    vtxptr[idx3].SetTangent(LEMath::FloatVector3(1.0f, 0.0f, 0.0f));
                    vtxptr[idx1].SetBinormal(LEMath::FloatVector3(0.0f, 0.0f, 1.0f));
                    vtxptr[idx2].SetBinormal(LEMath::FloatVector3(0.0f, 0.0f, 1.0f));
                    vtxptr[idx3].SetBinormal(LEMath::FloatVector3(0.0f, 0.0f, 1.0f));
/*                    LEMath::FloatVector3 p1 = vtxptr[idx1].GetPosition();
                    LEMath::FloatVector3 p2 = vtxptr[idx2].GetPosition();
                    LEMath::FloatVector3 p3 = vtxptr[idx3].GetPosition();
                    LEMath::FloatVector2 t1 = vtxptr[idx1].GetTexcoord();
                    LEMath::FloatVector2 t2 = vtxptr[idx2].GetTexcoord();
                    LEMath::FloatVector2 t3 = vtxptr[idx3].GetTexcoord();
                    LEMath::FloatVector3 p21 = p2 - p1;
                    LEMath::FloatVector3 p31 = p3 - p1;
                    LEMath::FloatVector2 t21 = t2 - t1;
                    LEMath::FloatVector2 t31 = t3 - t1;
                    
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
*/
                }
            }
        }
    }
}