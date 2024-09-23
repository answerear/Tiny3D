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


#include "EditorScene.h"
#include "ProjectManager.h"


#define UVN_CAMERA
#define PERSPECTIVE_CAMERA
#define ASSETS_FROM_FILE

const char *SUB_MESH_NAME = "#0";


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    EditorScene::~EditorScene()
    {
        mSceneRT = nullptr;
        if (mSceneTarget != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mSceneTarget->getRenderTexture());
        }
        mSceneTarget = nullptr;
        
        T3D_SCENE_MGR.unloadScene();
    }

    //--------------------------------------------------------------------------

    void EditorScene::build()
    {
        // 生成 scene 使用的 render texture
        refreshSceneRenderTarget();
        
        // 创建场景
        ScenePtr scene = T3D_SCENE_MGR.createScene("__Scene__");
        T3D_SCENE_MGR.setCurrentScene(scene);
    
        // 根节点
        GameObjectPtr go = GameObject::create("__SceneRoot__");
        // scene->addRootGameObject(go);
        Transform3DPtr root = go->addComponent<Transform3D>();
        scene->getRootTransform()->addChild(root);

        // 编辑器内部用的根节点
        go = GameObject::create("__Builtin__");
        Transform3DPtr builtinNode = go->addComponent<Transform3D>();
        root->addChild(builtinNode);

        // 游戏用的根节点
        go = GameObject::create("__Game__");
        Transform3DPtr gameNode = go->addComponent<Transform3D>();
        root->addChild(gameNode);

        // 内置的 scene 相机 
        go = GameObject::create("__SceneCamera__");
        Transform3DPtr cameraNode = go->addComponent<Transform3D>();
        builtinNode->addChild(cameraNode);
        CameraPtr camera = go->addComponent<Camera>();
        camera->setOrder(65535);
        Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        camera->setViewport(vp);
        camera->setClearColor(ColorRGB(0.133f, 0.231f, 0.329f));
        camera->setRenderTarget(mSceneTarget);
        scene->addCamera(camera);

        // camera for perspective
        camera->setProjectionType(Camera::Projection::kPerspective);
        camera->setFovY(Radian(Math::PI / 3.0f));
        Real as = mGameRTWidth / mGameRTHeight;// Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
        camera->setAspectRatio(as);
        // camera->setFovY(Radian(Math::PI * 0.5f));
        camera->setNearPlaneDistance(0.1f);
        camera->setFarPlaneDistance(1000.0f);
        
        // construct camera position & orientation & scaling
        Vector3 eye(2.0f, 2.0f, -8.0f);
        Vector3 obj(0.0f, 0.0f, 0.0f);
        camera->lookAt(eye, obj, Vector3::UP);
        
        // construct frustum bound
        auto frustum = go->addComponent<FrustumBound>();
        T3D_ASSERT(frustum != nullptr);
        
        mSceneCamera = camera;

        // For test
        // camera
        // buildCamera(gameNode);

        // cube
        // buildCube(gameNode);
    }

    //--------------------------------------------------------------------------

    void EditorScene::refreshSceneRenderTarget()
    {
        RenderWindow *rw = T3D_AGENT.getDefaultRenderWindow();
        const uint32_t width = static_cast<uint32_t>(rw->getDescriptor().Width);
        const uint32_t height = static_cast<uint32_t>(rw->getDescriptor().Height);
        if (mSceneTarget == nullptr
            || width != mSceneTarget->getRenderTexture()->getWidth()
            || height != mSceneTarget->getRenderTexture()->getHeight())
        {
            if (mSceneTarget != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mSceneTarget->getRenderTexture());
                mSceneTarget = nullptr;
            }
            
            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__SceneRT__", width, height, PixelFormat::E_PF_B8G8R8A8);
            mSceneTarget = RenderTarget::create(renderTex);
            mSceneRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();

            if (mSceneCamera != nullptr)
            {
                mSceneCamera->setRenderTarget(mSceneTarget);
            }
        }
    }

    //--------------------------------------------------------------------------

    void EditorScene::refreshGameRenderTarget(const ImVec2 &size)
    {
        bool rtIsDirty = false;
        // if (mGameTarget == nullptr
        //     || mGameTarget->getRenderTexture()->getWidth() != static_cast<uint32_t>(size.x)
        //     || mGameTarget->getRenderTexture()->getHeight() != static_cast<uint32_t>(size.y))
        // {
        //     // 大小发生变化，重新生成
        //     if (mGameTarget != nullptr)
        //     {
        //         T3D_TEXTURE_MGR.unload(mGameTarget->getRenderTexture());
        //         mGameTarget = nullptr;
        //     }
        //
        //     RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__GameRT__", static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), PixelFormat::E_PF_R8G8B8A8);
        //     mGameTarget = RenderTarget::create(renderTex);
        //     mGameRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();
        //     rtIsDirty = true;
        // }
        if (mGameTarget == nullptr)
        {
            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__GameRT__", static_cast<uint32_t>(mGameRTWidth), static_cast<uint32_t>(mGameRTHeight), PixelFormat::E_PF_B8G8R8A8);
            mGameTarget = RenderTarget::create(renderTex);
            mGameRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();
            rtIsDirty = true;
        }
        
        auto scene = T3D_SCENE_MGR.getCurrentScene();

        if (scene != nullptr)
        {
            // 做个 trick ，把渲染到屏幕的改成渲染到纹理
            if (rtIsDirty)
            {
                for (auto item : scene->getCameras())
                {
                    if (item.second->getGameObject()->getName() != "__SceneCamera__")
                    {
                        // 要重新设置 render target ，并且不是编辑器 scene 视图使用的那个 render target
                        item.second->setRenderTarget(mGameTarget);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    void EditorScene::buildCamera(Transform3D *parent)
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
        camera->setClearColor(ColorRGB(0.133f, 0.231f, 0.329f));
        camera->setRenderTarget(rt);
        Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
        camera->setAspectRatio(as);
        T3D_SCENE_MGR.getCurrentScene()->addCamera(camera);
        
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

    void EditorScene::buildCube(Transform3D *parent)
    {
        // transform node for cube
        GameObjectPtr go = GameObject::create("Cube");
        Transform3DPtr xform = go->addComponent<Transform3D>();
        parent->addChild(xform);
    #if !defined (PERSPECTIVE_CAMERA)
        // xform->setPosition(0.0f, 0.0f, 200.0f);
        // xform->setScaling(100.0f, 100.0f, 100.0f);
    #endif
        
        // geometry component
        GeometryPtr geometry = go->addComponent<Geometry>();
        MeshPtr mesh = buildMesh();
        SubMesh *submesh = mesh->getSubMesh(SUB_MESH_NAME);
        geometry->setMeshObject(mesh, submesh);
        
        // aabb bound component
        AabbBoundPtr bound = go->addComponent<AabbBound>();
        buildAabb(mesh, submesh, bound);
    }

    Texture2DPtr EditorScene::buildTexture()
    {
        const uint32_t width = 64;
        const uint32_t height = 64;
        uint32_t pitch = Image::calcPitch(width, 32);
        const uint32_t dataSize = pitch * height;
        uint8_t *pixels = new uint8_t[dataSize];
        
        for (uint32_t y = 0; y < height; ++y)
        {
            uint8_t *lines = pixels + pitch * y;
            uint32_t i = 0;
            for (uint32_t x = 0; x < width; ++x)
            {
                if (x < 16 && y < 16)
                {
                    // top, blue
                    // B
                    lines[i++] = 196;
                    // G
                    lines[i++] = 114;
                    // R
                    lines[i++] = 68;
                }
                else if (x < 16 && y >= 16 && y < 32)
                {
                    // front, orange
                    // B
                    lines[i++] = 49;
                    // G
                    lines[i++] = 125;
                    // R
                    lines[i++] = 237;
                }
                else if (x >= 16 && x < 32 && y >= 16 && y < 32)
                {
                    // right, green
                    // B
                    lines[i++] = 71;
                    // G
                    lines[i++] = 173;
                    // R
                    lines[i++] = 112;
                }
                else if (x >= 32 && x < 48 && y >= 16 && y < 32)
                {
                    // back, yellow
                    // B
                    lines[i++] = 0;
                    // G
                    lines[i++] = 192;
                    // R
                    lines[i++] = 255;
                }
                else if ( x >= 48 && x < 64 && y >= 16 && y <32)
                {
                    // left, red
                    // B
                    lines[i++] = 0;
                    // G
                    lines[i++] = 0;
                    // R
                    lines[i++] = 255;
                }
                else if ( x < 16 && y >= 32 && y < 48)
                {
                    // bottom, purple
                    // B
                    lines[i++] = 160;
                    // G
                    lines[i++] = 48;
                    // R
                    lines[i++] = 112;
                }
                else
                {
                    // B
                    lines[i++] = 0;
                    // G
                    lines[i++] = 0;
                    // R
                    lines[i++] = 0;
                }
                
                // A
                lines[i++] = 255;
            }
        }
        
        Buffer texData;
        texData.Data = pixels;
        texData.DataSize = dataSize;
        
        Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("CubeTexture", width, height, PixelFormat::E_PF_B8G8R8X8, texData);
        
        return texture;
    }

    MaterialPtr EditorScene::buildMaterial()
    {
        TResult ret;
        
        // vertex & pixel shader keyword
        ShaderKeyword vkeyword;
        vkeyword.addKeyword("-");
        vkeyword.generate();
        ShaderKeyword pkeyword(vkeyword);
        
        // vertex shader
        const String vs =
            "//cbuffer ConstantBuffer : register(b0)\n"
            "//{\n"
            "//   float4x4 modelMatrix;\n"
            "//   float4x4 viewMatrix;\n"
            "//   float4x4 projectionMatrix;\n"
            "//}\n"
            "cbuffer Tiny3DPerDraw : register(b0)\n"
            "{\n"
            "   row_major float4x4 tiny3d_ObjectToWorld;\n"
            "   row_major float4x4 tiny3d_WorldToObject;\n"
            "}\n"
            "\n"
            "cbuffer Tiny3DPerFrame : register(b1)\n"
            "{\n"
            "   row_major float4x4 tiny3d_MatrixV;\n"
            "   row_major float4x4 tiny3d_MatrixP;\n"
            "   row_major float4x4 tiny3d_MatrixVP;\n"
            "}\n"
            "\n"
            "static float4x4 tiny3d_MatrixMVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);\n"
            "static float4x4 tiny3d_MatrixMV = mul(tiny3d_MatrixV, tiny3d_ObjectToWorld);\n"
            "\n"
            "#define TINY3D_MATRIX_M    tiny3d_ObjectToWorld\n"
            "#define TINY3D_MATRIX_V    tiny3d_MatrixV\n"
            "#define TINY3D_MATRIX_P    tiny3d_MatrixP\n"
            "#define TINY3D_MATRIX_VP   tiny3d_MatrixVP\n"
            "#define TINY3D_MATRIX_MV   tiny3d_MatrixMV\n"
            "#define TINY3D_MATRIX_MVP  tiny3d_MatrixMVP\n"
            "\n"
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
            "   //float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);\n"
            "   //float4 viewPosition = mul(viewMatrix, worldPosition);\n"
            "   //float4 clipPosition = mul(projectionMatrix, viewPosition);\n"
            "   //output.position = clipPosition;\n"
            "   //row_major float4x4 MVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);\n"
            "   output.position = mul(TINY3D_MATRIX_MVP, float4(input.position, 1.0f));\n"
            "   //output.position = float4(input.position, 1.0);\n"
            "   output.uv = input.uv;\n"
            "   return output;\n"
            "}\n";
        
        ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vs);
        vshader->setShaderStage(SHADER_STAGE::kVertex);

        // pixel shader
        const String ps =
            "#define CONCATENATE(a, b) a##b\n"
            "#define TEX2D(name) Texture2D name; SamplerState CONCATENATE(sampler, name);\n"
            "#define TEX2D_R(name, r) Texture2D name : register(CONCATENATE(t, r)); SamplerState CONCATENATE(sampler, name) : register(CONCATENATE(s, r));\n"
            "#define SAMPLE(tex, uv) tex.Sample(CONCATENATE(sampler, tex), uv);\n"
            "//Texture2D texCube : register(t0);\n"
            "//SamplerState samplertexCube : register(s0);\n"
            "TEX2D(texCube);\n"
            "struct PS_INPUT\n"
            "{\n"
            "    float4 position : SV_POSITION;\n"
            "    float2 uv : TEXCOORD0;\n"
            "};\n"
            "float4 main(PS_INPUT input) : SV_Target\n"
            "{\n"
            "    //float4 color = texCube.Sample(sampler_texCube, input.uv);\n"
            "    float4 color = SAMPLE(texCube, input.uv);\n"
            "    //float4 color = float4(0.15f, 0.5f, 1.0f, 1.0f);\n"
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

        // // constants
        // ShaderConstantParams constants;
        //
        // // model matrix
        // Matrix4 modelMatrix(false);
        // const String modelMatrixName = "modelMatrix";
        // ShaderConstantParamPtr matrixParam = ShaderConstantParam::create(modelMatrixName, &modelMatrix, sizeof(modelMatrix), ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
        // shader->addConstantParam(matrixParam);
        //
        // // view matrix
        // Matrix4 viewMatrix(false);
        // const String viewMatrixName = "viewMatrix";
        // matrixParam = ShaderConstantParam::create(viewMatrixName, &viewMatrix, sizeof(viewMatrix), ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
        // shader->addConstantParam(matrixParam);
        //
        // // projection matrix
        // Matrix4 projMatrix(false);
        // const String projMatrixName = "projectionMatrix";
        // matrixParam = ShaderConstantParam::create(projMatrixName, &projMatrix, sizeof(projMatrix), ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
        // shader->addConstantParam(matrixParam);
        
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
        ret = material->switchKeywords(enableKeywrods, disableKeywords);

        return material;
    }

    MeshPtr EditorScene::buildMesh()
    {
#if defined (ASSETS_FROM_FILE)
        String path = PROJECT_MGR.getTempPath() + Dir::getNativeSeparator() + "builtin";
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
        // ArchivePtr archive = PROJECT_MGR.getBuiltinArchive();
        T3D_ASSERT(archive != nullptr);
        MeshPtr mesh = T3D_MESH_MGR.loadMesh(archive, "test-cube.tmesh");
        StringArray enableKeywrods;
        enableKeywrods.push_back("");
        StringArray disableKeywords;
        for (auto submesh : mesh->getSubMeshes())
        {
            submesh.second->getMaterial()->switchKeywords(enableKeywrods, disableKeywords);
        }
#else
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
        Vector2 uv(0.5f, 0.5f);
        Vector3 center(0.0f, 0.0f, 0.0f);
        Vector3 extent(0.5f, 0.5f, 0.5f);

        const uint32_t kVertexCount = 24;
        const uint32_t kIndexCount = 36;
        BoxVertex *vertices = new BoxVertex[kVertexCount];
        uint16_t *indices = new uint16_t[kIndexCount];

        const uint32_t kTexSize = 64;

        const Real end1stQuater = Real(15) / Real(kTexSize);
        const Real start2ndQuater= Real(16) / Real(kTexSize);
        const Real end2ndQuater = Real(31) / Real(kTexSize);
        const Real start3rdQuater = Real(32) / Real(kTexSize);
        const Real end3rdQuater = Real(47) / Real(kTexSize);
        const Real start4thQuater = Real(48) / Real(kTexSize);

        // front - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[0].position = center + offset;
        vertices[0].uv = Vector2(0.0f, start2ndQuater);
        
        // front - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[1].position = center + offset;
        vertices[1].uv = Vector2(0.0f, end2ndQuater);
        
        // front - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[2].position = center + offset;
        vertices[2].uv = Vector2(end1stQuater, start2ndQuater);
        
        // front - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[3].position = center + offset;
        vertices[3].uv = Vector2(end1stQuater, end2ndQuater);

        // right - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[4].position = center + offset;
        vertices[4].uv = Vector2(start2ndQuater, start2ndQuater);
        
        // right - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[5].position = center + offset;
        vertices[5].uv = Vector2(start2ndQuater, end2ndQuater);
        
        // right - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[6].position = center + offset;
        vertices[6].uv = Vector2(end2ndQuater, start2ndQuater);
        
        // right - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[7].position = center + offset;
        vertices[7].uv = Vector2(end2ndQuater, end2ndQuater);

        // back - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[8].position = center + offset;
        vertices[8].uv = Vector2(start3rdQuater, start2ndQuater);
        
        // back - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[9].position = center + offset;
        vertices[9].uv = Vector2(start3rdQuater, end2ndQuater);

        // back - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[10].position = center + offset;
        vertices[10].uv = Vector2(end3rdQuater, start2ndQuater);
        
        // back - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[11].position = center + offset;
        vertices[11].uv = Vector2(end3rdQuater, end2ndQuater);
        
        // left - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[12].position = center + offset;
        vertices[12].uv = Vector2(start4thQuater, start2ndQuater);
        
        // left - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[13].position = center + offset;
        vertices[13].uv = Vector2(start4thQuater, end2ndQuater);

        // left - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[14].position = center + offset;
        vertices[14].uv = Vector2(1.0f, start2ndQuater);
        
        // left - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[15].position = center + offset;
        vertices[15].uv = Vector2(1.0f, end2ndQuater);

        // top - V0
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[16].position = center + offset;
        vertices[16].uv = Vector2(0.0f, end1stQuater);
        
        // top - V2
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = -extent[2];
        vertices[17].position = center + offset;
        vertices[17].uv = Vector2(end1stQuater, end1stQuater);

        // top - V4
        offset[0] = extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[18].position = center + offset;
        vertices[18].uv = Vector2(end1stQuater, 0.0f);

        // top - V6
        offset[0] = -extent[0];
        offset[1] = extent[1];
        offset[2] = extent[2];
        vertices[19].position = center + offset;
        vertices[19].uv = Vector2(0.0f, 0.0f);

        // bottom - V1
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[20].position = center + offset;
        vertices[20].uv = Vector2(0.0f, start3rdQuater);

        // bottom - V7
        offset[0] = -extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[21].position = center + offset;
        vertices[21].uv = Vector2(0.0f, end3rdQuater);

        // bottom - V3
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = -extent[2];
        vertices[22].position = center + offset;
        vertices[22].uv = Vector2(end1stQuater, start3rdQuater);
        
        // bottom - V5
        offset[0] = extent[0];
        offset[1] = -extent[1];
        offset[2] = extent[2];
        vertices[23].position = center + offset;
        vertices[23].uv = Vector2(end1stQuater, end3rdQuater);
        
        // Front face
        indices[0] = 0, indices[1] = 2, indices[2] = 1;
        indices[3] = 1, indices[4] = 2, indices[5] = 3;
        
        // Back
        indices[6] = 4, indices[7] = 6, indices[8] = 5;
        indices[9] = 5, indices[10] = 6, indices[11] = 7;
        
        // Left
        indices[12] = 8, indices[13] = 10, indices[14] = 9;
        indices[15] = 9, indices[16] = 10, indices[17] = 11;
        
        // Right
        indices[18] = 12, indices[19] = 14, indices[20] = 13;
        indices[21] = 13, indices[22] = 14, indices[23] = 15;
        
        // Top
        indices[24] = 16, indices[25] = 19, indices[26] = 18;
        indices[27] = 18, indices[28] = 17, indices[29] = 16;
        
        // Bottom
        indices[30] = 20, indices[31] = 22, indices[32] = 21;
        indices[33] = 21, indices[34] = 22, indices[35] = 23;
        
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
        MaterialPtr material = buildMaterial();
        SubMeshPtr submesh = SubMesh::create(name, material, PrimitiveType::kTriangleList, indexBuffer, true);
        SubMeshes subMeshes;
        subMeshes.emplace(name, submesh);

        MeshPtr mesh = T3D_MESH_MGR.createMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
#endif
        return mesh;
    }

    void EditorScene::buildAabb(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
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

    //--------------------------------------------------------------------------
    
    NS_END
}
