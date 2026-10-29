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

#include "SkeletalAnimationApp.h"

#define UVN_CAMERA

#define USE_GPU_SKIN

using namespace Tiny3D;

const char *SUB_MESH_NAME = "#0";

const float kOneBoneLength = 4.0f;
const char *kArmSkeleton = "ArmSkeleton";
const char *kUpperArmName = "UpperArm";
const char *kForeArmName = "ForeArm";
const char *kPalmName = "Palm";
const char *kAnimationArm = "Arm";
const char *kArmRotateLeft = "RotateLeft";
const char *kArmRotateRight = "RotateRight";

const char *DEFINE_GPU_SKIN = "T3D_GPU_SKIN";

SkeletalAnimationApp theApp;

#if defined(T3D_OS_WINDOWS)
// extern const char *SAMPLE_LIT_VERTEX_SHADER;
// extern const char *SAMPLE_LIT_PIXEL_SHADER;
extern const char *FORWARD_VERTEX_SHADER;
extern const char *SHADOW_VERTEX_SHADER;
extern const char *SKIN_SHADOW_VERTEX_SHADER;
extern const char *SKIN_FORWARD_VERTEX_SHADER;
extern const char *FORWARD_PIXEL_SHADER;
extern const char *GPU_SKIN_SHADOW_VERTEX_SHADER;
extern const char *GPU_SKIN_FORWARD_VERTEX_SHADER;
extern const char *FORWARD_VERTEX_SHADER_GL;
extern const char *SHADOW_VERTEX_SHADER_GL;
extern const char *SKIN_SHADOW_VERTEX_SHADER_GL;
extern const char *SKIN_FORWARD_VERTEX_SHADER_GL;
extern const char *FORWARD_PIXEL_SHADER_GL;
extern const char *GPU_SKIN_SHADOW_VERTEX_SHADER_GL;
extern const char *GPU_SKIN_FORWARD_VERTEX_SHADER_GL;
#endif
#if defined(T3D_OS_WINDOWS) || defined(T3D_OS_ANDROID)
extern const unsigned char FORWARD_VERTEX_SHADER_VK[];
extern const size_t FORWARD_VERTEX_SHADER_VK_SIZE;
extern const unsigned char SHADOW_VERTEX_SHADER_VK[];
extern const size_t SHADOW_VERTEX_SHADER_VK_SIZE;
extern const unsigned char SKIN_SHADOW_VERTEX_SHADER_VK[];
extern const size_t SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
extern const unsigned char SKIN_FORWARD_VERTEX_SHADER_VK[];
extern const size_t SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
extern const unsigned char FORWARD_PIXEL_SHADER_VK[];
extern const size_t FORWARD_PIXEL_SHADER_VK_SIZE;
extern const unsigned char GPU_SKIN_SHADOW_VERTEX_SHADER_VK[];
extern const size_t GPU_SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
extern const unsigned char GPU_SKIN_FORWARD_VERTEX_SHADER_VK[];
extern const size_t GPU_SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
#endif
#if defined(T3D_OS_ANDROID)
extern const char *FORWARD_VERTEX_SHADER_GLES;
extern const char *SHADOW_VERTEX_SHADER_GLES;
extern const char *SKIN_SHADOW_VERTEX_SHADER_GLES;
extern const char *SKIN_FORWARD_VERTEX_SHADER_GLES;
extern const char *FORWARD_PIXEL_SHADER_GLES;
extern const char *GPU_SKIN_SHADOW_VERTEX_SHADER_GLES;
extern const char *GPU_SKIN_FORWARD_VERTEX_SHADER_GLES;
#endif

SkeletalAnimationApp::SkeletalAnimationApp()
{
}

SkeletalAnimationApp::~SkeletalAnimationApp()
{
}

TResult SkeletalAnimationApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    // add ambient light to the root of scene
    AmbientLightPtr ambient = scene->getRootGameObject()->addComponent<AmbientLight>();
    ambient->setColor(ColorRGB::WHITE);
    ambient->setIntensity(0.5f);
    
    // root game object
    GameObjectPtr go = GameObject::create("TestScene");
    // scene->addRootGameObject(go);
    Transform3DPtr root = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);

    // directional light
    go = GameObject::create("DirectionLight");
    Transform3DPtr node = go->addComponent<Transform3D>();
    // Quaternion q(Radian(Math::PI * 0.5f), Vector3::UNIT_X);
    Vector3 eye(-2.0f, 2.0f, -4.0f);
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

    // cube shader & material
    const String rendererName = T3D_AGENT.getActiveRHIRenderer()->getName();
    bool isVK = (rendererName == RHIRenderer::VULKAN);
#if defined(T3D_OS_ANDROID)
    bool isGLES = true;  // Android: non-VK uses GLES
    bool isGL = false;
#elif defined(T3D_OS_WINDOWS) || defined(T3D_OS_LINUX)
    bool isGLES = false;
    bool isGL = (rendererName == RHIRenderer::OPENGL4);
#else
    bool isGLES = false;
    bool isGL = false;
#endif

    const char *skinForwardVS = nullptr;
    size_t skinForwardVSSize = 0;
    const char *skinShadowVS = nullptr;
    size_t skinShadowVSSize = 0;
    const char *gpuSkinForwardVS = nullptr;
    size_t gpuSkinForwardVSSize = 0;
    const char *gpuSkinShadowVS = nullptr;
    size_t gpuSkinShadowVSSize = 0;

#if defined(T3D_OS_WINDOWS)
    if (isVK)
    {
        skinForwardVS = reinterpret_cast<const char*>(SKIN_FORWARD_VERTEX_SHADER_VK);
        skinForwardVSSize = SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
        skinShadowVS = reinterpret_cast<const char*>(SKIN_SHADOW_VERTEX_SHADER_VK);
        skinShadowVSSize = SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
        gpuSkinForwardVS = reinterpret_cast<const char*>(GPU_SKIN_FORWARD_VERTEX_SHADER_VK);
        gpuSkinForwardVSSize = GPU_SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
        gpuSkinShadowVS = reinterpret_cast<const char*>(GPU_SKIN_SHADOW_VERTEX_SHADER_VK);
        gpuSkinShadowVSSize = GPU_SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else if (isGL)
    {
        skinForwardVS = SKIN_FORWARD_VERTEX_SHADER_GL;
        skinForwardVSSize = strlen(SKIN_FORWARD_VERTEX_SHADER_GL);
        skinShadowVS = SKIN_SHADOW_VERTEX_SHADER_GL;
        skinShadowVSSize = strlen(SKIN_SHADOW_VERTEX_SHADER_GL);
        gpuSkinForwardVS = GPU_SKIN_FORWARD_VERTEX_SHADER_GL;
        gpuSkinForwardVSSize = strlen(GPU_SKIN_FORWARD_VERTEX_SHADER_GL);
        gpuSkinShadowVS = GPU_SKIN_SHADOW_VERTEX_SHADER_GL;
        gpuSkinShadowVSSize = strlen(GPU_SKIN_SHADOW_VERTEX_SHADER_GL);
    }
    else  // DirectX/HLSL
    {
        skinForwardVS = SKIN_FORWARD_VERTEX_SHADER;
        skinForwardVSSize = strlen(SKIN_FORWARD_VERTEX_SHADER);
        skinShadowVS = SKIN_SHADOW_VERTEX_SHADER;
        skinShadowVSSize = strlen(SKIN_SHADOW_VERTEX_SHADER);
        gpuSkinForwardVS = GPU_SKIN_FORWARD_VERTEX_SHADER;
        gpuSkinForwardVSSize = strlen(GPU_SKIN_FORWARD_VERTEX_SHADER);
        gpuSkinShadowVS = GPU_SKIN_SHADOW_VERTEX_SHADER;
        gpuSkinShadowVSSize = strlen(GPU_SKIN_SHADOW_VERTEX_SHADER);
    }
#elif defined(T3D_OS_ANDROID)
    if (isVK)
    {
        skinForwardVS = reinterpret_cast<const char*>(SKIN_FORWARD_VERTEX_SHADER_VK);
        skinForwardVSSize = SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
        skinShadowVS = reinterpret_cast<const char*>(SKIN_SHADOW_VERTEX_SHADER_VK);
        skinShadowVSSize = SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
        gpuSkinForwardVS = reinterpret_cast<const char*>(GPU_SKIN_FORWARD_VERTEX_SHADER_VK);
        gpuSkinForwardVSSize = GPU_SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
        gpuSkinShadowVS = reinterpret_cast<const char*>(GPU_SKIN_SHADOW_VERTEX_SHADER_VK);
        gpuSkinShadowVSSize = GPU_SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else  // OpenGL ES
    {
        skinForwardVS = SKIN_FORWARD_VERTEX_SHADER_GLES;
        skinForwardVSSize = strlen(SKIN_FORWARD_VERTEX_SHADER_GLES);
        skinShadowVS = SKIN_SHADOW_VERTEX_SHADER_GLES;
        skinShadowVSSize = strlen(SKIN_SHADOW_VERTEX_SHADER_GLES);
        gpuSkinForwardVS = GPU_SKIN_FORWARD_VERTEX_SHADER_GLES;
        gpuSkinForwardVSSize = strlen(GPU_SKIN_FORWARD_VERTEX_SHADER_GLES);
        gpuSkinShadowVS = GPU_SKIN_SHADOW_VERTEX_SHADER_GLES;
        gpuSkinShadowVSSize = strlen(GPU_SKIN_SHADOW_VERTEX_SHADER_GLES);
    }
#elif defined(T3D_OS_IOS)
    // TODO: Metal or GLES
#elif defined(T3D_OS_OSX)
    // TODO: Metal or OpenGL4
#elif defined(T3D_OS_LINUX)
    if (isVK)
    {
        skinForwardVS = reinterpret_cast<const char*>(SKIN_FORWARD_VERTEX_SHADER_VK);
        skinForwardVSSize = SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
        skinShadowVS = reinterpret_cast<const char*>(SKIN_SHADOW_VERTEX_SHADER_VK);
        skinShadowVSSize = SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
        gpuSkinForwardVS = reinterpret_cast<const char*>(GPU_SKIN_FORWARD_VERTEX_SHADER_VK);
        gpuSkinForwardVSSize = GPU_SKIN_FORWARD_VERTEX_SHADER_VK_SIZE;
        gpuSkinShadowVS = reinterpret_cast<const char*>(GPU_SKIN_SHADOW_VERTEX_SHADER_VK);
        gpuSkinShadowVSSize = GPU_SKIN_SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else  // OpenGL4
    {
        skinForwardVS = SKIN_FORWARD_VERTEX_SHADER_GL;
        skinForwardVSSize = strlen(SKIN_FORWARD_VERTEX_SHADER_GL);
        skinShadowVS = SKIN_SHADOW_VERTEX_SHADER_GL;
        skinShadowVSSize = strlen(SKIN_SHADOW_VERTEX_SHADER_GL);
        gpuSkinForwardVS = GPU_SKIN_FORWARD_VERTEX_SHADER_GL;
        gpuSkinForwardVSSize = strlen(GPU_SKIN_FORWARD_VERTEX_SHADER_GL);
        gpuSkinShadowVS = GPU_SKIN_SHADOW_VERTEX_SHADER_GL;
        gpuSkinShadowVSSize = strlen(GPU_SKIN_SHADOW_VERTEX_SHADER_GL);
    }
#endif

    ShaderPtr shader = buildShader("Cube-Shader",
        skinForwardVS, skinForwardVSSize,
        skinShadowVS, skinShadowVSSize,
        gpuSkinForwardVS, gpuSkinForwardVSSize,
        gpuSkinShadowVS, gpuSkinShadowVSSize);
    mCubeMaterial = buildArmMaterial(shader);
    
    // plane shader & material
    const char *planeForwardVS = nullptr;
    size_t planeForwardVSSize = 0;
    const char *planeShadowVS = nullptr;
    size_t planeShadowVSSize = 0;

#if defined(T3D_OS_WINDOWS)
    if (isVK)
    {
        planeForwardVS = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        planeForwardVSSize = FORWARD_VERTEX_SHADER_VK_SIZE;
        planeShadowVS = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        planeShadowVSSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else if (isGL)
    {
        planeForwardVS = FORWARD_VERTEX_SHADER_GL;
        planeForwardVSSize = strlen(FORWARD_VERTEX_SHADER_GL);
        planeShadowVS = SHADOW_VERTEX_SHADER_GL;
        planeShadowVSSize = strlen(SHADOW_VERTEX_SHADER_GL);
    }
    else  // DirectX/HLSL
    {
        planeForwardVS = FORWARD_VERTEX_SHADER;
        planeForwardVSSize = strlen(FORWARD_VERTEX_SHADER);
        planeShadowVS = SHADOW_VERTEX_SHADER;
        planeShadowVSSize = strlen(SHADOW_VERTEX_SHADER);
    }
#elif defined(T3D_OS_ANDROID)
    if (isVK)
    {
        planeForwardVS = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        planeForwardVSSize = FORWARD_VERTEX_SHADER_VK_SIZE;
        planeShadowVS = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        planeShadowVSSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else  // OpenGL ES
    {
        planeForwardVS = FORWARD_VERTEX_SHADER_GLES;
        planeForwardVSSize = strlen(FORWARD_VERTEX_SHADER_GLES);
        planeShadowVS = SHADOW_VERTEX_SHADER_GLES;
        planeShadowVSSize = strlen(SHADOW_VERTEX_SHADER_GLES);
    }
#elif defined(T3D_OS_IOS)
    // TODO: Metal or GLES
#elif defined(T3D_OS_OSX)
    // TODO: Metal or OpenGL4
#elif defined(T3D_OS_LINUX)
    if (isVK)
    {
        planeForwardVS = reinterpret_cast<const char*>(FORWARD_VERTEX_SHADER_VK);
        planeForwardVSSize = FORWARD_VERTEX_SHADER_VK_SIZE;
        planeShadowVS = reinterpret_cast<const char*>(SHADOW_VERTEX_SHADER_VK);
        planeShadowVSSize = SHADOW_VERTEX_SHADER_VK_SIZE;
    }
    else  // OpenGL4
    {
        planeForwardVS = FORWARD_VERTEX_SHADER_GL;
        planeForwardVSSize = strlen(FORWARD_VERTEX_SHADER_GL);
        planeShadowVS = SHADOW_VERTEX_SHADER_GL;
        planeShadowVSSize = strlen(SHADOW_VERTEX_SHADER_GL);
    }
#endif

    shader = buildShader("Plane-Shader",
        planeForwardVS, planeForwardVSSize,
        planeShadowVS, planeShadowVSSize,
        gpuSkinShadowVS, gpuSkinShadowVSSize,
        gpuSkinShadowVS, gpuSkinShadowVSSize);
    mPlaneMaterial = buildPlaneMaterial(shader);

    // cube mesh
    mCubeMesh = buildArmMesh(mCubeMaterial->getUUID());

    // plane mesh
    mPlaneMesh = buildPlaneMesh(mPlaneMaterial->getUUID());
    
    // camera
    buildCamera(root);

    // arm
    buildArm(root, Vector3(0, 0, 0), Radian(0.0f));

    // plane
    buildPlane(root);
    
    scene->printSceneHierarchy(true);
    
    return T3D_OK;
}

void SkeletalAnimationApp::applicationWillTerminate() 
{
    if (mPauseResumeTimerID != T3D_INVALID_ID)
    {
        T3D_TIMER_MGR.stopTimer(mPauseResumeTimerID);
        mPauseResumeTimerID = T3D_INVALID_ID;
    }
    mArmGeometry = nullptr;
    mCubeMesh = nullptr;
    mPlaneMesh = nullptr;
    mCubeMaterial = nullptr;
    mPlaneMaterial = nullptr;
}

void SkeletalAnimationApp::buildCamera(Transform3D *parent)
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
    camera->setClearColor(ColorRGB::BLACK);
    camera->setRenderTarget(rt);
    
    // camera for perspective
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    // camera->setFovY(Radian(Math::PI * 0.5f));
    Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);
    
    // construct camera position & orientation & scaling

    Vector3 eye(8.0f, 20.0f, -20.0f);

#if defined (UVN_CAMERA)
    Vector3 obj(0.0f, 0.0f, 0.0f);
    camera->lookAt(eye, obj, Vector3::UP);
#else
    xform->setPosition(eye);
    Radian xAngle(Degree(25.0f).valueRadians());
    Radian yAngle(-Math::PI * 0.25f);
    // Radian yAngle(0.0f);
    Radian zAngle(0.0f);
    xform->rotate(xAngle, yAngle, zAngle);
#endif

    // construct frustum bound
    auto frustum = go->addComponent<FrustumBound>();
    T3D_ASSERT(frustum != nullptr);
}

PassPtr SkeletalAnimationApp::buildShadowPass(const char *vs, size_t vsSize, const char *vs4GPU, size_t vs4GPUSize)
{
    const SHADER_LANGUAGE shadingLang = T3D_AGENT.getActiveRHIRenderer()->getShadingLanguage();

    // CPU_SKIN keyword for shadow pass
    ShaderKeyword keyCPUSkin;
    keyCPUSkin.addKeyword("");
    keyCPUSkin.generate();

    // vertex shader for shadow pass
    ShaderVariantPtr vshader = ShaderVariant::create(std::move(keyCPUSkin), vs, vsSize);
    vshader->setShaderStage(SHADER_STAGE::kVertex);
    vshader->setLanguage(shadingLang);

    // GPU_SKIN keyword for shadow pass
    ShaderKeyword keyGPUSkin;
    keyGPUSkin.addKeyword(DEFINE_GPU_SKIN);
    keyGPUSkin.generate();

    // GPU skin vertex shader for shadow pass
    ShaderVariantPtr vshader4GPU = ShaderVariant::create(std::move(keyGPUSkin), vs4GPU, vs4GPUSize);
    vshader4GPU->setShaderStage(SHADER_STAGE::kVertex);
    vshader4GPU->setLanguage(shadingLang);

    // shadow pass
    PassPtr pass = Pass::create("ShadowCaster");
    TResult ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    ret = pass->addShaderVariant(vshader4GPU->getShaderKeyword(), vshader4GPU);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeShadowCasterStr);

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
    rasterizeDesc.DepthBias = 50.0f;
    rasterizeDesc.SlopeScaledDepthBias = 2.5f;
    renderState->setRasterizerDesc(rasterizeDesc);
    pass->setRenderState(renderState);

    return pass;
}

PassPtr SkeletalAnimationApp::buildForwardPass(const char *vs, size_t vsSize, const char *vs4GPU, size_t vs4GPUSize)
{
    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
    const SHADER_LANGUAGE shadingLang = renderer->getShadingLanguage();

    // vertex & pixel shader keyword for forward pass
    ShaderKeyword keyCPUSkin;
    keyCPUSkin.addKeyword("");
    keyCPUSkin.generate();
    ShaderKeyword pkeyword(keyCPUSkin);
    
    // vertex shader for forward pass 
    ShaderVariantPtr vshader = ShaderVariant::create(std::move(keyCPUSkin), vs, vsSize);
    vshader->setShaderStage(SHADER_STAGE::kVertex);
    vshader->setLanguage(shadingLang);

    // GPU_SKIN keyword for forward pass
    ShaderKeyword keyGPUSkin;
    keyGPUSkin.addKeyword(DEFINE_GPU_SKIN);
    keyGPUSkin.generate();

    // GPU skin vertex shader for forward pass
    ShaderVariantPtr vshader4GPU = ShaderVariant::create(std::move(keyGPUSkin), vs4GPU, vs4GPUSize);
    vshader4GPU->setShaderStage(SHADER_STAGE::kVertex);
    vshader4GPU->setLanguage(shadingLang);

    // pixel shader for forward pass
    const String rendererName = renderer->getName();
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

    // forward pass
    PassPtr pass = Pass::create("ForwardBase");
    TResult ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    pass->addShaderVariant(vshader4GPU->getShaderKeyword(), vshader4GPU);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    ret = pass->addShaderVariant(pshader->getShaderKeyword(), pshader);
    T3D_ASSERT(T3D_SUCCEEDED(ret));
    pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);

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
    pass->setRenderState(renderState);

    return pass;
}


ShaderPtr SkeletalAnimationApp::buildShader(const String &name, const char *forwardVS, size_t forwardVSSize, const char *shadowVS, size_t shadowVSSize, const char *forwardVS4GPU, size_t forwardVS4GPUSize, const char *shadowVS4GPU, size_t shadowVS4GPUSize)
{
    // shadow pass
    PassPtr shadowPass= buildShadowPass(shadowVS, shadowVSSize, shadowVS4GPU, shadowVS4GPUSize);

    // forward pass
    PassPtr forwardPass = buildForwardPass(forwardVS, forwardVSSize, forwardVS4GPU, forwardVS4GPUSize);

    // technique
    TechniquePtr tech = Technique::create("Default-Technique");
    bool rval = tech->addPass(forwardPass);
    T3D_ASSERT(rval);
    rval = tech->addPass(shadowPass);
    T3D_ASSERT(rval);
    tech->addTag(ShaderLab::kBuiltinTagQueue, ShaderLab::kBuiltinQueueGeometryStr);

    // shader 
    ShaderPtr shader = T3D_SHADER_MGR.createShader(name);
    rval = shader->addTechnique(tech);
    T3D_ASSERT(rval);

    return shader;
}


void SkeletalAnimationApp::buildArm(Transform3D *parent, const Vector3 &pos, const Radian &yAngles)
{
    static int index = 0;
    std::stringstream ss;
    ss << "Cube#" << index;
    
    // transform node for cube
    GameObjectPtr go = GameObject::createWithTransform(ss.str());
    Transform3D *xform = static_cast<Transform3D *>(go->getTransformNode());
    parent->addChild(xform);
    xform->setPosition(pos);
    xform->rotate(Radian::kZero, yAngles, Radian::kZero);

    // 将骨骼树根节点（UpperArm）挂到 Cube#0 下
    // Skeleton *skeleton = mCubeMesh->getSkeleton();
    // TransformNode *upperArmXform = skeleton->getRootBoneGameObject()->getTransformNode();
    // xform->addChild(upperArmXform);

    go = GameObject::createWithTransform("body");
    xform->addChild(go->getTransformNode());
    xform = static_cast<Transform3D *>(go->getTransformNode());
    xform->rotate(Degree::kZero, Degree(30.0f), Degree::kZero);

    // submesh
    SubMesh *submesh = mCubeMesh->getSubMesh(SUB_MESH_NAME);

    // geometry component
    SkinnedGeometryPtr geometry = go->addComponent<SkinnedGeometry>();
    geometry->setMeshObject(mCubeMesh, submesh);
    geometry->populateAllChildren();
    geometry->setDefaultClipName(kArmRotateLeft);
#if defined (USE_GPU_SKIN)
    const bool useGPUSkin = true;
#else
    const bool useGPUSkin = false;
#endif
    geometry->setGPUSkinning(useGPUSkin);

    // 验证 1：传入不存在的 clipName 应返回 INVALID_ID
    AnimationPlayerPtr player = geometry->getAnimationPlayer();
    T3D_ASSERT(player != nullptr);
    ID invalidID = player->playClip("NonExistentClip", false, true, useGPUSkin);
    T3D_ASSERT(invalidID == AnimationPlayer::INVALID_ID);
    T3D_LOG_INFO(LOG_TAG_APP, "[T0-4 Verify] playClip('NonExistentClip') returned ID=%u (expected INVALID_ID=%u)",
        invalidID, AnimationPlayer::INVALID_ID);

    // 验证 2：按名称播放第二个片段 kArmRotateRight 与第一个片段 kArmRotateLeft，返回递增正整数 ID
    ID firstID = player->playClip(kArmRotateRight, false, true, useGPUSkin);
    T3D_ASSERT(firstID != AnimationPlayer::INVALID_ID);
    // 验证 T1-2：多片段混合 playClips 与双片段混合 playClip(from, to, startMS) 接口返回有效 ID
    ID layeredInvalidID = player->playClips({ "NonExistentClip" }, useGPUSkin);
    T3D_ASSERT(layeredInvalidID == AnimationPlayer::INVALID_ID);
    ID layeredID = player->playClips({ kArmRotateLeft, kArmRotateRight }, useGPUSkin);
    T3D_ASSERT(layeredID > firstID);
    ID twoClipID = player->playClip(kArmRotateLeft, kArmRotateRight, 500, true, useGPUSkin);
    T3D_ASSERT(twoClipID > layeredID && player->isCrossFading());
    T3D_LOG_INFO(LOG_TAG_APP, "[T1-2 Verify] playClips ID=%u, playClip('%s'->'%s', start=500ms) ID=%u",
        layeredID, kArmRotateLeft, kArmRotateRight, twoClipID);

    bool playOK = geometry->play(kArmRotateLeft, true);
    ID secondID = geometry->getPlaybackID();
    T3D_ASSERT(playOK && secondID > twoClipID);
    T3D_LOG_INFO(LOG_TAG_APP, "[T0-4 Verify] playClip('%s') ID=%u, play('%s') ID=%u",
        kArmRotateRight, firstID, kArmRotateLeft, secondID);

    // 验证 3：250ms 高频采样定时器 —— 依次验证：
    //   (A) T0-4: pausePlayback -> 定格保持 -> resumePlayback
    //   (B) T1-2 对比实验 1（硬切 isBlending=false）：切换瞬间角度发生 ~28° 剧烈跳变（画面瞬间弹回竖直）
    //   (C) T1-2 对比实验 2（混合 isBlending=true, 1500ms）：切换瞬间 0° 跳变，打印每 250ms 的 From/To/Blended 角度与进度条
    //   (D) 持续循环交替演示 CrossFade 与硬切，方便在窗口中肉眼观察对比
    mArmGeometry = geometry;
    mTimerTickCount = 0;
    mPauseResumeTimerID = T3D_TIMER_MGR.startTimer(250, true, [this, useGPUSkin](ID timerID, int32_t dt)
    {
        if (mArmGeometry == nullptr)
        {
            return;
        }
        AnimationPlayerPtr animPlayer = mArmGeometry->getAnimationPlayer();
        ID playbackID = mArmGeometry->getPlaybackID();
        const BoneGameObjectsMap &bones = mArmGeometry->getAllBones();
        auto palmIt = bones.find(kPalmName);
        Transform3D *palmXform = (palmIt != bones.end())
            ? static_cast<Transform3D *>(palmIt->second->getTransformNode())
            : nullptr;
        if (palmXform == nullptr)
        {
            return;
        }

        // 辅助函数：从绕 Z 轴旋转的四元数提取带符号角度（度，范围 [-45°, +45°]）
        auto quatToZDeg = [](const Quaternion &q) -> float
        {
            float w = (q.w() < 0.0f) ? -q.w() : q.w();
            float z = (q.w() < 0.0f) ? -q.z() : q.z();
            return 2.0f * std::atan2(z, w) * (180.0f / Math::PI);
        };

        // 辅助函数：计算单个片段在给定 elapsedMS 下 Palm 的理论独立采样角度（度）
        auto calcClipStandaloneDeg = [](const String &clipName, float elapsedMS) -> float
        {
            float tSec = std::clamp(elapsedMS / 1000.0f, 0.0f, 8.0f);
            float ratio = (tSec <= 4.0f) ? (tSec / 4.0f) : ((8.0f - tSec) / 4.0f);
            float maxDeg = (clipName == kArmRotateLeft) ? 45.0f : -45.0f;
            return maxDeg * ratio;
        };

        mTimerTickCount++;

        // --- (A) T0-4 暂停/恢复验证（Tick 4 = 1.0s 暂停，Tick 6 = 1.5s 检查定格，Tick 8 = 2.0s 恢复） ---
        if (mTimerTickCount == 4)
        {
            TResult res = animPlayer->pausePlayback(playbackID);
            T3D_ASSERT(T3D_SUCCEEDED(res) && animPlayer->isPaused());
            mPausedPalmOrientation = palmXform->getOrientation();
            T3D_LOG_INFO(LOG_TAG_APP, "[T0-4 Verify] Paused at 1.0s (ID=%u), Palm Z-angle=%+.2f deg",
                playbackID, quatToZDeg(mPausedPalmOrientation));
        }
        else if (mTimerTickCount == 6)
        {
            const Quaternion &curOri = palmXform->getOrientation();
            T3D_ASSERT(curOri == mPausedPalmOrientation);
            T3D_LOG_INFO(LOG_TAG_APP, "[T0-4 Verify] Hold check at 1.5s while paused, Palm Z-angle=%+.2f deg (unchanged)",
                quatToZDeg(curOri));
        }
        else if (mTimerTickCount == 8)
        {
            T3D_ASSERT(palmXform->getOrientation() == mPausedPalmOrientation);
            TResult res = animPlayer->resumePlayback(playbackID);
            T3D_ASSERT(T3D_SUCCEEDED(res) && !animPlayer->isPaused());
            T3D_LOG_INFO(LOG_TAG_APP, "[T0-4 Verify] Resumed at 2.0s (ID=%u), continuing smoothly.", playbackID);
        }
        // --- (B) Tick 14 (3.5s，有效播放约 2.5s，手臂已大幅向左弯曲)：演示【无混合硬切 isBlending=false】 ---
        else if (mTimerTickCount == 14)
        {
            const Quaternion &curOri = palmXform->getOrientation();
            T3D_ASSERT(curOri != mPausedPalmOrientation);
            float beforeDeg = quatToZDeg(curOri);

            // 使用 isBlending = false 硬切到 RotateRight，并立即执行一次求值对比切换前后同一时刻的突变
            ID hardCutID = animPlayer->playClip(kArmRotateRight, /*isBlending=*/false, true, useGPUSkin);
            T3D_ASSERT(hardCutID != AnimationPlayer::INVALID_ID && !animPlayer->isCrossFading());
            animPlayer->updateAnimation();
            float afterDeg = quatToZDeg(palmXform->getOrientation());
            float jumpDeg = std::abs(afterDeg - beforeDeg);

            // 硬切断言：从 ~+28° 直接归零到 0°，瞬间跳变必然大于 15°
            T3D_ASSERT(jumpDeg > 15.0f);
            T3D_LOG_INFO(LOG_TAG_APP,
                "\n================================================================================\n"
                "[T1-2 Compare 1: HARD CUT (isBlending = false)] '%s' -> '%s'\n"
                "  Palm Z-Angle BEFORE switch = %+6.2f deg\n"
                "  Palm Z-Angle AFTER  switch = %+6.2f deg  >>> INSTANT JUMP Delta = %.2f deg (snaps back to 0!)\n"
                "================================================================================",
                kArmRotateLeft, kArmRotateRight, beforeDeg, afterDeg, jumpDeg);
        }
        // --- (C) Tick 24 (6.0s，RotateRight 已播放 2.5s，手臂大幅向右弯曲至 ~-28°)：启动【CrossFade 混合 isBlending=true】 ---
        else if (mTimerTickCount == 24)
        {
            float beforeDeg = quatToZDeg(palmXform->getOrientation());

            animPlayer->setBlendDuration(1500);
            ID blendID = animPlayer->playClip(kArmRotateLeft, /*isBlending=*/true, true, useGPUSkin);
            T3D_ASSERT(blendID != AnimationPlayer::INVALID_ID && animPlayer->isCrossFading());

            // 立即求值验证：在 alpha=0 处，切换前后姿态完全连续，零跳变！
            animPlayer->updateAnimation();
            float afterDeg = quatToZDeg(palmXform->getOrientation());
            float jumpDeg = std::abs(afterDeg - beforeDeg);
            T3D_ASSERT(jumpDeg < 1.0f);

            T3D_LOG_INFO(LOG_TAG_APP,
                "\n================================================================================\n"
                "[T1-2 Compare 2: CROSSFADE BLEND (isBlending = true, duration = 1500ms)] '%s' -> '%s'\n"
                "  Palm Z-Angle BEFORE switch = %+6.2f deg\n"
                "  Palm Z-Angle AFTER  switch = %+6.2f deg  >>> INSTANT JUMP Delta = %.2f deg (ZERO jump, smooth start!)\n"
                "--------------------------------------------------------------------------------",
                kArmRotateRight, kArmRotateLeft, beforeDeg, afterDeg, jumpDeg);
        }
        // --- Tick 25 ~ 30 (6.25s ~ 7.50s)：每 250ms 实时打印 CrossFade 混合进度条与三方角度对比 ---
        else if (mTimerTickCount >= 25 && mTimerTickCount <= 30)
        {
            float blendedDeg = quatToZDeg(palmXform->getOrientation());
            float alpha = animPlayer->getBlendAlpha();
            const auto &primary = animPlayer->getPrimaryClipState();
            const auto &target = animPlayer->getTargetClipState();

            if (animPlayer->isCrossFading())
            {
                float fromDeg = calcClipStandaloneDeg(primary.clipName, primary.elapsedMS);
                float toDeg = calcClipStandaloneDeg(target.clipName, target.elapsedMS);

                // 断言：混合后的角度严格介于源片段角度与目标片段角度之间！
                float minDeg = std::min(fromDeg, toDeg) - 0.5f;
                float maxDeg = std::max(fromDeg, toDeg) + 0.5f;
                T3D_ASSERT(blendedDeg >= minDeg && blendedDeg <= maxDeg);

                int filled = static_cast<int>(alpha * 20.0f + 0.5f);
                String bar(filled, '#');
                bar.append(20 - filled, '.');

                T3D_LOG_INFO(LOG_TAG_APP,
                    "  [CrossFade alpha=%.2f | %s] From(%s)=%+6.2f deg + To(%s)=%+6.2f deg ===> Blended=%+6.2f deg",
                    alpha, bar.c_str(),
                    primary.clipName.c_str(), fromDeg,
                    target.clipName.c_str(), toDeg,
                    blendedDeg);
            }
            else
            {
                T3D_ASSERT(animPlayer->getCurrentClipName() == kArmRotateLeft);
                T3D_LOG_INFO(LOG_TAG_APP,
                    "  [CrossFade alpha=1.00 | ####################] Done! Seamlessly switched to '%s', Palm=%+6.2f deg\n"
                    "================================================================================",
                    animPlayer->getCurrentClipName().c_str(), blendedDeg);
            }
        }
        else if (mTimerTickCount == 31)
        {
            // 第 31 tick（1750ms > 1500ms）：确保过渡必定已完成并稳定在 kArmRotateLeft
            T3D_ASSERT(!animPlayer->isCrossFading());
            T3D_ASSERT(animPlayer->getCurrentClipName() == kArmRotateLeft);
            T3D_LOG_INFO(LOG_TAG_APP,
                "[T1-2 Verify] All assertions passed! Now alternating CROSSFADE vs HARD CUT every 3s in window...");
        }
        // --- (D) Tick > 31：每 12 ticks (3.0s) 在窗口中自动交替触发一次 CrossFade 与硬切，方便肉眼直观对比 ---
        else if (mTimerTickCount > 31 && ((mTimerTickCount - 31) % 12 == 0))
        {
            uint32_t cycle = (mTimerTickCount - 31) / 12;
            String curClip = animPlayer->getCurrentClipName();
            String nextClip = (curClip == kArmRotateLeft) ? kArmRotateRight : kArmRotateLeft;
            // 奇数轮演示 CrossFade 平滑过渡，偶数轮演示硬切瞬移
            bool useBlend = (cycle % 2 == 1);
            float beforeDeg = quatToZDeg(palmXform->getOrientation());

            animPlayer->setBlendDuration(1200);
            animPlayer->playClip(nextClip, useBlend, true, useGPUSkin);
            animPlayer->updateAnimation();
            float afterDeg = quatToZDeg(palmXform->getOrientation());

            T3D_LOG_INFO(LOG_TAG_APP,
                "[Live Compare #%u] %s : '%s' -> '%s' | Before=%+6.2f deg -> After=%+6.2f deg (Jump Delta=%.2f deg)",
                cycle,
                useBlend ? ">>> CROSSFADE BLEND (1200ms) <<<" : "!!! HARD CUT (NO BLEND) !!!",
                curClip.c_str(), nextClip.c_str(),
                beforeDeg, afterDeg, std::abs(afterDeg - beforeDeg));
        }
    });
    
    // aabb bound component
    AabbBoundPtr bound = go->addComponent<AabbBound>();
    buildAabb(mCubeMesh, submesh, bound);
}

Texture2DPtr SkeletalAnimationApp::buildArmTexture()
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
#if 0
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
#else
            // B
            lines[i++] = 255;
            // G
            lines[i++] = 255;
            // R
            lines[i++] = 255;
#endif
            
            // A
            lines[i++] = 255;
        }
    }
    
    Buffer texData;
    texData.Data = pixels;
    texData.DataSize = dataSize;
    
    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("textureCube", width, height, PixelFormat::E_PF_B8G8R8X8, texData);
    
    return texture;
}

MaterialPtr SkeletalAnimationApp::buildArmMaterial(Tiny3D::Shader *shader)
{
    // samplers
    ShaderSamplerParams samplers;
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildArmTexture();
    // sampler state
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    
    // material
    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Cube-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    disableKeywords.push_back(DEFINE_GPU_SKIN);
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());
    
    initMaterialData(material);
    
    return material;
}


MeshPtr SkeletalAnimationApp::buildArmMesh(const Tiny3D::UUID &materialUUID)
{
    // 顶点数据排列如下：
    // Back Face  Right Face  Forward Face  Left Face
    // 6-----7    14-----15   22-----23     30-----31
    // |     |     |     |     |     |       |     |
    // |     |     |     |     |     |       |     |
    // 4-----5    12-----13   20-----21     28-----29
    // |     |     |     |     |     |       |     |
    // |     |     |     |     |     |       |     |
    // 2-----3    10-----11   18-----19     26-----27
    // |     |     |     |     |     |       |     |
    // |     |     |     |     |     |       |     |
    // 0-----1     8-----9    16-----17     24-----25
    //
    // Top Face   Bottom Face
    // 35-----34  37-----36
    //  |     |    |     |
    //  |     |    |     |
    // 32-----33  38-----39
    
    struct BoxVertex
    {
        Vector3 position {};
        Vector3 normal {};
        Vector2 uv {};
        float weight[T3D_MAX_BLEND_BONES] {0.0f};
        uint8_t indices[T3D_MAX_BLEND_BONES] {0xFF};
    };
    
    // vertex attributes
    VertexAttributes attributes(5);

    uint32_t attribIdx = 0;
    // position
    uint32_t offset = 0;
    VertexAttribute attrPos(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
    offset += sizeof(Vector3);
    attributes[attribIdx] = attrPos;
    attribIdx++;
    // normal
    VertexAttribute attrNormal(0, offset, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
    offset += sizeof(Vector3);
    attributes[attribIdx] = attrNormal;
    attribIdx++;
    // uv
    VertexAttribute attrUV(0, offset, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
    offset += sizeof(Vector2);
    attributes[attribIdx] = attrUV;
    attribIdx++;
    // blend weights
    VertexAttribute attrWeights(0, offset, VertexAttribute::Type::E_VAT_FLOAT4, VertexAttribute::Semantic::E_VAS_BLENDWEIGHT, 0);
    offset += sizeof(float) * T3D_MAX_BLEND_BONES;
    attributes[attribIdx] = attrWeights;
    attribIdx++;
    // blend indices
    VertexAttribute attrIndices(0, offset, VertexAttribute::Type::E_VAT_UBYTE4, VertexAttribute::Semantic::E_VAS_BLENDINDICES, 0);
    offset += sizeof(uint32_t) * T3D_MAX_BLEND_BONES;
    attributes[attribIdx] = attrIndices;
    attribIdx++;

    // vertices & indices
    const uint32_t kVertexCount = 40;
    const uint32_t kIndexCount = 84;
    BoxVertex *vertices = T3D_POD_NEW_ARRAY(BoxVertex, kVertexCount);
    uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, kIndexCount);

    // Back
    for (int32_t i = 0; i < 8; i++)
    {
        int32_t idx = i & 0x1; // i % 2
        int32_t j = i >> 1; // i / 2;
        vertices[i].position.x() = idx == 0 ? -0.5f : 0.5f;
        vertices[i].position.y() = kOneBoneLength * static_cast<float>(j);
        vertices[i].position.z() = -0.5f;
        vertices[i].normal = -Vector3::FORWARD;
        vertices[i].uv = Vector2(0.5f, 0.5f);
    }

    // Right
    for (int32_t i = 0; i < 8; i++)
    {
        int32_t idx = i & 0x1; // i % 2
        int32_t j = i >> 1; // i / 2;
        vertices[i + 8].position.x() = 0.5f;
        vertices[i + 8].position.y() = kOneBoneLength * static_cast<float>(j);
        vertices[i + 8].position.z() = idx == 0 ? -0.5f : 0.5f;
        vertices[i + 8].normal = Vector3::RIGHT;
        vertices[i + 8].uv = Vector2(0.5f, 0.5f);
    }

    // Forward
    for (int32_t i = 0; i < 8; i++)
    {
        int32_t idx = i & 0x1; // i % 2
        int32_t j = i >> 1; // i / 2;
        vertices[i + 16].position.x() = idx == 0 ? 0.5f : -0.5f;
        vertices[i + 16].position.y() = kOneBoneLength * static_cast<float>(j);
        vertices[i + 16].position.z() = 0.5f;
        vertices[i + 16].normal = Vector3::FORWARD;
        vertices[i + 16].uv = Vector2(0.5f, 0.5f);
    }

    // Left
    for (int32_t i = 0; i < 8; i++)
    {
        int32_t idx = i & 0x1; // i % 2
        int32_t j = i >> 1; // i / 2;
        vertices[i + 24].position.x() = -0.5f;
        vertices[i + 24].position.y() = kOneBoneLength * static_cast<float>(j);
        vertices[i + 24].position.z() = idx == 0 ? 0.5f : -0.5f;
        vertices[i + 24].normal = -Vector3::RIGHT;
        vertices[i + 24].uv = Vector2(0.5f, 0.5f);
    }

    // 骨骼权重和索引
    for (int32_t i = 0; i < 4; i++)
    {
        int32_t idx = 8 * i;
        vertices[idx].weight[0] = 1.0f; vertices[idx].weight[1] = vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 0; vertices[idx].indices[1] = vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = 1.0f; vertices[idx].weight[1] = vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 0; vertices[idx].indices[1] = vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = vertices[idx].weight[1] = 0.5f; vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 0; vertices[idx].indices[1] = 1; vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = vertices[idx].weight[1] = 0.5f; vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 0; vertices[idx].indices[1] = 1; vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = vertices[idx].weight[1] = 0.5f; vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 1; vertices[idx].indices[1] = 2; vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = vertices[idx].weight[1] = 0.5f; vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 1; vertices[idx].indices[1] = 2; vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = 1.0f; vertices[idx].weight[1] = vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 2; vertices[idx].indices[1] = vertices[idx].indices[2] = vertices[idx].indices[3] = -1;

        idx++;
        vertices[idx].weight[0] = 1.0f; vertices[idx].weight[1] = vertices[idx].weight[2] = vertices[idx].weight[3] = 0.0f;
        vertices[idx].indices[0] = 2; vertices[idx].indices[1] = vertices[idx].indices[2] = vertices[idx].indices[3] = -1;
    }
    
    float top = 3.0f * 4.0f;
    
    // Top
    vertices[32].position = Vector3(-0.5f, top, -0.5f);
    vertices[32].normal = Vector3::UP;
    vertices[32].uv = Vector2(0.5f, 0.5f);
    
    vertices[33].position = Vector3(0.5f, top, -0.5f);
    vertices[33].normal = Vector3::UP;
    vertices[33].uv = Vector2(0.5f, 0.5f);
    
    vertices[34].position = Vector3(0.5f, top, 0.5f);
    vertices[34].normal = Vector3::UP;
    vertices[34].uv = Vector2(0.5f, 0.5f);
    
    vertices[35].position = Vector3(-0.5f, top, 0.5f);
    vertices[35].normal = Vector3::UP;
    vertices[35].uv = Vector2(0.5f, 0.5f);

    for (int32_t i = 32; i < 36; i++)
    {
        vertices[i].weight[0] = 1.0f; vertices[i].weight[1] = vertices[i].weight[2] = vertices[i].weight[3] = 0.0f;
        vertices[i].indices[0] = 2; vertices[i].indices[1] = vertices[i].indices[2] = vertices[i].indices[3] = -1;
    }
    
    // Bottom
    vertices[36].position = Vector3(-0.5f, 0.0f, -0.5f);
    vertices[36].normal = -Vector3::UP;
    vertices[36].uv = Vector2(0.5f, 0.5f);

    vertices[37].position = Vector3(0.5f, 0.0f, -0.5f);
    vertices[37].normal = -Vector3::UP;
    vertices[37].uv = Vector2(0.5f, 0.5f);

    vertices[38].position = Vector3(0.5f, 0.0f, 0.5f);
    vertices[38].normal = -Vector3::UP;
    vertices[38].uv = Vector2(0.5f, 0.5f);

    vertices[39].position = Vector3(-0.5f, 0.0f, 0.5f);
    vertices[39].normal = -Vector3::UP;
    vertices[39].uv = Vector2(0.5f, 0.5f);

    for (int32_t i = 36; i < 40; i++)
    {
        vertices[i].weight[0] = 1.0f; vertices[i].weight[1] = vertices[i].weight[2] = vertices[i].weight[3] = 0.0f;
        vertices[i].indices[0] = 0; vertices[i].indices[1] = vertices[i].indices[2] = vertices[i].indices[3] = -1;
    }
    
    // Back bottom face
    indices[0] = 0; indices[1] = 2; indices[2] = 3;
    indices[3] = 0; indices[4] = 3; indices[5] = 1;

    // Back middle face
    indices[6] = 2; indices[7] = 4; indices[8] = 5;
    indices[9] = 2; indices[10] = 5; indices[11] = 3;

    // Back top face
    indices[12] = 4; indices[13] = 6; indices[14] = 7;
    indices[15] = 4; indices[16] = 7; indices[17] = 5;
    
    // Right bottom face
    indices[18] = 8; indices[19] = 10; indices[20] = 11;
    indices[21] = 8; indices[22] = 11; indices[23] = 9;
    
    // Right middle face
    indices[24] = 10; indices[25] = 12; indices[26] = 13;
    indices[27] = 10; indices[28] = 13; indices[29] = 11;

    // Right top face
    indices[30] = 12; indices[31] = 14; indices[32] = 15;
    indices[33] = 12; indices[34] = 15; indices[35] = 13;

    // Forward bottom face
    indices[36] = 16; indices[37] = 18; indices[38] = 19;
    indices[39] = 16; indices[40] = 19; indices[41] = 17;

    // Forward middle face
    indices[42] = 18; indices[43] = 20; indices[44] = 21;
    indices[45] = 18; indices[46] = 21; indices[47] = 19;

    // Forward top face
    indices[48] = 20; indices[49] = 22; indices[50] = 23;
    indices[51] = 20; indices[52] = 23; indices[53] = 21;

    // Left bottom face
    indices[54] = 24; indices[55] = 26; indices[56] = 27;
    indices[57] = 24; indices[58] = 27; indices[59] = 25;

    // Left middle face
    indices[60] = 26; indices[61] = 28; indices[62] = 29;
    indices[63] = 26; indices[64] = 29; indices[65] = 27;

    // Left top face
    indices[66] = 28; indices[67] = 30; indices[68] = 31;
    indices[69] = 28; indices[70] = 31; indices[71] = 29;

    // Top face
    indices[72] = 32; indices[73] = 35; indices[74] = 34;
    indices[75] = 32; indices[76] = 34; indices[77] = 33;

    // Bottom face
    indices[78] = 38; indices[79] = 37; indices[80] = 36;
    indices[81] = 38; indices[82] = 36; indices[83] = 39;
    
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
    SubMeshPtr submesh = SubMesh::create(name, materialUUID, PrimitiveType::kTriangleList, std::move(indexBuffer), true);
    SubMeshes subMeshes;
    subMeshes.emplace(name, submesh);

    SkeletonPtr skeleton = buildArmSkeleton(nullptr);
    SkeletalAnimationPtr skeletalAni = buildArmSkeletalAnimation();
    
    // skeleton->getRootBoneGameObject()->getTransformNode()->printHierarchy();
    
    MeshPtr mesh = T3D_MESH_MGR.createSkinnedMesh("Cube", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes), skeleton, skeletalAni);
#if defined (T3D_DEBUG)
    APP_LOG_DEBUG("Mesh (%s) Data :\n%s", mesh->getName().c_str(), mesh->getDebugInfo().c_str());
#endif
    return mesh;
}

void SkeletalAnimationApp::buildAnimationTracks(const String &name, const Radian &targetAngle, uint32_t duration, AnimationTracks &tracks)
{
    TranslationTrack trackT;
    OrientationTrack trackO;
    ScalingTrack trackS;
    
    const uint32_t steps = duration / 2; // 一半帧弯曲，一半帧恢復
    
    uint32_t i = 0;
    
    // Upper Arm
    Radian angle;
    Radian stepAngle = targetAngle / static_cast<float>(steps);
    // 弯曲
    for (i = 0; i < steps; i++)
    {
        Quaternion orientation(angle, Vector3::FORWARD);
        KfOrientationPtr keyframe = KfOrientation::create(i * 1000, orientation);
        trackO.push_back(keyframe);
        angle += stepAngle;
    }
    Quaternion orientation(angle, Vector3::FORWARD);
    KfOrientationPtr keyframe = KfOrientation::create(i * 1000, orientation);
    trackO.push_back(keyframe);
    // 恢复
    for (i = 1; i <= steps; i++)
    {
        angle -= stepAngle;
        Quaternion orientation(angle, Vector3::FORWARD);
        KfOrientationPtr keyframe = KfOrientation::create((i + steps) * 1000, orientation);
        trackO.push_back(keyframe);
    }
    AnimationTrackPtr track = AnimationTrack::create(trackT, trackO, trackS);
    tracks.emplace(name, track);
}

SkeletonPtr SkeletalAnimationApp::buildArmSkeleton(Transform3D *parent)
{
    Matrix4 offsetMatrix;
    Vector3 pos = Vector3::ZERO;

    // UpperArm（根骨骼）
    GameObjectPtr upperArmGO = GameObject::create(kUpperArmName);
    Transform3DPtr upperArmXform = upperArmGO->addComponent<Transform3D>();
    upperArmXform->setPosition(pos);
    upperArmXform->setOrientation(Quaternion::IDENTITY);
    upperArmXform->setScaling(Vector3::UNIT_SCALE);
    BonePtr upperArmBone = upperArmGO->addComponent<Bone>();
    offsetMatrix.makeInverseTransform(pos, Vector3::UNIT_SCALE, Quaternion::IDENTITY);
    upperArmBone->setOffsetMatrix(offsetMatrix);

    // // 将 UpperArm 挂到传入的父节点下（场景 hierarchy）
    // if (parent != nullptr)
    // {
    //     parent->addChild(upperArmXform.get());
    // }

    // ForeArm（UpperArm 的子骨骼）
    pos.y() += kOneBoneLength;
    GameObjectPtr foreArmGO = GameObject::create(kForeArmName);
    Transform3DPtr foreArmXform = foreArmGO->addComponent<Transform3D>();
    foreArmXform->setPosition(pos);
    foreArmXform->setOrientation(Quaternion::IDENTITY);
    foreArmXform->setScaling(Vector3::UNIT_SCALE);
    BonePtr foreArmBone = foreArmGO->addComponent<Bone>();
    offsetMatrix.makeInverseTransform(pos, Vector3::UNIT_SCALE, Quaternion::IDENTITY);
    foreArmBone->setOffsetMatrix(offsetMatrix);
    upperArmXform->addChild(foreArmXform.get());

    // Palm（ForeArm 的子骨骼）
    pos.y() += kOneBoneLength;
    GameObjectPtr palmGO = GameObject::create(kPalmName);
    Transform3DPtr palmXform = palmGO->addComponent<Transform3D>();
    palmXform->setPosition(pos);
    palmXform->setOrientation(Quaternion::IDENTITY);
    palmXform->setScaling(Vector3::UNIT_SCALE);
    BonePtr palmBone = palmGO->addComponent<Bone>();
    offsetMatrix.makeInverseTransform(pos, Vector3::UNIT_SCALE, Quaternion::IDENTITY);
    palmBone->setOffsetMatrix(offsetMatrix);
    foreArmXform->addChild(palmXform.get());

    return T3D_SKELETON_MGR.createSkeleton(kArmSkeleton, upperArmGO);
}


SkeletalAnimationPtr SkeletalAnimationApp::buildArmSkeletalAnimation()
{
    // animation clips
    AnimationTracks tracksLeft;

    const uint32_t duration = 8;
    
    // Upper Arm, 15°
    buildAnimationTracks(kUpperArmName, Radian(Math::PI / 12.0f), duration, tracksLeft);
    
    // Fore Arm, 30°
    buildAnimationTracks(kForeArmName, Radian(Math::PI / 6.0f), duration, tracksLeft);

    // Palm, 45°
    buildAnimationTracks(kPalmName, Radian(Math::HALF_PI / 2.0f), duration, tracksLeft);
    
    AnimationClipPtr clipLeft = AnimationClip::create(kArmRotateLeft, duration * 1000, std::move(tracksLeft));

    // 第二个动画片段：反向弯曲，用于验证按名称查找多 Clip
    AnimationTracks tracksRight;
    buildAnimationTracks(kUpperArmName, -Radian(Math::PI / 12.0f), duration, tracksRight);
    buildAnimationTracks(kForeArmName, -Radian(Math::PI / 6.0f), duration, tracksRight);
    buildAnimationTracks(kPalmName, -Radian(Math::HALF_PI / 2.0f), duration, tracksRight);
    AnimationClipPtr clipRight = AnimationClip::create(kArmRotateRight, duration * 1000, std::move(tracksRight));

    AnimationClips clips;
    clips.emplace(kArmRotateLeft, clipLeft);
    clips.emplace(kArmRotateRight, clipRight);
    return T3D_ANIMATION_MGR.createSkeletalAnimation(kAnimationArm, clips);
}


void SkeletalAnimationApp::buildAabb(Mesh *mesh, SubMesh *submesh, AabbBound *bound)
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
        memcpy(points+i, src, sizeof(Vector3));
        // Vector3 *srcPos = (Vector3*)src;
        // T3D_LOG_INFO(LOG_TAG_APP, "Index = %d, Src : (%f, %f, %f), Dst : (%f, %f, %f)", idx, srcPos->x(), srcPos->y(), srcPos->z(), points[i].x(), points[i].y(), points[i].z());
    }
    Aabb aabb;
    aabb.build(points, pointCount);
    T3D_POD_SAFE_DELETE_ARRAY(points);
    bound->setParams(aabb.getMinX(), aabb.getMaxX(), aabb.getMinY(), aabb.getMaxY(), aabb.getMinZ(), aabb.getMaxZ());
}

Texture2DPtr SkeletalAnimationApp::buildPlaneTexture()
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
#if 0
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
#else
            // B
            lines[i++] = 255;
            // G
            lines[i++] = 255;
            // R
            lines[i++] = 255;
#endif
            
            // A
            lines[i++] = 255;
        }
    }
    
    Buffer texData;
    texData.Data = pixels;
    texData.DataSize = dataSize;
    
    Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D("texturePlane", width, height, PixelFormat::E_PF_B8G8R8X8, texData);
    
    return texture;
}

MaterialPtr SkeletalAnimationApp::buildPlaneMaterial(Tiny3D::Shader *shader)
{    
    // samplers
    ShaderSamplerParams samplers;
    const String texSamplerName = "texCube";
    Texture2DPtr texture = buildPlaneTexture();
    // sampler state
    SamplerDesc samplerDesc;
    texture->setSamplerDesc(samplerDesc);
    
    // material
    MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Plane-Material", shader);
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    material->switchKeywords(enableKeywrods, disableKeywords);
    material->setTexture(texSamplerName, texture->getUUID());
    
    initMaterialData(material);
    
    return material;
}

MeshPtr SkeletalAnimationApp::buildPlaneMesh(const Tiny3D::UUID &materialUUID)
{
    struct QuadVertex
    {
        Vector3 position {};
        Vector3 normal {};
        Vector2 uv {};
    };
    
    // vertex attributes
    VertexAttribute attrPos(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
    VertexAttribute attrNormal(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_NORMAL, 0);
    VertexAttribute attrUV(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
    VertexAttributes attributes(3);
    attributes[0] = attrPos;
    attributes[1] = attrNormal;
    attributes[2] = attrUV;

    // vertices & indices
    const uint32_t kVertexCount = 4;
    const uint32_t kIndexCount = 6;
    QuadVertex *vertices = T3D_POD_NEW_ARRAY(QuadVertex, kVertexCount);
    uint16_t *indices = T3D_POD_NEW_ARRAY(uint16_t, kIndexCount);

    float scale = 8.0f;

    // vertex #0
    vertices[0].position = Vector3(1, 0, 1) * scale;
    vertices[0].normal = Vector3::UP;
    vertices[0].uv = Vector2(1, 1);

    // vertex #1
    vertices[1].position = Vector3(1, 0, -1) * scale;
    vertices[1].normal = Vector3::UP;
    vertices[1].uv = Vector2(1, 0);

    // vertex #2
    vertices[2].position = Vector3(-1, 0, -1) * scale;
    vertices[2].normal = Vector3::UP;
    vertices[2].uv = Vector2(0, 0);

    // vertex #3
    vertices[3].position = Vector3(-1, 0, 1) * scale;
    vertices[3].normal = Vector3::UP;
    vertices[3].uv = Vector2(0, 1);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;
    
    // construct mesh resource
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

    MeshPtr mesh = T3D_MESH_MGR.createMesh("Plane", std::move(attributes), std::move(vertexBuffers), std::move(strides), std::move(offsets), std::move(subMeshes));
    return mesh;
}

void SkeletalAnimationApp::buildPlane(Transform3D *parent)
{
    static int index = 0;
    std::stringstream ss;
    ss << "Plane#" << index;
    
    // transform node for cube
    GameObjectPtr go = GameObject::create(ss.str());
    Transform3DPtr xform = go->addComponent<Transform3D>();
    parent->addChild(xform);
    xform->setPosition(Vector3::ZERO);
    Radian xAngles(0.0f);
    Radian yAngles(0.0f);
    Radian zAngles(0.0f);
    xform->rotate(xAngles, yAngles, zAngles);
    xform->setScaling(Vector3(3.0f, 3.0f, 3.0f));
    
    // submesh
    SubMesh *submesh = mPlaneMesh->getSubMesh(SUB_MESH_NAME);

    // geometry component
    GeometryPtr geometry = go->addComponent<Geometry>();
    geometry->setMeshObject(mPlaneMesh, submesh);
}

void SkeletalAnimationApp::initMaterialData(Tiny3D::Material *material)
{
    // 這裡只是設置材質有該項變量，具體值，引擎會幫助動態計算和設置

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
    // bone matrices
    Matrix4Array matrices(T3D_MAX_SKIN_BONES, Matrix4::IDENTITY);
    material->setMatrixArray("tiny3d_BoneMatrices", matrices);
}
