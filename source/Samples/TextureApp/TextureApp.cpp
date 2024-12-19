/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "TextureApp.h"


#define UVN_CAMERA
#define PERSPECTIVE_CAMERA

#if (T3D_COORDINATION_RH)
#define USE_COORDINATION_RH
#endif

using namespace Tiny3D;

const char *SUB_MESH_NAME = "#0";


TextureApp theApp;

extern const char *SAMPLE_VERTEX_SHADER;
extern const char *SAMPLE_PIXEL_SHADER;

TextureApp::TextureApp()
{
}

TextureApp::~TextureApp()
{
}

bool TextureApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);
    
    // root game object
    GameObjectPtr go = GameObject::create("TestScene");
    // scene->addRootGameObject(go);
    Transform3DPtr root = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);
    
    // camera
    buildCamera(root);

    // cube
    buildCube(root);
    
    return true;
}

void TextureApp::buildCamera(Transform3D *parent)
{
    // render window for render target in camera
    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    // transform node for camera
    GameObjectPtr go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);
    
    // camera component
    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB::GRAY);
    camera->setRenderTarget(rt);
    Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    
    // camera for perspective
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);
    
    // construct camera position & orientation & scaling
    Vector3 eye(2.0f, 2.0f, -4.0f);

    Vector3 obj(0.0f, 0.0f, 0.0f);
    camera->lookAt(eye, obj, Vector3::UP);

    // construct frustum bound
    auto frustum = go->addComponent<FrustumBound>();
    T3D_ASSERT(frustum != nullptr);
}

void TextureApp::buildCube(Transform3D *parent)
{
    // transform node for cube
    GameObjectPtr go = GameObject::create("Cube");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);

    // material
    MaterialPtr material = buildMaterial();
    
    // mesh & submesh
    MeshPtr mesh = buildMesh(material->getUUID());
    SubMesh *submesh = mesh->getSubMesh(SUB_MESH_NAME);
    
    // geometry component
    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mesh, submesh);
    
    // aabb bound component
    AabbBoundPtr bound = go->addComponent<AabbBound>();
    buildAabb(mesh, submesh, bound);
}

Texture2DPtr TextureApp::buildTexture()
{
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_FS, Dir::getAppPath(), Archive::AccessMode::kRead);
    T3D_ASSERT(archive != nullptr);
    ImagePtr image = T3D_IMAGE_MGR.loadImage(archive, "Assets/samples/textures/blocks.png");
    T3D_ASSERT(image != nullptr);
    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("textureCube", image);
    return texture;
}


MaterialPtr TextureApp::buildMaterial()
{
    TResult ret;
    
    // vertex & pixel shader keyword
    ShaderKeyword vkeyword;
    vkeyword.addKeyword("");
    vkeyword.generate();
    ShaderKeyword pkeyword(vkeyword);
    
    // vertex shader
    const String vs = SAMPLE_VERTEX_SHADER;
    
    ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vs);
    vshader->setShaderStage(SHADER_STAGE::kVertex);

    // pixel shader
    const String ps = SAMPLE_PIXEL_SHADER;
    
    ShaderVariantPtr pshader = ShaderVariant::create(std::move(pkeyword), ps);
    pshader->setShaderStage(SHADER_STAGE::kPixel);

    // render state
    RenderStatePtr renderState = RenderState::create();

    // blend state
    BlendDesc blendDesc;
    renderState->setBlendDesc(blendDesc);

    // depth & stencil state
    DepthStencilDesc depthStencilDesc;
    renderState->setDepthStencilDesc(depthStencilDesc);

    // rasterizer state
    RasterizerDesc rasterizeDesc;
    renderState->setRasterizerDesc(rasterizeDesc);
    
    // pass
    PassPtr pass = Pass::create("0");
    ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    ret = pass->addShaderVariant(pshader->getShaderKeyword(), pshader);
    pass->setRenderState(renderState);
    T3D_ASSERT(T3D_SUCCEEDED(ret));

    // technique
    TechniquePtr tech = Technique::create("Default-Technique");
    bool rval = tech->addPass(pass);
    T3D_ASSERT(rval);

    // shader
    ShaderPtr shader = T3D_SHADER_MGR.createShader("Default-Shader");
    rval = shader->addTechnique(tech);
    T3D_ASSERT(rval);
    
    // samplers
    ShaderSamplerParams samplers;
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildTexture();
    // sampler state
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    // ShaderSamplerParamPtr sampler = ShaderSamplerParam::create(texSamplerName, TEXTURE_TYPE::TT_2D, texture);
    // shader->addSamplerParam(sampler);
    
    // material
    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Default-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());
    
    return material;
}


MeshPtr TextureApp::buildMesh(const Tiny3D::UUID &materialUUID)
{
    // 
    // 正方体顶点定义如下：
    //
    //           v6-------v4
    //          /|       /|
    //         / |      / |
    //        v0-------v2 |
    //        |  v7----|--v5
    //        | /      | /
    //        |/       |/
    //        v1-------v3
    //

    struct BoxVertex
    {
        Vector3 position {};
        Vector2 uv {};
    };
    
    // vertex attributes
    VertexAttribute attrPos(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
    VertexAttribute attrUV(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
    VertexAttributes attributes(2);
    attributes[0] = attrPos;
    attributes[1] = attrUV;

    // vertices & indices
    Vector3 offset;
    Vector3 center(0.0f, 0.0f, 0.0f);
    Vector3 extent(0.5f, 0.5f, 0.5f);

    const uint32_t kVertexCount = 24;
    const uint32_t kIndexCount = 36;
    BoxVertex *vertices = new BoxVertex[kVertexCount];
    uint16_t *indices = new uint16_t[kIndexCount];

    // front - V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[0].position = center + offset;
    vertices[0].uv = Vector2(0.0f, 0.0f);
    
    // front - V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[1].position = center + offset;
    vertices[1].uv = Vector2(0.0f, 0.5f);
    
    // front - V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[2].position = center + offset;
    vertices[2].uv = Vector2(0.5f, 0.0f);
    
    // front - V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[3].position = center + offset;
    vertices[3].uv = Vector2(0.5f, 0.5f);

    // right - V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[4].position = center + offset;
    vertices[4].uv = Vector2(0.0f, 0.5f);
    
    // right - V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[5].position = center + offset;
    vertices[5].uv = Vector2(0.0f, 1.0f);
    
    // right - V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[6].position = center + offset;
    vertices[6].uv = Vector2(0.5f, 0.5f);
    
    // right - V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[7].position = center + offset;
    vertices[7].uv = Vector2(0.5f, 1.0f);

    // back - V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[8].position = center + offset;
    vertices[8].uv = Vector2(0.0f, 0.0f);
    
    // back - V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[9].position = center + offset;
    vertices[9].uv = Vector2(0.0f, 0.5f);

    // back - V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[10].position = center + offset;
    vertices[10].uv = Vector2(0.5f, 0.0f);
    
    // back - V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[11].position = center + offset;
    vertices[11].uv = Vector2(0.5f, 0.5f);
    
    // left - V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[12].position = center + offset;
    vertices[12].uv = Vector2(0.5f, 0.5f);
    
    // left - V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[13].position = center + offset;
    vertices[13].uv = Vector2(0.5f, 1.0f);

    // left - V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[14].position = center + offset;
    vertices[14].uv = Vector2(1.0f, 0.5f);
    
    // left - V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[15].position = center + offset;
    vertices[15].uv = Vector2(1.0f, 1.0f);

    // top - V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[16].position = center + offset;
    vertices[16].uv = Vector2(0.5f, 0.5f);
    
    // top - V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[17].position = center + offset;
    vertices[17].uv = Vector2(1.0f, 0.5f);

    // top - V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[18].position = center + offset;
    vertices[18].uv = Vector2(1.0f, 0.0f);

    // top - V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[19].position = center + offset;
    vertices[19].uv = Vector2(0.5f, 0.0f);

    // bottom - V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[20].position = center + offset;
    vertices[20].uv = Vector2(0.5f, 0.5f);

    // bottom - V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[21].position = center + offset;
    vertices[21].uv = Vector2(0.5f, 1.0f);

    // bottom - V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[22].position = center + offset;
    vertices[22].uv = Vector2(1.0f, 0.5f);
    
    // bottom - V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[23].position = center + offset;
    vertices[23].uv = Vector2(1.0f, 1.0f);
    
    // Front face
    indices[0] = 0; indices[1] = 2; indices[2] = 1;
    indices[3] = 1; indices[4] = 2; indices[5] = 3;
    
    // Back
    indices[6] = 4; indices[7] = 6; indices[8] = 5;
    indices[9] = 5; indices[10] = 6; indices[11] = 7;
    
    // Left
    indices[12] = 8; indices[13] = 10; indices[14] = 9;
    indices[15] = 9; indices[16] = 10; indices[17] = 11;
    
    // Right
    indices[18] = 12; indices[19] = 14; indices[20] = 13;
    indices[21] = 13; indices[22] = 14; indices[23] = 15;
    
    // Top
    indices[24] = 16; indices[25] = 19; indices[26] = 18;
    indices[27] = 18; indices[28] = 17; indices[29] = 16;
    
    // Bottom
    indices[30] = 20; indices[31] = 22; indices[32] = 21;
    indices[33] = 21; indices[34] = 22; indices[35] = 23;
    
    // construct mesh resource
    Buffer vertexBuffer;
    vertexBuffer.Data = (uint8_t*)vertices;
    vertexBuffer.DataSize = sizeof(BoxVertex) * kVertexCount;
    Vertices vertexBuffers(1);
    vertexBuffers[0] = vertexBuffer;
    
    VertexStrides strides(1, sizeof(BoxVertex));
    VertexOffsets offsets(1, 0);
    
    Buffer indexBuffer;
    indexBuffer.Data = (uint8_t*)indices;
    indexBuffer.DataSize = sizeof(uint16_t) * kIndexCount;
    
    String name = SUB_MESH_NAME;
    SubMeshPtr submesh = SubMesh::create(name, materialUUID, PrimitiveType::kTriangleList, indexBuffer, true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}

void TextureApp::buildAabb(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
{
    const VertexAttribute *attr = mesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_POSITION, 0);
    size_t vertexSize = mesh->getVertexStride(attr->getSlot());
    size_t offset = mesh->getVertexOffset(attr->getOffset());
    const Buffer &vertexBuffer = mesh->getVertices()[attr->getSlot()];
    const Buffer &indexBuffer = submesh->getIndices();
    size_t indexSize = submesh->getIndexBuffer()->getIndexSize();
    size_t pointCount = submesh->getIndexBuffer()->getIndexCount();
    Vector3 *points = new Vector3[pointCount];
    for (size_t i = 0; i < pointCount; ++i)
    {
        int32_t idx = 0;
        const uint8_t *src = indexBuffer.Data + i * indexSize;
        memcpy(&idx, src, indexSize);
        src = vertexBuffer.Data + idx * vertexSize + offset;
        memcpy(points+i, src, sizeof(Vector3));
        // Vector3 *srcPos = (Vector3*)src;
        // T3D_LOG_INFO(LOG_TAG_APP, "Index = %d, Src : (%f, %f, %f), Dst : (%f, %f, %f)", idx, srcPos->x(), srcPos->y(), srcPos->z(), points[i].x(), points[i].y(), points[i].z());
    }
    Aabb aabb;
    aabb.build(points, pointCount);
    T3D_SAFE_DELETE_ARRAY(points);
    bound->setParams(aabb.getMinX(), aabb.getMaxX(), aabb.getMinY(), aabb.getMaxY(), aabb.getMinZ(), aabb.getMaxZ());
}
