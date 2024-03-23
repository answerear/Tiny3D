/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "GeometryApp.h"


using namespace Tiny3D;

const char *SUB_MESH_NAME = "#0";

GeometryApp theApp;


GeometryApp::GeometryApp()
{
}

GeometryApp::~GeometryApp()
{
}

bool GeometryApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
    T3D_SCENE_MGR.setCurrentScene(scene);
    
    // root game object
    GameObjectPtr go = GameObject::create("TestScene");
    scene->addRootGameObject(go);
    Transform3DPtr root = go->addComponent<Transform3D>();

    // render window for render target in camera
    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    // transform node for camera
    go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    root->addChild(xform);
    // camera component
    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB::GRAY);
    camera->setRenderTarget(rt);
    scene->addCamera(camera);

    // transform node for cube
    go = GameObject::create("Cube");
    xform = go->addComponent<Transform3D>();
    root->addChild(xform);
    // geometry component
    GeometryPtr geometry = go->addComponent<Geometry>();
    MeshPtr mesh = buildMesh();
    geometry->setMeshObject(mesh, mesh->getSubMesh(SUB_MESH_NAME));

    return true;
}

void GeometryApp::applicationWillTerminate() 
{
    
}

Texture2DPtr GeometryApp::buildTexture()
{
    const uint32_t width = 256;
    const uint32_t height = 256;
    uint32_t pitch = Image::calcPitch(width, 24);
    const uint32_t dataSize = pitch * height;
    uint8_t *pixels = new uint8_t[dataSize];
    
    for (uint32_t y = 0; y < height; ++y)
    {
        uint8_t *lines = pixels + pitch * y;
        uint32_t i = 0;
        for (uint32_t x = 0; x < width; ++x)
        {
            // blue
            lines[i++] = 39;
            // green
            lines[i++] = 127;
            // red
            lines[i++] = 255;
        }
    }
    
    Buffer texData;
    texData.Data = pixels;
    texData.DataSize = dataSize;
    
    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("textureCube", width, height, PixelFormat::E_PF_B8G8R8, texData, 1, 0, 0);
    
    return texture;
}


MaterialPtr GeometryApp::buildMaterial()
{
    TResult ret;
    
    // vertex & pixel shader keyword
    ShaderKeyword vkeyword;
    vkeyword.addKeyword("-");
    vkeyword.generate();
    ShaderKeyword pkeyword(vkeyword);
    
    // vertex shader
    const String vs =
        "cbuffer ConstantBuffer : register(b0)\n"
        "{\n"
        "   float4x4 modelMatrix;\n"
        "   float4x4 viewMatrix;\n"
        "   float4x4 projectionMatrix;\n"
        "}\n"
        "struct VertexInput\n"
        "{\n"
        "   float3 position : POSITION;\n"
        "   float2 uv : TEXCOORD0;\n"
        "};\n"
        "struct VertexOutput\n"
        "{\n"
        "   float4 position : SV_POSITION;\n"
        "   float2 uv : TEXCOORD0;\n"
        "};\n"
        "VertexOutput main(VertexInput input)\n"
        "{\n"
        "   VertexOutput output;\n"
        "   float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);\n"
        "   float4 viewPosition = mul(worldPosition, viewMatrix);\n"
        "   float4 clipPosition = mul(viewPosition, projectionMatrix);\n"
        "   output.position = clipPosition;\n"
        "   output.uv = input.uv;\n"
        "   return output;\n"
        "}\n";
    
    ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vs);
    vshader->setShaderStage(SHADER_STAGE::kVertex);

    // pixel shader
    const String ps =
        "Texture2D texCube : register(t0);\n"
        "SamplerState sampler_texCube : register(s0);\n"
        "struct PS_INPUT\n"
        "{\n"
        "    float4 Pos : SV_POSITION;\n"
        "    float2 Tex : TEXCOORD0;\n"
        "};\n"
        "float4 main(PS_INPUT input) : SV_Target\n"
        "{\n"
        "    float4 color = texCube.Sample(sampler_texCube, input.Tex);\n"
        "    return color;\n"
        "}";
    
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
    T3D_ASSERT(T3D_SUCCEEDED(ret), "Add vertex shader variant !");
    ret = pass->addShaderVariant(pshader->getShaderKeyword(), pshader);
    pass->setRenderState(renderState);
    T3D_ASSERT(T3D_SUCCEEDED(ret), "Add pixel shader variant !");

    // technique
    TechniquePtr tech = Technique::create("Default-Technique");
    bool rval = tech->addPass(pass);
    T3D_ASSERT(rval, "Add pass !");

    // shader
    ShaderPtr shader = T3D_SHADER_MGR.createShader("Default-Shader");
    rval = shader->addTechnique(tech);
    T3D_ASSERT(rval, "Add technique !");

    // constants
    ShaderConstantParams constants;

    // model matrix
    Matrix4 modelMatrix;
    const String modelMatrixName = "modelMatrix";
    ShaderConstantParamPtr matrixParam = ShaderConstantParam::create(modelMatrixName, &modelMatrix, sizeof(modelMatrix), ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
    shader->addConstantParam(matrixParam);
    
    // view matrix
    Matrix4 viewMatrix;
    const String viewMatrixName = "viewMatrix";
    matrixParam = ShaderConstantParam::create(viewMatrixName, &viewMatrix, sizeof(viewMatrix), ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
    shader->addConstantParam(matrixParam);
    
    // projection matrix
    Matrix4 projMatrix;
    const String projMatrixName = "projectionMatrix";
    matrixParam = ShaderConstantParam::create(projMatrixName, &projMatrix, sizeof(projMatrix), ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
    shader->addConstantParam(matrixParam);
    
    // samplers
    ShaderSamplerParams samplers;
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildTexture();
    // sampler state
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    ShaderSamplerParamPtr sampler = ShaderSamplerParam::create(texSamplerName, TEXTURE_TYPE::TT_2D, texture);
    shader->addSamplerParam(sampler);
    
    // material
    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Default-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("-");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    
    return material;
}


MeshPtr GeometryApp::buildMesh()
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
    Vector2 uv(0.0f, 0.0f);
    Vector3 center(0.0f, 0.0f, 0.0f);
    Vector3 extent(0.5f, 0.5f, 0.5f);
    BoxVertex *vertices = new BoxVertex[8];
    uint16_t *indices = new uint16_t[36];
    
    // V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[0].position = center + offset;
    vertices[0].uv = uv;

    // V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[1].position = center + offset;
    vertices[1].uv = uv;

    // V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[2].position = center + offset;
    vertices[2].uv = uv;

    // V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[3].position = center + offset;
    vertices[3].uv = uv;

    // V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[4].position = center + offset;
    vertices[4].uv = uv;

    // V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[5].position = center + offset;
    vertices[5].uv = uv;

    // V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[6].position = center + offset;
    vertices[6].uv = uv;

    // V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[7].position = center + offset;
    vertices[7].uv = uv;

    // Front face
    indices[0] = 0, indices[1] = 1, indices[2] = 2;
    indices[3] = 1, indices[4] = 3, indices[5] = 2;

    // Back
    indices[6] = 4, indices[7] = 5, indices[8] = 6;
    indices[9] = 5, indices[10] = 7, indices[11] = 6;

    // Left
    indices[12] = 0, indices[13] = 6, indices[14] = 7;
    indices[15] = 0, indices[16] = 7, indices[17] = 1;

    // Right
    indices[18] = 2, indices[19] = 3, indices[20] = 4;
    indices[21] = 3, indices[22] = 5, indices[23] = 4;

    // Top
    indices[24] = 0, indices[25] = 2, indices[26] = 4;
    indices[27] = 0, indices[28] = 4, indices[29] = 6;

    // Bottom
    indices[30] = 1, indices[31] = 7, indices[32] = 3;
    indices[33] = 3, indices[34] = 7, indices[35] = 5;
    

    // construct mesh resource
    Buffer vertexBuffer;
    vertexBuffer.Data = (uint8_t*)vertices;
    vertexBuffer.DataSize = sizeof(vertices);
    Vertices vertexBuffers(1);
    vertexBuffers[0] = vertexBuffer;
    
    VertexStrides strides(1, sizeof(BoxVertex));
    VertexOffsets offsets(1, 0);
    
    Buffer indexBuffer;
    indexBuffer.Data = (uint8_t*)indices;
    indexBuffer.DataSize = sizeof(indices);
    
    String name = SUB_MESH_NAME;
    MaterialPtr material = buildMaterial();
    SubMeshPtr submesh = SubMesh::create(name, material, PrimitiveType::kTriangleList, indexBuffer, true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}


