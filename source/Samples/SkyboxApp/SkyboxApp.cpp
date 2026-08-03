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

#include "SkyboxApp.h"


#define ARCHIVE_TYPE_BUNDLE         "BundleFileSystem"
#define ARCHIVE_TYPE_FS             "FileSystem"
#define ARCHIVE_TYPE_ANDROID        "AndroidAsset"

using namespace Tiny3D;


static const char *SUB_MESH_NAME = "#0";


SkyboxApp theApp;

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


SkyboxApp::SkyboxApp()
{
}

SkyboxApp::~SkyboxApp()
{
}

TResult SkyboxApp::go(int32_t argc, char *argv[])
{
    TResult ret;

    Agent *theEngine = T3D_NEW Agent();
    ret = theEngine->init(argc, argv, true, true);
    if (ret == T3D_OK)
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}

TResult SkyboxApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("SkyboxScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    // add ambient light to the root of scene
    AmbientLightPtr ambient = scene->getRootGameObject()->addComponent<AmbientLight>();
    // 环境光压低，把明暗交给方向光，立方体各个面才有区分度，不会糊成一片白
    ambient->setColor(ColorRGB::WHITE);
    ambient->setIntensity(0.3f);

    // root game object
    GameObjectPtr go = GameObject::create("SkyboxScene");
    Transform3DPtr root = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);

    // directional light
    go = GameObject::create("DirectionLight");
    Transform3DPtr node = go->addComponent<Transform3D>();
    Vector3 eye(-2.0f, 2.0f, -4.0f);
    Matrix3 matR;
    matR.lookAt_LH(eye, Vector3::ZERO, Vector3::UP);
    node->setOrientation(Quaternion(matR));
    node->setPosition(eye);
    root->addChild(node);
    DirectionalLightPtr light = go->addComponent<DirectionalLight>();
    light->setColor(ColorRGB::WHITE);
    light->setDiffuseIntensity(1.0f);
    light->setSpecularIntensity(1.0f);

    // camera
    buildCamera(root);

    // runtime 模式：挂载只读 bundle 到资源门面搜索链（主包优先级 0）
    // 与 ResourceApp 共用同一个 bundle，天空盒的 shader / 材质 / cubemap 都在里面
    const String bundlePath = Dir::getResourcePath("assets/samples/bundle");
    ArchivePtr bundle = T3D_ARCHIVE_MGR.loadArchive(bundlePath, ARCHIVE_TYPE_BUNDLE, Archive::AccessMode::kRead);
    T3D_ASSERT(bundle != nullptr);
    T3D_ASSET_MGR.init(AssetManager::Mode::kRuntime);
    T3D_ASSET_MGR.mount(bundle, 0);

    // skybox
    buildSkybox(scene);

    // cube + 地面：共用 ShadowApp 同款 lit/shadow shader，平面承接投影
    ShaderPtr shader = buildShader();
    mCubeMaterial = buildCubeMaterial(shader);
    mPlaneMaterial = buildPlaneMaterial(shader);
    mCubeMesh = buildCubeMesh(mCubeMaterial->getUUID());
    mPlaneMesh = buildPlaneMesh(mPlaneMaterial->getUUID());
    buildCube(root);
    buildPlane(root);

    APP_LOG_DEBUG("Scene hierarchy:");
    scene->printSceneHierarchy(true);

    return T3D_OK;
}

void SkyboxApp::applicationWillTerminate()
{
    if (mOrbitTimerID != T3D_INVALID_TIMER_ID)
    {
        T3D_TIMER_MGR.stopTimer(mOrbitTimerID);
        mOrbitTimerID = T3D_INVALID_TIMER_ID;
    }

    mCubeMesh = nullptr;
    mPlaneMesh = nullptr;
    mCubeMaterial = nullptr;
    mPlaneMaterial = nullptr;
}

void SkyboxApp::buildCamera(Transform3D *parent)
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
    // 背景交给天空盒；万一材质加载失败，引擎会自动回退成这个纯色
    camera->setClearFlags(Camera::ClearFlags::kSkybox);
    camera->setClearColor(ColorRGB::BLACK);
    camera->setRenderTarget(rt);

    // camera for perspective
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);

    // 注视点取立方体中心高度（搁在地面上时中心 y=3）；相机略抬高一点俯视，
    // 方便同时看到天空盒、立方体和地上的投影
    const Real radius = 30.0f;
    const Real lookHeight = Real(3.0f);
    const Real camHeight = Real(8.0f);
    const Vector3 target(0.0f, lookHeight, 0.0f);
    camera->lookAt(Vector3(0.0f, camHeight, -radius), target, Vector3::UP);

    // 让相机绕着物体转，用来确认天空盒跟着视角走而不是贴在屏幕上
    Camera *rawCamera = camera;
    mOrbitTimerID = T3D_TIMER_MGR.startTimer(32, true,
        [this, rawCamera, radius, camHeight, target](ID timerID, uint32_t dt)
        {
            mCameraYaw += Real(0.006f);
            const Radian yaw(mCameraYaw);
            const Vector3 eye(
                Math::sin(yaw) * radius,
                camHeight,
                -Math::cos(yaw) * radius);
            rawCamera->lookAt(eye, target, Vector3::UP);
        });

    // construct frustum bound
    auto frustum = go->addComponent<FrustumBound>();
    T3D_ASSERT(frustum != nullptr);
}

void SkyboxApp::buildSkybox(Scene *scene)
{
    MaterialPtr skyboxMaterial = T3D_ASSET_MGR.loadMaterial("Skybox-Cubemap.tmat");
    if (skyboxMaterial == nullptr)
    {
        APP_LOG_DEBUG("Failed to load skybox material, fallback to solid color background !");
        return;
    }

    // 挂在场景根节点上就是全局天空盒，所有相机共用
    SkyboxPtr skybox = scene->getRootGameObject()->addComponent<Skybox>();
    T3D_ASSERT(skybox != nullptr);
    skybox->setMaterial(skyboxMaterial);
}

void SkyboxApp::buildCube(Transform3D *parent)
{
    GameObjectPtr go = GameObject::create("Cube");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);

    // 单位立方体 ±0.5，放大到 6 米见方；中心抬到 3 正好搁在 y=0 的地面上
    const Real cubeScale = Real(6.0f);
    xform->setPosition(Vector3(0.0f, cubeScale * Real(0.5f), 0.0f));
    xform->setScaling(Vector3(cubeScale, cubeScale, cubeScale));
    xform->setOrientation(Quaternion(Radian(Math::PI / 6.0f), Vector3::UNIT_Y));

    SubMesh *submesh = mCubeMesh->getSubMesh(SUB_MESH_NAME);

    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mCubeMesh, submesh);

    AabbBoundPtr bound = go->addComponent<AabbBound>();
    buildAabb(mCubeMesh, submesh, bound);
}

void SkyboxApp::buildPlane(Transform3D *parent)
{
    GameObjectPtr go = GameObject::create("Plane");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);
    xform->setPosition(Vector3::ZERO);
    // 网格本身 ±5，再放大一圈，保证相机 30 米外也能看见投影落点
    xform->setScaling(Vector3(4.0f, 4.0f, 4.0f));

    SubMesh *submesh = mPlaneMesh->getSubMesh(SUB_MESH_NAME);

    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mPlaneMesh, submesh);

    AabbBoundPtr bound = go->addComponent<AabbBound>();
    buildAabb(mPlaneMesh, submesh, bound);
}

PassPtr SkyboxApp::buildShadowPass()
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
    else  // DirectX/HLSL
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
    else  // OpenGL ES
    {
        vsCode = SHADOW_VERTEX_SHADER_GLES;
        vsSize = strlen(SHADOW_VERTEX_SHADER_GLES);
    }
#elif defined(T3D_OS_IOS)
    // TODO: Metal or GLES
#elif defined(T3D_OS_OSX)
    // TODO: Metal or OpenGL4
#elif defined(T3D_OS_LINUX)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        vsSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else  // OpenGL4
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

PassPtr SkyboxApp::buildForwardPass()
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
    else  // DirectX/HLSL
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
    else  // OpenGL ES
    {
        vsCode = FORWARD_VERTEX_SHADER_GLES;
        vsSize = strlen(FORWARD_VERTEX_SHADER_GLES);
    }
#elif defined(T3D_OS_IOS)
    // TODO: Metal or GLES
#elif defined(T3D_OS_OSX)
    // TODO: Metal or OpenGL4
#elif defined(T3D_OS_LINUX)
    if (rendererName == RHIRenderer::VULKAN)
    {
        vsCode = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        vsSize = FORWARD_VERTEX_SHADER_VK_SIZE;
    }
    else  // OpenGL4
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
    else  // DirectX/HLSL
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
    else  // OpenGL ES
    {
        psCode = FORWARD_PIXEL_SHADER_GLES;
        psSize = strlen(FORWARD_PIXEL_SHADER_GLES);
    }
#elif defined(T3D_OS_IOS)
    // TODO: Metal or GLES
#elif defined(T3D_OS_OSX)
    // TODO: Metal or OpenGL4
#elif defined(T3D_OS_LINUX)
    if (rendererName == RHIRenderer::VULKAN)
    {
        psCode = reinterpret_cast<const char*>(FORWARD_PIXEL_SHADER_VK);
        psSize = FORWARD_PIXEL_SHADER_VK_SIZE;
    }
    else  // OpenGL4
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

ShaderPtr SkyboxApp::buildShader()
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

void SkyboxApp::setupLitMaterialParams(Material *material)
{
    // 这里只是设置材质有该项变量，具体值由引擎在渲染时动态填充
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
}

Texture2DPtr SkyboxApp::buildCubeTexture()
{
    // bundle 里已有与 TextureApp 同源的 blocks atlas；优先走已挂载的资源门面
    TexturePtr loaded = T3D_ASSET_MGR.loadTexture("blocks.ttex");
    if (loaded != nullptr)
    {
        Texture2DPtr texture = smart_pointer_cast<Texture2D>(loaded);
        if (texture != nullptr)
        {
            SamplerDesc samplerDesc;
            texture->setSamplerDesc(samplerDesc);
            return texture;
        }
    }

#if defined(T3D_OS_ANDROID)
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_ANDROID, "", Archive::AccessMode::kRead);
#else
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_FS, Dir::getAppPath(), Archive::AccessMode::kRead);
#endif
    T3D_ASSERT(archive != nullptr);
    T3D_ASSET_MGR.mount(archive, 1);

    ImagePtr image = T3D_ASSET_MGR.loadImage("assets/samples/textures/blocks.png");
    T3D_ASSERT(image != nullptr);
    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("textureCube", image);
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    return texture;
}

MaterialPtr SkyboxApp::buildCubeMaterial(Shader *shader)
{
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildCubeTexture();

    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Cube-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());
    setupLitMaterialParams(material);
    return material;
}

Texture2DPtr SkyboxApp::buildPlaneTexture()
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
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    return texture;
}

MaterialPtr SkyboxApp::buildPlaneMaterial(Shader *shader)
{
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildPlaneTexture();

    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Plane-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());
    setupLitMaterialParams(material);
    return material;
}

MeshPtr SkyboxApp::buildPlaneMesh(const Tiny3D::UUID &materialUUID)
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

    const float scale = 5.0f;

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
    vertexBuffer.Data = (uint8_t*)vertices;
    vertexBuffer.DataSize = sizeof(QuadVertex) * kVertexCount;
    Vertices vertexBuffers(1);
    vertexBuffers[0] = vertexBuffer;

    VertexStrides strides(1, sizeof(QuadVertex));
    VertexOffsets offsets(1, 0);

    Buffer indexBuffer;
    indexBuffer.Data = (uint8_t*)indices;
    indexBuffer.DataSize = sizeof(uint16_t) * kIndexCount;

    String name = SUB_MESH_NAME;
    SubMeshPtr submesh = SubMesh::create(name, materialUUID, PrimitiveType::kTriangleList, std::move(indexBuffer), true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    return T3D_MESH_MGR.createMesh("Plane", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
}

MeshPtr SkyboxApp::buildCubeMesh(const Tiny3D::UUID &materialUUID)
{
    //
    // 正方体顶点：layout 与 ShadowApp 一致（position + normal + uv），
    // UV 仍用 TextureApp 的 blocks atlas 切分。
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

    // front - V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[0].position = center + offset;
    vertices[0].normal = -Vector3::FORWARD;
    vertices[0].uv = Vector2(0.0f, 0.0f);

    // front - V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[1].position = center + offset;
    vertices[1].normal = -Vector3::FORWARD;
    vertices[1].uv = Vector2(0.0f, 0.5f);

    // front - V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[2].position = center + offset;
    vertices[2].normal = -Vector3::FORWARD;
    vertices[2].uv = Vector2(0.5f, 0.0f);

    // front - V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[3].position = center + offset;
    vertices[3].normal = -Vector3::FORWARD;
    vertices[3].uv = Vector2(0.5f, 0.5f);

    // right - V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[4].position = center + offset;
    vertices[4].normal = Vector3::RIGHT;
    vertices[4].uv = Vector2(0.0f, 0.5f);

    // right - V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[5].position = center + offset;
    vertices[5].normal = Vector3::RIGHT;
    vertices[5].uv = Vector2(0.0f, 1.0f);

    // right - V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[6].position = center + offset;
    vertices[6].normal = Vector3::RIGHT;
    vertices[6].uv = Vector2(0.5f, 0.5f);

    // right - V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[7].position = center + offset;
    vertices[7].normal = Vector3::RIGHT;
    vertices[7].uv = Vector2(0.5f, 1.0f);

    // back - V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[8].position = center + offset;
    vertices[8].normal = Vector3::FORWARD;
    vertices[8].uv = Vector2(0.0f, 0.0f);

    // back - V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[9].position = center + offset;
    vertices[9].normal = Vector3::FORWARD;
    vertices[9].uv = Vector2(0.0f, 0.5f);

    // back - V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[10].position = center + offset;
    vertices[10].normal = Vector3::FORWARD;
    vertices[10].uv = Vector2(0.5f, 0.0f);

    // back - V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[11].position = center + offset;
    vertices[11].normal = Vector3::FORWARD;
    vertices[11].uv = Vector2(0.5f, 0.5f);

    // left - V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[12].position = center + offset;
    vertices[12].normal = -Vector3::RIGHT;
    vertices[12].uv = Vector2(0.5f, 0.5f);

    // left - V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[13].position = center + offset;
    vertices[13].normal = -Vector3::RIGHT;
    vertices[13].uv = Vector2(0.5f, 1.0f);

    // left - V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[14].position = center + offset;
    vertices[14].normal = -Vector3::RIGHT;
    vertices[14].uv = Vector2(1.0f, 0.5f);

    // left - V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[15].position = center + offset;
    vertices[15].normal = -Vector3::RIGHT;
    vertices[15].uv = Vector2(1.0f, 1.0f);

    // top - V0
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[16].position = center + offset;
    vertices[16].normal = Vector3::UP;
    vertices[16].uv = Vector2(0.5f, 0.5f);

    // top - V2
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = -extent[2];
    vertices[17].position = center + offset;
    vertices[17].normal = Vector3::UP;
    vertices[17].uv = Vector2(1.0f, 0.5f);

    // top - V4
    offset[0] = extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[18].position = center + offset;
    vertices[18].normal = Vector3::UP;
    vertices[18].uv = Vector2(1.0f, 0.0f);

    // top - V6
    offset[0] = -extent[0];
    offset[1] = extent[1];
    offset[2] = extent[2];
    vertices[19].position = center + offset;
    vertices[19].normal = Vector3::UP;
    vertices[19].uv = Vector2(0.5f, 0.0f);

    // bottom - V1
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[20].position = center + offset;
    vertices[20].normal = -Vector3::UP;
    vertices[20].uv = Vector2(0.5f, 0.5f);

    // bottom - V7
    offset[0] = -extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[21].position = center + offset;
    vertices[21].normal = -Vector3::UP;
    vertices[21].uv = Vector2(0.5f, 1.0f);

    // bottom - V3
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = -extent[2];
    vertices[22].position = center + offset;
    vertices[22].normal = -Vector3::UP;
    vertices[22].uv = Vector2(1.0f, 0.5f);

    // bottom - V5
    offset[0] = extent[0];
    offset[1] = -extent[1];
    offset[2] = extent[2];
    vertices[23].position = center + offset;
    vertices[23].normal = -Vector3::UP;
    vertices[23].uv = Vector2(1.0f, 1.0f);

    // Front
    indices[0] = 0; indices[1] = 2; indices[2] = 1;
    indices[3] = 1; indices[4] = 2; indices[5] = 3;

    // Right
    indices[6] = 4; indices[7] = 6; indices[8] = 5;
    indices[9] = 5; indices[10] = 6; indices[11] = 7;

    // Back
    indices[12] = 8; indices[13] = 10; indices[14] = 9;
    indices[15] = 9; indices[16] = 10; indices[17] = 11;

    // Left
    indices[18] = 12; indices[19] = 14; indices[20] = 13;
    indices[21] = 13; indices[22] = 14; indices[23] = 15;

    // Top
    indices[24] = 16; indices[25] = 19; indices[26] = 18;
    indices[27] = 18; indices[28] = 17; indices[29] = 16;

    // Bottom
    indices[30] = 20; indices[31] = 22; indices[32] = 21;
    indices[33] = 21; indices[34] = 22; indices[35] = 23;

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
    SubMeshPtr submesh = SubMesh::create(name, materialUUID, PrimitiveType::kTriangleList, std::move(indexBuffer), true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}

void SkyboxApp::buildAabb(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
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
