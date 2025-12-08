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

#define ARCHIVE_TYPE_METAFS         "MetaFileSystem"
#define ARCHIVE_TYPE_FS             "FileSystem"

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
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchive");
    settings.pluginSettings.plugins.push_back("MetaFSArchive");
    settings.pluginSettings.plugins.push_back("D3D11Renderer");
    settings.pluginSettings.plugins.push_back("FreeImageCodec");

#if defined (T3D_OS_WINDOWS)
    settings.renderSettings.renderer = RHIRenderer::DIRECT3D11;
#else
#endif
    
    ret = theEngine->init(argc, argv, true, true, settings);
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

    // mesh
    loadMesh(root);
    
    return T3D_OK;
}

void ResourceApp::applicationWillTerminate() 
{
    mCubeMesh = nullptr;
    mPlaneMesh = nullptr;
    mCubeMaterial = nullptr;
    mPlaneMaterial = nullptr;
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

void ResourceApp::loadMesh(Transform3D *parent)
{
    TResult ret = T3D_OK;
    GameObjectPtr go = GameObject::createWithTransform("mesh");
    Transform3D *node = static_cast<Transform3D*>(go->getTransformNode());
    parent->addChild(node);
    node->setScaling(0.01f, 0.01f, 0.01f);
    
    const String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Assets" + Dir::getNativeSeparator() + "samples" + Dir::getNativeSeparator() + "meshes";
    ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
    T3D_ASSERT(archive != nullptr);
    const String meshName = "tortoise.tmesh";
    MeshPtr mesh = T3D_MESH_MGR.loadMesh(archive, meshName);
    T3D_ASSERT(mesh != nullptr);

    GeometryPtr geometry;
    if (mesh->getType() == Resource::Type::kMesh)
    {
        geometry = go->addComponent<Geometry>();
    }
    else if (mesh->getType() == Resource::Type::kSkinnedMesh)
    {
        geometry = go->addComponent<SkinnedGeometry>();
    }
    else
    {
        T3D_ASSERT(false);
    }
    
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    for (auto submesh : mesh->getSubMeshes())
    {
        Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
        T3D_ASSERT(material != nullptr);
        ret = material->switchKeywords(enableKeywrods, disableKeywords);
        T3D_ASSERT(T3D_SUCCEEDED(ret));
    }

    for (const auto submesh : mesh->getSubMeshes())
    {
        geometry->setMeshObject(mesh, submesh.second);
        break;
    }

    if (mesh->getType() == Resource::Type::kSkinnedMesh)
    {
        SkinnedGeometry *skinnedGeometry = smart_pointer_cast<SkinnedGeometry>(geometry);
        skinnedGeometry->setGPUSkinning(false);
        const String &defaultClip = skinnedGeometry->getDefaultClipName();
        skinnedGeometry->play(defaultClip, true);
    }
}




