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

#include "DynamicBatchApp.h"
#include "Render/T3DRenderPipeline.h"
#include "T3DRmlCanvas.h"
#include "UI/T3DUISystem.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>

#include <sstream>

#define ARCHIVE_TYPE_FS         "FileSystem"
#define ARCHIVE_TYPE_ANDROID    "AndroidAsset"


#define UVN_CAMERA

using namespace Tiny3D;

const char *SUB_MESH_NAME = "#0";

DynamicBatchApp theApp;

#if defined(T3D_OS_WINDOWS)
extern const char *SHADOW_VERTEX_SHADER;
extern const char *FORWARD_VERTEX_SHADER;
extern const char *FORWARD_PIXEL_SHADER;
extern const char *SHADOW_VERTEX_SHADER_GL;
extern const char *FORWARD_VERTEX_SHADER_GL;
extern const char *FORWARD_PIXEL_SHADER_GL;
#endif
#if defined(T3D_OS_WINDOWS) || defined(T3D_OS_ANDROID)
extern const unsigned char SHADOW_VERTEX_SHADER_VK[];
extern const size_t SHADOW_VERTEX_SHADER_VK_SIZE;
extern const unsigned char FORWARD_VERTEX_SHADER_VK[];
extern const size_t FORWARD_VERTEX_SHADER_VK_SIZE;
extern const unsigned char FORWARD_PIXEL_SHADER_VK[];
extern const size_t FORWARD_PIXEL_SHADER_VK_SIZE;
#endif
#if defined(T3D_OS_ANDROID)
extern const char *SHADOW_VERTEX_SHADER_GLES;
extern const char *FORWARD_VERTEX_SHADER_GLES;
extern const char *FORWARD_PIXEL_SHADER_GLES;
#endif

DynamicBatchApp::DynamicBatchApp()
{
}

DynamicBatchApp::~DynamicBatchApp()
{
}

TResult DynamicBatchApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // 开启前向渲染管线的动态合批
    T3D_AGENT.getSettings().renderSettings.enableDynamicBatch = true;
    T3D_RENDER_PIPELINE.enableDynamicBatch(true);

#if defined(T3D_OS_ANDROID)
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_ANDROID, "", Archive::AccessMode::kRead);
#else
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_FS, Dir::getAppPath(), Archive::AccessMode::kRead);
#endif
    if (archive != nullptr)
    {
        T3D_ASSET_MGR.init(AssetManager::Mode::kRuntime);
        T3D_ASSET_MGR.mount(archive, 0);
    }

    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("DynamicBatchScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    // add ambient light to the root of scene
    AmbientLightPtr ambient = scene->getRootGameObject()->addComponent<AmbientLight>();
    ambient->setColor(ColorRGB::WHITE);
    ambient->setIntensity(0.45f);

    // root game object
    GameObjectPtr go = GameObject::create("DynamicBatchRoot");
    Transform3DPtr root = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);

    // directional light (casts shadow)
    go = GameObject::create("DirectionLight");
    Transform3DPtr node = go->addComponent<Transform3D>();
    Vector3 eye(-3.0f, 5.0f, -5.0f);
    Matrix3 matR;
    matR.lookAt_LH(eye, Vector3::ZERO, Vector3::UP);
    Quaternion q(matR);
    node->setOrientation(q);
    node->setPosition(eye);
    root->addChild(node);
    DirectionalLightPtr light = go->addComponent<DirectionalLight>();
    light->setColor(ColorRGB::WHITE);
    light->setDiffuseIntensity(1.0f);
    light->setSpecularIntensity(1.0f);

    // point light
    go = GameObject::create("PointLight");
    node = go->addComponent<Transform3D>();
    Vector3 lightPos(3.0f, 3.0f, -3.0f);
    node->setPosition(lightPos);
    root->addChild(node);
    PointLightPtr pointLight = go->addComponent<PointLight>();
    pointLight->setColor(ColorRGB::WHITE);
    pointLight->setDiffuseIntensity(0.8f);
    pointLight->setSpecularIntensity(0.8f);
    pointLight->setAttenuationConstant(1.0f);
    pointLight->setAttenuationLinear(0.09f);
    pointLight->setAttenuationQuadratic(0.032f);

    // shader
    ShaderPtr shader = buildShader();

    // cube & plane material
    mCubeMaterial = buildCubeMaterial(shader);
    mPlaneMaterial = buildPlaneMaterial(shader);

    // cube mesh
    mCubeMesh = buildCubeMesh(mCubeMaterial->getUUID());

    // plane mesh
    mPlaneMesh = buildPlaneMesh(mPlaneMaterial->getUUID());

    // camera
    buildCamera(root);

    // 放置 20 个共享同一材质（mCubeMaterial）的 Cube（5 列 × 4 行网格排布）
    const int32_t kCols = 5;
    const int32_t kRows = 4;
    const Real kSpacingX = 2.0f;
    const Real kSpacingZ = 2.0f;
    const Real startX = -((kCols - 1) * 0.5f) * kSpacingX;
    const Real startZ = -((kRows - 1) * 0.5f) * kSpacingZ;

    int32_t cubeIndex = 0;
    for (int32_t r = 0; r < kRows; ++r)
    {
        for (int32_t c = 0; c < kCols; ++c)
        {
            Real x = startX + c * kSpacingX;
            Real z = startZ + r * kSpacingZ;
            Vector3 pos(x, 0.5f, z);
            Degree deg(static_cast<Real>(cubeIndex * 18));
            Radian yAngles(deg.valueRadians());
            Vector3 scale(0.8f, 0.8f + (cubeIndex % 3) * 0.15f, 0.8f);
            buildCube(root, pos, yAngles, scale);
            ++cubeIndex;
        }
    }

    // plane
    buildPlane(root);

    T3D_LOG_INFO(LOG_TAG_APP,
        "[DynamicBatchApp] Controls: Press [Space] / [B] (or [1]=ON, [2]=OFF, Touch on mobile) to toggle Dynamic Batching.");

    return T3D_OK;
}

void DynamicBatchApp::setupUI(GameObject *cameraObject)
{
    if (cameraObject == nullptr || T3D_AGENT.getUISystem() == nullptr)
    {
        return;
    }

    RmlCanvasPtr canvas = cameraObject->addComponent<RmlCanvas>();
    if (canvas == nullptr || canvas->getContext() == nullptr)
    {
        return;
    }

    mCanvas = canvas.get();
    mCanvas->setDocumentsLoaded(true);
}

void DynamicBatchApp::ensureHudDocument()
{
    if (mHudDoc != nullptr || mCanvas == nullptr || mCanvas->getContext() == nullptr)
    {
        return;
    }

    const char *rmlSource = R"(
<rml>
<head>
    <title>DynamicBatch HUD</title>
    <style>
        body {
            font-family: "Noto Sans SC";
            font-size: 16dp;
            color: #f0f4f8;
            width: 100%;
            height: 100%;
        }
        #stats_box {
            display: block;
            position: absolute;
            top: 16dp;
            right: 16dp;
            width: 340dp;
            padding: 12dp 16dp;
            background-color: #141c28d9;
            border-width: 1dp;
            border-color: #4a6282;
            border-radius: 6dp;
        }
        .title {
            display: block;
            font-size: 18dp;
            color: #ffd166;
            margin-bottom: 8dp;
        }
        .row {
            display: block;
            margin-bottom: 4dp;
        }
        .hint {
            display: block;
            margin-top: 8dp;
            padding-top: 6dp;
            border-top-width: 1dp;
            border-top-color: #2d3e55;
            font-size: 14dp;
            color: #9bb4d0;
        }
    </style>
</head>
<body>
    <div id="stats_box">
        <div class="title">Dynamic Batching Stats</div>
        <div id="line_mode" class="row">DynamicBatch: ON</div>
        <div id="line_batches" class="row">Batches: 0 (Batched: 0, Unbatched: 0)</div>
        <div id="line_verts" class="row">Batched Mesh: 0 verts / 0 indices</div>
        <div id="line_draws" class="row">DrawCalls: 0 (Shadow: 0, Forward: 0)</div>
        <div class="hint">[Space / B]: Toggle  |  [1]: ON  |  [2]: OFF</div>
    </div>
</body>
</rml>
)";

    mHudDoc = mCanvas->getContext()->LoadDocumentFromMemory(rmlSource);
    if (mHudDoc != nullptr)
    {
        mHudDoc->Show();
        mLastDynamicBatchCount = UINT32_MAX;
        mLastTotalDrawCalls = UINT32_MAX;
    }
}

void DynamicBatchApp::setDynamicBatchMode(bool enabled)
{
    T3D_AGENT.getSettings().renderSettings.enableDynamicBatch = enabled;
    T3D_RENDER_PIPELINE.enableDynamicBatch(enabled);
    mLastDynamicBatchCount = UINT32_MAX;
    mLastTotalDrawCalls = UINT32_MAX;
}

void DynamicBatchApp::onPreRender()
{
    ensureHudDocument();

    if (Input::getInstancePtr() != nullptr)
    {
        if (T3D_INPUT.getKeyDown(APP_SCANCODE_1) || T3D_INPUT.getKeyDown(APP_SCANCODE_KP_1))
        {
            setDynamicBatchMode(true);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_2) || T3D_INPUT.getKeyDown(APP_SCANCODE_KP_2))
        {
            setDynamicBatchMode(false);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_SPACE) || T3D_INPUT.getKeyDown(APP_SCANCODE_B))
        {
            setDynamicBatchMode(!T3D_RENDER_PIPELINE.isDynamicBatchEnabled());
        }
        else if (T3D_INPUT.getTouchCount() == 1)
        {
            const Touch touch = T3D_INPUT.getTouch(0);
            if (touch.phase == TouchPhase::Began)
            {
                setDynamicBatchMode(!T3D_RENDER_PIPELINE.isDynamicBatchEnabled());
            }
        }
    }
}

void DynamicBatchApp::onPostRender()
{
    if (updateStatsDisplay(false) && mCanvas != nullptr && mCanvas->getContext() != nullptr)
    {
        // DOM 文本变更后同步 Update 一次，保证下一帧 UI 立即绘出最新数据
        mCanvas->getContext()->Update();
    }
}

bool DynamicBatchApp::updateStatsDisplay(bool forceUpdate)
{
    const RenderPipeline::FrameStats &stats = T3D_RENDER_PIPELINE.getFrameStats();
    const bool batchOn = T3D_RENDER_PIPELINE.isDynamicBatchEnabled();

    if (!forceUpdate
        && stats.dynamicBatchCount == mLastDynamicBatchCount
        && stats.totalDrawCalls == mLastTotalDrawCalls
        && batchOn == mLastBatchEnabled)
    {
        return false;
    }

    mLastDynamicBatchCount = stats.dynamicBatchCount;
    mLastTotalDrawCalls = stats.totalDrawCalls;
    mLastBatchEnabled = batchOn;

    T3D_LOG_INFO(LOG_TAG_APP,
        "DynamicBatch: %s | Batches: %u (BatchedObjs: %u, UnbatchedObjs: %u) | DrawCalls: %u (Shadow: %u, Forward: %u)",
        batchOn ? "ON" : "OFF",
        stats.dynamicBatchCount,
        stats.batchedObjects,
        stats.unbatchedObjects,
        stats.totalDrawCalls,
        stats.shadowDrawCalls,
        stats.forwardDrawCalls);

    if (mHudDoc == nullptr)
    {
        return false;
    }

    if (Rml::Element *elMode = mHudDoc->GetElementById("line_mode"))
    {
        elMode->SetInnerRML(batchOn
            ? "DynamicBatch: <span style=\"color: #52e580;\">ON</span>"
            : "DynamicBatch: <span style=\"color: #ff6b6b;\">OFF</span>");
    }

    if (Rml::Element *elBatches = mHudDoc->GetElementById("line_batches"))
    {
        std::stringstream ss;
        ss << "Batches: " << stats.dynamicBatchCount
           << " (Batched: " << stats.batchedObjects
           << ", Unbatched: " << stats.unbatchedObjects << ")";
        elBatches->SetInnerRML(ss.str());
    }

    if (Rml::Element *elVerts = mHudDoc->GetElementById("line_verts"))
    {
        std::stringstream ss;
        ss << "Batched Mesh: " << stats.batchedVertices
           << " verts / " << stats.batchedIndices << " indices";
        elVerts->SetInnerRML(ss.str());
    }

    if (Rml::Element *elDraws = mHudDoc->GetElementById("line_draws"))
    {
        std::stringstream ss;
        ss << "DrawCalls: " << stats.totalDrawCalls
           << " (Shadow: " << stats.shadowDrawCalls
           << ", Forward: " << stats.forwardDrawCalls << ")";
        elDraws->SetInnerRML(ss.str());
    }

    return true;
}

void DynamicBatchApp::applicationWillTerminate()
{
    mHudDoc = nullptr;
    mCanvas = nullptr;
    mCubeMesh = nullptr;
    mPlaneMesh = nullptr;
    mCubeMaterial = nullptr;
    mPlaneMaterial = nullptr;
}

void DynamicBatchApp::buildCamera(Transform3D *parent)
{
    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    GameObjectPtr go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);

    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB::BLACK);
    camera->setRenderTarget(rt);

    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);

    Vector3 eye(0.0f, 6.5f, -8.5f);

#if defined (UVN_CAMERA)
    Vector3 obj(0.0f, 0.0f, 0.0f);
    camera->lookAt(eye, obj, Vector3::UP);
#else
    xform->setPosition(eye);
    Radian xAngle(Degree(35.0f).valueRadians());
    Radian yAngle(0.0f);
    Radian zAngle(0.0f);
    xform->rotate(xAngle, yAngle, zAngle);
#endif

    auto frustum = go->addComponent<FrustumBound>();
    T3D_ASSERT(frustum != nullptr);

    setupUI(go.get());
}

PassPtr DynamicBatchApp::buildShadowPass()
{
    ShaderKeyword vkeyword;
    vkeyword.addKeyword("");
    vkeyword.generate();

    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
    const String rendererName = renderer->getName();
    const SHADER_LANGUAGE shadingLang = renderer->getShadingLanguage();
    const char *vsCode = nullptr;
    size_t vsSize = 0;

#if defined(T3D_OS_WINDOWS)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        vsSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else if (rendererName == RHIRenderer::OPENGL4)
    {
        vsCode = SHADOW_VERTEX_SHADER_GL;
        vsSize = strlen(SHADOW_VERTEX_SHADER_GL);
    }
    else
    {
        vsCode = SHADOW_VERTEX_SHADER;
        vsSize = strlen(SHADOW_VERTEX_SHADER);
    }
#elif defined(T3D_OS_ANDROID)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        vsSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else
    {
        vsCode = SHADOW_VERTEX_SHADER_GLES;
        vsSize = strlen(SHADOW_VERTEX_SHADER_GLES);
    }
#elif defined(T3D_OS_LINUX)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        vsSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else
    {
        vsCode = SHADOW_VERTEX_SHADER_GL;
        vsSize = strlen(SHADOW_VERTEX_SHADER_GL);
    }
#endif

    ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vsCode, vsSize);
    vshader->setShaderStage(SHADER_STAGE::kVertex);
    vshader->setLanguage(shadingLang);

    PassPtr pass = Pass::create("ShadowCaster");
    TResult ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeShadowCasterStr);

    RenderStatePtr renderState = RenderState::create();
    BlendDesc blendDesc;
    renderState->setBlendDesc(blendDesc);

    DepthStencilDesc depthStencilDesc;
    renderState->setDepthStencilDesc(depthStencilDesc);

    RasterizerDesc rasterizeDesc;
    rasterizeDesc.DepthBias = 50.0f;
    rasterizeDesc.SlopeScaledDepthBias = 2.5f;
    renderState->setRasterizerDesc(rasterizeDesc);
    pass->setRenderState(renderState);

    return pass;
}

PassPtr DynamicBatchApp::buildForwardPass()
{
    ShaderKeyword vkeyword;
    vkeyword.addKeyword("");
    vkeyword.generate();
    ShaderKeyword pkeyword(vkeyword);

    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
    const String rendererName = renderer->getName();
    const SHADER_LANGUAGE shadingLang = renderer->getShadingLanguage();
    const char *vsCode = nullptr;
    size_t vsSize = 0;

#if defined(T3D_OS_WINDOWS)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        vsSize = FORWARD_VERTEX_SHADER_VK_SIZE;
    }
    else if (rendererName == RHIRenderer::OPENGL4)
    {
        vsCode = FORWARD_VERTEX_SHADER_GL;
        vsSize = strlen(FORWARD_VERTEX_SHADER_GL);
    }
    else
    {
        vsCode = FORWARD_VERTEX_SHADER;
        vsSize = strlen(FORWARD_VERTEX_SHADER);
    }
#elif defined(T3D_OS_ANDROID)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        vsSize = FORWARD_VERTEX_SHADER_VK_SIZE;
    }
    else
    {
        vsCode = FORWARD_VERTEX_SHADER_GLES;
        vsSize = strlen(FORWARD_VERTEX_SHADER_GLES);
    }
#elif defined(T3D_OS_LINUX)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        vsSize = FORWARD_VERTEX_SHADER_VK_SIZE;
    }
    else
    {
        vsCode = FORWARD_VERTEX_SHADER_GL;
        vsSize = strlen(FORWARD_VERTEX_SHADER_GL);
    }
#endif

    ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vsCode, vsSize);
    vshader->setShaderStage(SHADER_STAGE::kVertex);
    vshader->setLanguage(shadingLang);

    const char *psCode = nullptr;
    size_t psSize = 0;

#if defined(T3D_OS_WINDOWS)
    if (rendererName == RHIRenderer::VULKAN)
    {
        psCode = reinterpret_cast<const char*>(FORWARD_PIXEL_SHADER_VK);
        psSize = FORWARD_PIXEL_SHADER_VK_SIZE;
    }
    else if (rendererName == RHIRenderer::OPENGL4)
    {
        psCode = FORWARD_PIXEL_SHADER_GL;
        psSize = strlen(FORWARD_PIXEL_SHADER_GL);
    }
    else
    {
        psCode = FORWARD_PIXEL_SHADER;
        psSize = strlen(FORWARD_PIXEL_SHADER);
    }
#elif defined(T3D_OS_ANDROID)
    if (rendererName == RHIRenderer::VULKAN)
    {
        psCode = reinterpret_cast<const char*>(FORWARD_PIXEL_SHADER_VK);
        psSize = FORWARD_PIXEL_SHADER_VK_SIZE;
    }
    else
    {
        psCode = FORWARD_PIXEL_SHADER_GLES;
        psSize = strlen(FORWARD_PIXEL_SHADER_GLES);
    }
#elif defined(T3D_OS_LINUX)
    if (rendererName == RHIRenderer::VULKAN)
    {
        psCode = reinterpret_cast<const char*>(FORWARD_PIXEL_SHADER_VK);
        psSize = FORWARD_PIXEL_SHADER_VK_SIZE;
    }
    else
    {
        psCode = FORWARD_PIXEL_SHADER_GL;
        psSize = strlen(FORWARD_PIXEL_SHADER_GL);
    }
#endif

    ShaderVariantPtr pshader = ShaderVariant::create(std::move(pkeyword), psCode, psSize);
    pshader->setShaderStage(SHADER_STAGE::kPixel);
    pshader->setLanguage(shadingLang);

    PassPtr pass = Pass::create("ForwardBase");
    TResult ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    ret = pass->addShaderVariant(pshader->getShaderKeyword(), pshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);

    RenderStatePtr renderState = RenderState::create();
    BlendDesc blendDesc;
    renderState->setBlendDesc(blendDesc);

    DepthStencilDesc depthStencilDesc;
    renderState->setDepthStencilDesc(depthStencilDesc);

    RasterizerDesc rasterizeDesc;
    renderState->setRasterizerDesc(rasterizeDesc);
    pass->setRenderState(renderState);

    return pass;
}

ShaderPtr DynamicBatchApp::buildShader()
{
    PassPtr shadowPass = buildShadowPass();
    PassPtr forwardPass = buildForwardPass();

    TechniquePtr tech = Technique::create("Default-Technique");
    bool rval = tech->addPass(forwardPass);
    T3D_ASSERT(rval);
    rval = tech->addPass(shadowPass);
    T3D_ASSERT(rval);
    tech->addTag(ShaderLab::kBuiltinTagQueue, ShaderLab::kBuiltinQueueGeometryStr);

    ShaderPtr shader = T3D_SHADER_MGR.createShader("Default-Shader");
    rval = shader->addTechnique(tech);
    T3D_ASSERT(rval);

    return shader;
}

void DynamicBatchApp::buildCube(Transform3D *parent, const Vector3 &pos, const Radian &yAngles, const Vector3 &scale)
{
    static int index = 0;
    std::stringstream ss;
    ss << "Cube#" << index++;

    GameObjectPtr go = GameObject::create(ss.str());
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);
    xform->setPosition(pos);
    Radian xAngles(0.0f);
    Radian zAngles(0.0f);
    xform->rotate(xAngles, yAngles, zAngles);
    xform->setScaling(scale);

    SubMesh *submesh = mCubeMesh->getSubMesh(SUB_MESH_NAME);

    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mCubeMesh, submesh);

    AabbBoundPtr bound = go->addComponent<AabbBound>();
    buildAabb(mCubeMesh, submesh, bound);
}

Texture2DPtr DynamicBatchApp::buildCubeTexture()
{
    const uint32_t width = 64;
    const uint32_t height = 64;
    uint32_t pitch = Image::calcPitch(width, 32);
    const uint32_t dataSize = pitch * height;
    uint8_t *pixels = T3D_POD_NEW_ARRAY(uint8_t, dataSize);

    for (uint32_t y = 0; y < height; ++y)
    {
        uint8_t *lines = pixels + pitch * y;
        uint32_t i = 0;
        for (uint32_t x = 0; x < width; ++x)
        {
            if (x < 16 && y < 16)
            {
                // top, blue
                lines[i++] = 196;
                lines[i++] = 114;
                lines[i++] = 68;
            }
            else if (x < 16 && y >= 16 && y < 32)
            {
                // front, orange
                lines[i++] = 49;
                lines[i++] = 125;
                lines[i++] = 237;
            }
            else if (x >= 16 && x < 32 && y >= 16 && y < 32)
            {
                // right, green
                lines[i++] = 71;
                lines[i++] = 173;
                lines[i++] = 112;
            }
            else if (x >= 32 && x < 48 && y >= 16 && y < 32)
            {
                // back, yellow
                lines[i++] = 0;
                lines[i++] = 192;
                lines[i++] = 255;
            }
            else if (x >= 48 && x < 64 && y >= 16 && y < 32)
            {
                // left, red
                lines[i++] = 0;
                lines[i++] = 0;
                lines[i++] = 255;
            }
            else if (x < 16 && y >= 32 && y < 48)
            {
                // bottom, purple
                lines[i++] = 160;
                lines[i++] = 48;
                lines[i++] = 112;
            }
            else
            {
                lines[i++] = 0;
                lines[i++] = 0;
                lines[i++] = 0;
            }

            lines[i++] = 255;
        }
    }

    Buffer texData;
    texData.Data = pixels;
    texData.DataSize = dataSize;

    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("textureCube", width, height, PixelFormat::E_PF_B8G8R8X8, texData);

    return texture;
}

MaterialPtr DynamicBatchApp::buildCubeMaterial(Tiny3D::Shader *shader)
{
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildCubeTexture();
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);

    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Cube-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());

    // Camera
    material->setVector("tiny3d_CameraWorldPos", Vector4::ZERO);
    // Object
    material->setVector("tiny3d_ObjectSmoothness", Vector4(0.5f, 0, 0, 0));
    // ProjectionParams
    material->setVector("tiny3d_ProjectionParams", Vector4(1.0f, 0, 0, 0));
    // Ambient
    material->setColor("tiny3d_AmbientLight", ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    // Directional light
    material->setColor("tiny3d_DirLightColor", ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setVector("tiny3d_DirLightDir", Vector4::ZERO);
    // Point lights
    ColorArray colors(4, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setColorArray("tiny3d_PointLightColor", colors);
    Vector4Array values(4, Vector4::ZERO);
    material->setVectorArray("tiny3d_PointLightPos", values);
    material->setVectorArray("tiny3d_PointLightAttenuation", values);
    // Spot lights
    material->setColorArray("tiny3d_SpotLightColor", colors);
    material->setVectorArray("tiny3d_SpotLightPos", values);
    material->setVectorArray("tiny3d_SpotLightDir", values);
    material->setVectorArray("tiny3d_SpotLightAttenuation", values);

    return material;
}

MeshPtr DynamicBatchApp::buildCubeMesh(const Tiny3D::UUID &materialUUID)
{
    struct BoxVertex
    {
        Vector3 position {};
        Vector3 normal {};
        Vector2 uv {};
    };

    VertexAttribute attrPos(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
    VertexAttribute attrNormal(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
    VertexAttribute attrUV(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
    VertexAttributes attributes(3);
    attributes[0] = attrPos;
    attributes[1] = attrNormal;
    attributes[2] = attrUV;

    Vector3 offset;
    Vector3 center(0.0f, 0.0f, 0.0f);
    Vector3 extent(0.5f, 0.5f, 0.5f);

    const uint32_t kVertexCount = 24;
    const uint32_t kIndexCount = 36;
    BoxVertex *vertices = T3D_POD_NEW_ARRAY(BoxVertex, kVertexCount);
    uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, kIndexCount);

    const uint32_t kTexSize = 64;
    const Real end1stQuater = Real(15) / Real(kTexSize);
    const Real start2ndQuater = Real(16) / Real(kTexSize);
    const Real end2ndQuater = Real(31) / Real(kTexSize);
    const Real start3rdQuater = Real(32) / Real(kTexSize);
    const Real end3rdQuater = Real(47) / Real(kTexSize);
    const Real start4thQuater = Real(48) / Real(kTexSize);

    // front - V0..V3
    offset = Vector3(-extent[0], extent[1], -extent[2]);
    vertices[0].position = center + offset;
    vertices[0].normal = -Vector3::FORWARD;
    vertices[0].uv = Vector2(0.0f, start2ndQuater);

    offset = Vector3(-extent[0], -extent[1], -extent[2]);
    vertices[1].position = center + offset;
    vertices[1].normal = -Vector3::FORWARD;
    vertices[1].uv = Vector2(0.0f, end2ndQuater);

    offset = Vector3(extent[0], extent[1], -extent[2]);
    vertices[2].position = center + offset;
    vertices[2].normal = -Vector3::FORWARD;
    vertices[2].uv = Vector2(end1stQuater, start2ndQuater);

    offset = Vector3(extent[0], -extent[1], -extent[2]);
    vertices[3].position = center + offset;
    vertices[3].normal = -Vector3::FORWARD;
    vertices[3].uv = Vector2(end1stQuater, end2ndQuater);

    // right - V4..V7
    offset = Vector3(extent[0], extent[1], -extent[2]);
    vertices[4].position = center + offset;
    vertices[4].normal = Vector3::RIGHT;
    vertices[4].uv = Vector2(start2ndQuater, start2ndQuater);

    offset = Vector3(extent[0], -extent[1], -extent[2]);
    vertices[5].position = center + offset;
    vertices[5].normal = Vector3::RIGHT;
    vertices[5].uv = Vector2(start2ndQuater, end2ndQuater);

    offset = Vector3(extent[0], extent[1], extent[2]);
    vertices[6].position = center + offset;
    vertices[6].normal = Vector3::RIGHT;
    vertices[6].uv = Vector2(end2ndQuater, start2ndQuater);

    offset = Vector3(extent[0], -extent[1], extent[2]);
    vertices[7].position = center + offset;
    vertices[7].normal = Vector3::RIGHT;
    vertices[7].uv = Vector2(end2ndQuater, end2ndQuater);

    // back - V8..V11
    offset = Vector3(extent[0], extent[1], extent[2]);
    vertices[8].position = center + offset;
    vertices[8].normal = Vector3::FORWARD;
    vertices[8].uv = Vector2(start3rdQuater, start2ndQuater);

    offset = Vector3(extent[0], -extent[1], extent[2]);
    vertices[9].position = center + offset;
    vertices[9].normal = Vector3::FORWARD;
    vertices[9].uv = Vector2(start3rdQuater, end2ndQuater);

    offset = Vector3(-extent[0], extent[1], extent[2]);
    vertices[10].position = center + offset;
    vertices[10].normal = Vector3::FORWARD;
    vertices[10].uv = Vector2(end3rdQuater, start2ndQuater);

    offset = Vector3(-extent[0], -extent[1], extent[2]);
    vertices[11].position = center + offset;
    vertices[11].normal = Vector3::FORWARD;
    vertices[11].uv = Vector2(end3rdQuater, end2ndQuater);

    // left - V12..V15
    offset = Vector3(-extent[0], extent[1], extent[2]);
    vertices[12].position = center + offset;
    vertices[12].normal = -Vector3::RIGHT;
    vertices[12].uv = Vector2(start4thQuater, start2ndQuater);

    offset = Vector3(-extent[0], -extent[1], extent[2]);
    vertices[13].position = center + offset;
    vertices[13].normal = -Vector3::RIGHT;
    vertices[13].uv = Vector2(start4thQuater, end2ndQuater);

    offset = Vector3(-extent[0], extent[1], -extent[2]);
    vertices[14].position = center + offset;
    vertices[14].normal = -Vector3::RIGHT;
    vertices[14].uv = Vector2(1.0f, start2ndQuater);

    offset = Vector3(-extent[0], -extent[1], -extent[2]);
    vertices[15].position = center + offset;
    vertices[15].normal = -Vector3::RIGHT;
    vertices[15].uv = Vector2(1.0f, end2ndQuater);

    // top - V16..V19
    offset = Vector3(-extent[0], extent[1], -extent[2]);
    vertices[16].position = center + offset;
    vertices[16].normal = Vector3::UP;
    vertices[16].uv = Vector2(0.0f, end1stQuater);

    offset = Vector3(extent[0], extent[1], -extent[2]);
    vertices[17].position = center + offset;
    vertices[17].normal = Vector3::UP;
    vertices[17].uv = Vector2(end1stQuater, end1stQuater);

    offset = Vector3(extent[0], extent[1], extent[2]);
    vertices[18].position = center + offset;
    vertices[18].normal = Vector3::UP;
    vertices[18].uv = Vector2(end1stQuater, 0.0f);

    offset = Vector3(-extent[0], extent[1], extent[2]);
    vertices[19].position = center + offset;
    vertices[19].normal = Vector3::UP;
    vertices[19].uv = Vector2(0.0f, 0.0f);

    // bottom - V20..V23
    offset = Vector3(-extent[0], -extent[1], -extent[2]);
    vertices[20].position = center + offset;
    vertices[20].normal = -Vector3::UP;
    vertices[20].uv = Vector2(0.0f, start3rdQuater);

    offset = Vector3(-extent[0], -extent[1], extent[2]);
    vertices[21].position = center + offset;
    vertices[21].normal = -Vector3::UP;
    vertices[21].uv = Vector2(0.0f, end3rdQuater);

    offset = Vector3(extent[0], -extent[1], -extent[2]);
    vertices[22].position = center + offset;
    vertices[22].normal = -Vector3::UP;
    vertices[22].uv = Vector2(end1stQuater, start3rdQuater);

    offset = Vector3(extent[0], -extent[1], extent[2]);
    vertices[23].position = center + offset;
    vertices[23].normal = -Vector3::UP;
    vertices[23].uv = Vector2(end1stQuater, end3rdQuater);

    // Front face
    indices[0] = 0, indices[1] = 2, indices[2] = 1;
    indices[3] = 1, indices[4] = 2, indices[5] = 3;
    // Right
    indices[6] = 4, indices[7] = 6, indices[8] = 5;
    indices[9] = 5, indices[10] = 6, indices[11] = 7;
    // Back
    indices[12] = 8, indices[13] = 10, indices[14] = 9;
    indices[15] = 9, indices[16] = 10, indices[17] = 11;
    // Left
    indices[18] = 12, indices[19] = 14, indices[20] = 13;
    indices[21] = 13, indices[22] = 14, indices[23] = 15;
    // Top
    indices[24] = 16, indices[25] = 19, indices[26] = 18;
    indices[27] = 18, indices[28] = 17, indices[29] = 16;
    // Bottom
    indices[30] = 20, indices[31] = 22, indices[32] = 21;
    indices[33] = 21, indices[34] = 22, indices[35] = 23;

    Buffer vertexBuffer;
    vertexBuffer.Data = reinterpret_cast<uint8_t *>(vertices);
    vertexBuffer.DataSize = sizeof(BoxVertex) * kVertexCount;
    Vertices vertexBuffers(1);
    vertexBuffers[0] = vertexBuffer;

    VertexStrides strides(1, sizeof(BoxVertex));
    VertexOffsets offsets(1, 0);

    Buffer indexBuffer;
    indexBuffer.Data = reinterpret_cast<uint8_t *>(indices);
    indexBuffer.DataSize = sizeof(uint16_t) * kIndexCount;

    String name = SUB_MESH_NAME;
    SubMeshPtr submesh = SubMesh::create(name, materialUUID, PrimitiveType::kTriangleList, std::move(indexBuffer), true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}

void DynamicBatchApp::buildAabb(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
{
    const VertexAttribute *attr = mesh->findVertexAttributeBySemantic(VertexAttribute::Semantic::E_VAS_POSITION, 0);
    size_t vertexSize = mesh->getVertexStride(attr->getSlot());
    size_t offset = mesh->getVertexOffset(attr->getOffset());
    const Buffer &vertexBuffer = mesh->getVertices()[attr->getSlot()];
    const Buffer &indexBuffer = submesh->getIndices();
    size_t indexSize = submesh->getIndexBuffer()->getIndexSize();
    size_t pointCount = submesh->getIndexBuffer()->getIndexCount();
    Vector3 *points = T3D_POD_NEW_ARRAY(Vector3, pointCount);
    for (size_t i = 0; i < pointCount; ++i)
    {
        int32_t idx = 0;
        const uint8_t *src = indexBuffer.Data + i * indexSize;
        memcpy(&idx, src, indexSize);
        src = vertexBuffer.Data + idx * vertexSize + offset;
        memcpy(points + i, src, sizeof(Vector3));
    }
    Aabb aabb;
    aabb.build(points, pointCount);
    T3D_POD_SAFE_DELETE_ARRAY(points);
    bound->setParams(aabb.getMinX(), aabb.getMaxX(), aabb.getMinY(), aabb.getMaxY(), aabb.getMinZ(), aabb.getMaxZ());
}

Texture2DPtr DynamicBatchApp::buildPlaneTexture()
{
    const uint32_t width = 64;
    const uint32_t height = 64;
    uint32_t pitch = Image::calcPitch(width, 32);
    const uint32_t dataSize = pitch * height;
    uint8_t *pixels = T3D_POD_NEW_ARRAY(uint8_t, dataSize);

    for (uint32_t y = 0; y < height; ++y)
    {
        uint8_t *lines = pixels + pitch * y;
        uint32_t i = 0;
        for (uint32_t x = 0; x < width; ++x)
        {
            lines[i++] = 255;
            lines[i++] = 255;
            lines[i++] = 255;
            lines[i++] = 255;
        }
    }

    Buffer texData;
    texData.Data = pixels;
    texData.DataSize = dataSize;

    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("texturePlane", width, height, PixelFormat::E_PF_B8G8R8X8, texData);

    return texture;
}

MaterialPtr DynamicBatchApp::buildPlaneMaterial(Tiny3D::Shader *shader)
{
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildPlaneTexture();
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);

    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Plane-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());

    material->setVector("tiny3d_CameraWorldPos", Vector4::ZERO);
    material->setVector("tiny3d_ObjectSmoothness", Vector4(0.5f, 0, 0, 0));
    material->setVector("tiny3d_ProjectionParams", Vector4(1.0f, 0, 0, 0));
    material->setColor("tiny3d_AmbientLight", ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setColor("tiny3d_DirLightColor", ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setVector("tiny3d_DirLightDir", Vector4::ZERO);

    ColorArray colors(4, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    material->setColorArray("tiny3d_PointLightColor", colors);
    Vector4Array values(4, Vector4::ZERO);
    material->setVectorArray("tiny3d_PointLightPos", values);
    material->setVectorArray("tiny3d_PointLightAttenuation", values);
    material->setColorArray("tiny3d_SpotLightColor", colors);
    material->setVectorArray("tiny3d_SpotLightPos", values);
    material->setVectorArray("tiny3d_SpotLightDir", values);
    material->setVectorArray("tiny3d_SpotLightAttenuation", values);

    return material;
}

MeshPtr DynamicBatchApp::buildPlaneMesh(const Tiny3D::UUID &materialUUID)
{
    struct QuadVertex
    {
        Vector3 position {};
        Vector3 normal {};
        Vector2 uv {};
    };

    VertexAttribute attrPos(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
    VertexAttribute attrNormal(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
    VertexAttribute attrUV(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
    VertexAttributes attributes(3);
    attributes[0] = attrPos;
    attributes[1] = attrNormal;
    attributes[2] = attrUV;

    const uint32_t kVertexCount = 4;
    const uint32_t kIndexCount = 6;
    QuadVertex *vertices = T3D_POD_NEW_ARRAY(QuadVertex, kVertexCount);
    uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, kIndexCount);

    float scale = 5.0f;

    vertices[0].position = Vector3(1, 0, 1) * scale;
    vertices[0].normal = Vector3::UP;
    vertices[0].uv = Vector2(1, 1);

    vertices[1].position = Vector3(1, 0, -1) * scale;
    vertices[1].normal = Vector3::UP;
    vertices[1].uv = Vector2(1, 0);

    vertices[2].position = Vector3(-1, 0, -1) * scale;
    vertices[2].normal = Vector3::UP;
    vertices[2].uv = Vector2(0, 0);

    vertices[3].position = Vector3(-1, 0, 1) * scale;
    vertices[3].normal = Vector3::UP;
    vertices[3].uv = Vector2(0, 1);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    Buffer vertexBuffer;
    vertexBuffer.Data = reinterpret_cast<uint8_t *>(vertices);
    vertexBuffer.DataSize = sizeof(QuadVertex) * kVertexCount;
    Vertices vertexBuffers(1);
    vertexBuffers[0] = vertexBuffer;

    VertexStrides strides(1, sizeof(QuadVertex));
    VertexOffsets offsets(1, 0);

    Buffer indexBuffer;
    indexBuffer.Data = reinterpret_cast<uint8_t *>(indices);
    indexBuffer.DataSize = sizeof(uint16_t) * kIndexCount;

    String name = SUB_MESH_NAME;
    SubMeshPtr submesh = SubMesh::create(name, materialUUID, PrimitiveType::kTriangleList, std::move(indexBuffer), true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Plane", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}

void DynamicBatchApp::buildPlane(Transform3D *parent)
{
    static int index = 0;
    std::stringstream ss;
    ss << "Plane#" << index;

    GameObjectPtr go = GameObject::create(ss.str());
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);
    xform->setPosition(Vector3::ZERO);
    xform->setScaling(Vector3(3.0f, 3.0f, 3.0f));

    SubMesh *submesh = mPlaneMesh->getSubMesh(SUB_MESH_NAME);

    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mPlaneMesh, submesh);
}
