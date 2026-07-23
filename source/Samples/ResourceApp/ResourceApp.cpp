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

#include "ResourceApp.h"


#define UVN_CAMERA

#define USE_GPU_SKIN

#define ARCHIVE_TYPE_BUNDLE         "BundleFileSystem"

using namespace Tiny3D;


ResourceApp theApp;


ResourceApp::ResourceApp()
{
}

ResourceApp::~ResourceApp()
{
}

TResult ResourceApp::go(int32_t argc, char *argv[])
{
    TResult ret;

    Agent *theEngine = T3D_NEW Agent();
    ret = theEngine->init(argc, argv, true, true);
    if (ret == T3D_OK)
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}

TResult ResourceApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    // add ambient light to the root of scene
    AmbientLightPtr ambient = scene->getRootGameObject()->addComponent<AmbientLight>();
    ambient->setColor(ColorRGB::WHITE);
    ambient->setIntensity(1.0f);
    
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

    // camera
    buildCamera(root);

    // runtime 模式：挂载只读 bundle 到资源门面搜索链（主包优先级 0）
    // 只读 bundle：BundleFSArchive 读 bundle.manifest，按名字/UUID 直读散列文件。
    // 逻辑资源路径(小写、'/' 分隔)，由 Platform 层解析为各平台物理路径，零平台宏。
    // Windows / Android 统一从 bundle 加载，BundleFSArchive 已由 Tiny3D.cfg 自动加载。
    const String bundlePath = Dir::getResourcePath("assets/samples/bundle");
    ArchivePtr bundle = T3D_ARCHIVE_MGR.loadArchive(bundlePath, ARCHIVE_TYPE_BUNDLE, Archive::AccessMode::kRead);
    T3D_ASSERT(bundle != nullptr);
    T3D_ASSET_MGR.init(AssetManager::Mode::kRuntime);
    T3D_ASSET_MGR.mount(bundle, 0);

    // mesh
    loadMesh(root);
    
    APP_LOG_DEBUG("Scene hierarchy:");
    scene->printSceneHierarchy(true);
    
    return T3D_OK;
}

void ResourceApp::applicationWillTerminate() 
{
    if (mPitchTimerID != T3D_INVALID_TIMER_ID)
    {
        T3D_TIMER_MGR.stopTimer(mPitchTimerID);
        mPitchTimerID = T3D_INVALID_TIMER_ID;
    }
    
    mMesh = nullptr;
}

void ResourceApp::buildCamera(Transform3D *parent)
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

    Vector3 eye(10.0f, 20.0f, -20.0f);

#if defined (UVN_CAMERA)
    Vector3 obj(0.0f, 0.0f, 0.0f);
    camera->lookAt(eye, obj, Vector3::UP);
#else
    xform->setPosition(eye);
    // Radian xAngle(Degree(25.0f).valueRadians());
    // Radian yAngle(-Math::PI * 0.25f);
    Radian xAngle(0.0f);
    Radian yAngle(0.0f);
    Radian zAngle(0.0f);
    xform->rotate(xAngle, yAngle, zAngle);
#endif

    // construct frustum bound
    auto frustum = go->addComponent<FrustumBound>();
    T3D_ASSERT(frustum != nullptr);
}

void ResourceApp::loadMesh(Transform3D *parent)
{
    GameObjectPtr go = GameObject::createWithTransform("mesh");
    Transform3D *node = static_cast<Transform3D*>(go->getTransformNode());
    parent->addChild(node);
    // node->setScaling(0.01f, 0.01f, 0.01f);
    
    // mPitchTimerID = T3D_TIMER_MGR.startTimer(1000, true, [node, this](ID timerID, uint32_t dt)
    //     {
    //         const Degree deltaAngle(5.0f);
    //         node->rotate(Vector3::UNIT_X, deltaAngle);
    //     });
    
    // 资源 bundle 已在 applicationDidFinishLaunching 挂载到资源门面搜索链，
    // 这里应用层无需再感知 archive，直接按名字一步加载即可。
    // shader 在运行时按当前 renderer 的着色语言从 ShaderVariantSet 选对应变体。
    const String meshName = "tortoise.tmesh";
    mMesh = T3D_ASSET_MGR.loadMesh(meshName);
    T3D_ASSERT(mMesh != nullptr);
    
    Geometry *geometry = nullptr;
    GameObjectPtr goBody = GameObject::createWithMesh(mMesh->getMeshNodeName(), mMesh, geometry, node);
    T3D_ASSERT(goBody != nullptr && geometry != nullptr);

    Transform3D *bodyNode = static_cast<Transform3D*>(goBody->getTransformNode());
    bodyNode->setPosition(mMesh->getMeshPosition());
    bodyNode->setOrientation(mMesh->getMeshOrientation());
    bodyNode->setScaling(mMesh->getMeshScaling());

    if (mMesh->getType() == Resource::Type::kSkinnedMesh)
    {
        SkinnedGeometry *skinnedGeometry = static_cast<SkinnedGeometry*>(geometry);
        skinnedGeometry->setGPUSkinning(false);
        skinnedGeometry->play(skinnedGeometry->getDefaultClipName(), true);
        //
        // T3D_TIMER_MGR.startTimer(8, false, [this, skinnedGeometry](uint32_t timerID, uint32_t dt)
        // {
        //     skinnedGeometry->stop();
        // });
    }
}

