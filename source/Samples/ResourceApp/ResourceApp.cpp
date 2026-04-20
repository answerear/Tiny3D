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
    ret = theEngine->init(argc, argv, true, true);
    if (ret == T3D_OK)
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}

TResult ResourceApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // auto compile shaders if language mismatch
    const String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Assets" + Dir::getNativeSeparator() + "samples" + Dir::getNativeSeparator() + "meshes";
    autoCompileShaders(path);

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

    // load MetaFSArchive plugin on demand (not in shared Tiny3D.cfg)
    TResult ret = T3D_AGENT.loadPlugin("MetaFSArchive");
    if (T3D_FAILED(ret))
    {
        APP_LOG_DEBUG("Failed to load MetaFSArchive plugin ! ERROR [%d]", ret);
        return ret;
    }

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
    TResult ret = T3D_OK;
    GameObjectPtr go = GameObject::createWithTransform("mesh");
    Transform3D *node = static_cast<Transform3D*>(go->getTransformNode());
    parent->addChild(node);
    // node->setScaling(0.01f, 0.01f, 0.01f);
    
    // mPitchTimerID = T3D_TIMER_MGR.startTimer(1000, true, [node, this](ID timerID, uint32_t dt)
    //     {
    //         const Degree deltaAngle(5.0f);
    //         node->rotate(Vector3::UNIT_X, deltaAngle);
    //     });
    
    const String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Assets" + Dir::getNativeSeparator() + "samples" + Dir::getNativeSeparator() + "meshes";
    ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
    T3D_ASSERT(archive != nullptr);
    const String meshName = "tortoise.tmesh";
    mMesh = T3D_MESH_MGR.loadMesh(archive, meshName);
    T3D_ASSERT(mMesh != nullptr);
    
    GameObjectPtr goBody = GameObject::createWithTransform(mMesh->getMeshNodeName());
    Transform3D *bodyNode = static_cast<Transform3D*>(goBody->getTransformNode());
    bodyNode->setPosition(mMesh->getMeshPosition());
    bodyNode->setOrientation(mMesh->getMeshOrientation());
    bodyNode->setScaling(mMesh->getMeshScaling());
    node->addChild(bodyNode);

    GeometryPtr geometry;
    if (mMesh->getType() == Resource::Type::kMesh)
    {
        geometry = goBody->addComponent<Geometry>();
    }
    else if (mMesh->getType() == Resource::Type::kSkinnedMesh)
    {
        geometry = goBody->addComponent<SkinnedGeometry>();
    }
    else
    {
        T3D_ASSERT(false);
    }
    
    StringArray enableKeywrods;
    enableKeywrods.push_back("");
    StringArray disableKeywords;
    for (auto submesh : mMesh->getSubMeshes())
    {
        Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(submesh.second->getMaterialUUID()));
        T3D_ASSERT(material != nullptr);
        ret = material->switchKeywords(enableKeywrods, disableKeywords);
        T3D_ASSERT(T3D_SUCCEEDED(ret));
    }

    for (const auto submesh : mMesh->getSubMeshes())
    {
        geometry->setMeshObject(mMesh, submesh.second);
        break;
    }

    if (mMesh->getType() == Resource::Type::kSkinnedMesh)
    {
        SkinnedMesh *skinnedMesh = smart_pointer_cast<SkinnedMesh>(mMesh);
        T3D_ASSERT(skinnedMesh != nullptr);
        SkeletalAnimation *skeletalAni = skinnedMesh->getSkeletalAnimation();
        T3D_ASSERT(skeletalAni != nullptr);
        const AnimationClips &clips = skeletalAni->getAnimationClips();
        T3D_ASSERT(!clips.empty());
        const String &clipName = clips.begin()->first;
        SkinnedGeometry *skinnedGeometry = smart_pointer_cast<SkinnedGeometry>(geometry);
        skinnedGeometry->populateAllChildren();
        skinnedGeometry->setDefaultClipName(clipName);
        T3D_ASSERT(skinnedGeometry != nullptr);
        skinnedGeometry->setGPUSkinning(false);
        const String &defaultClip = skinnedGeometry->getDefaultClipName();
        skinnedGeometry->play(defaultClip, true);
        //
        // T3D_TIMER_MGR.startTimer(8, false, [this, skinnedGeometry](uint32_t timerID, uint32_t dt)
        // {
        //     skinnedGeometry->stop();
        // });
    }
}

void ResourceApp::autoCompileShaders(const String &resourcePath)
{
    // Determine required shader language from the active renderer
    String requiredLang;
    const String &rendererName = T3D_AGENT.getActiveRHIRenderer()->getName();
    if (rendererName == RHIRenderer::OPENGL4
        || rendererName == RHIRenderer::OPENGLES2
        || rendererName == RHIRenderer::OPENGLES3)
    {
        requiredLang = "glsl";
    }
    else
    {
        requiredLang = "hlsl";
    }

    // Enumerate all *.shader.meta files in the resource directory
    String searchPattern = resourcePath + Dir::getNativeSeparator() + "*.shader.meta";
    Dir dir;
    bool working = dir.findFile(searchPattern);

    while (working)
    {
        if (!dir.isDots() && !dir.isDirectory())
        {
            do
            {
                const String shaderMetaPath = dir.getFilePath();

                // Read .shader.meta to get ShaderUUID
                FileDataStream fs;
                if (!fs.open(shaderMetaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                {
                    APP_LOG_DEBUG("Failed to open %s", shaderMetaPath.c_str());
                    break;
                }

                MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
                fs.close();

                if (meta == nullptr || meta->getType() != Meta::kShaderLab)
                {
                    break;
                }

                MetaShaderLab *metaShaderLab = static_cast<MetaShaderLab *>(meta.get());
                const Tiny3D::UUID &shaderUUID = metaShaderLab->getShaderUUID();

                // Derive .shader source path from .shader.meta path (strip ".meta")
                String shaderSourcePath = shaderMetaPath.substr(0, shaderMetaPath.length() - 5);

                // Find the .tshader.meta file by looking for the file whose MetaShader UUID matches
                // We know the naming convention: <title>.tshader.meta
                String shaderDir, shaderName;
                Dir::parsePath(shaderSourcePath, shaderDir, shaderName);
                String shaderTitle, shaderExt;
                Dir::parsePath(shaderSourcePath, shaderDir, shaderTitle, shaderExt);
                String tshaderMetaPath = resourcePath + Dir::getNativeSeparator() + shaderTitle + "." + Resource::EXT_SHADER + ".meta";

                if (!Dir::exists(tshaderMetaPath))
                {
                    APP_LOG_DEBUG("tshader meta not found: %s", tshaderMetaPath.c_str());
                    break;
                }

                // Read .tshader.meta to get Language and UUID
                FileDataStream tshaderMetaFs;
                if (!tshaderMetaFs.open(tshaderMetaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                {
                    APP_LOG_DEBUG("Failed to open %s", tshaderMetaPath.c_str());
                    break;
                }

                MetaPtr tshaderMeta = T3D_SERIALIZER_MGR.deserialize<Meta>(tshaderMetaFs);
                tshaderMetaFs.close();

                if (tshaderMeta == nullptr || tshaderMeta->getType() != Meta::kShader)
                {
                    break;
                }

                MetaShader *metaShader = static_cast<MetaShader *>(tshaderMeta.get());
                const String &currentLang = metaShader->getLanguage();

                if (currentLang == requiredLang)
                {
                    // Language matches, no recompilation needed
                    break;
                }

                APP_LOG_DEBUG("Shader language mismatch for %s: current='%s', required='%s'. Recompiling...",
                    shaderTitle.c_str(), currentLang.c_str(), requiredLang.c_str());

                // Call scc.exe to recompile with -u to preserve UUID
                String sccPath = Dir::getAppPath() + Dir::getNativeSeparator() + "scc.exe";
                String cmdLine = shaderSourcePath
                    + " -t " + requiredLang
                    + " -u " + metaShader->getUUID().toString()
                    + " -o " + resourcePath;

                Process process;
                TResult ret = process.start(sccPath, cmdLine);
                if (T3D_FAILED(ret))
                {
                    APP_LOG_DEBUG("Failed to start scc.exe for %s ! ERROR [%d]", shaderTitle.c_str(), ret);
                    break;
                }

                ret = process.wait();
                if (T3D_FAILED(ret))
                {
                    APP_LOG_DEBUG("Failed to wait for scc.exe for %s ! ERROR [%d]", shaderTitle.c_str(), ret);
                    break;
                }

                uint32_t exitCode = process.getExitCode();
                if (exitCode != 0)
                {
                    APP_LOG_DEBUG("scc.exe failed for %s with exit code [%d]", shaderTitle.c_str(), exitCode);
                    break;
                }

                // Update .tshader.meta with new language
                metaShader->setLanguage(requiredLang);
                FileDataStream writeFs;
                if (writeFs.open(tshaderMetaPath.c_str(),
                    FileDataStream::EOpenMode::E_MODE_TRUNCATE
                    | FileDataStream::EOpenMode::E_MODE_READ_WRITE
                    | FileDataStream::EOpenMode::E_MODE_TEXT))
                {
                    T3D_SERIALIZER_MGR.serialize(writeFs, metaShader);
                    writeFs.close();
                }

                APP_LOG_DEBUG("Successfully recompiled %s to %s", shaderTitle.c_str(), requiredLang.c_str());
            } while (false);
        }

        working = dir.findNextFile();
    }

    dir.close();
}




