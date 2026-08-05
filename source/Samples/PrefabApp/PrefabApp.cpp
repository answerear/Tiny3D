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

#include "PrefabApp.h"


#define ARCHIVE_TYPE_FS         "FileSystem"

using namespace Tiny3D;

namespace
{
    // Prefab 默认位姿（保存进 .tprefab）
    const Vector3 kPrefabDefaultPos(0.0f, 0.0f, 0.0f);
    // 实例覆盖位姿（应随场景持久化并在第 5 步重放）
    const Vector3 kOverridePos(2.0f, 1.0f, 0.0f);
}

PrefabApp theApp;

PrefabApp::PrefabApp()
{
}

PrefabApp::~PrefabApp()
{
}

void PrefabApp::logStep(const char *name, bool ok, const String &detail)
{
    if (ok)
    {
        ++mPassCount;
        APP_LOG_DEBUG("[PASS] %s %s", name, detail.c_str());
    }
    else
    {
        ++mFailCount;
        APP_LOG_DEBUG("[FAIL] %s %s", name, detail.c_str());
    }

    mReport += String(ok ? "[PASS] " : "[FAIL] ") + name + " " + detail + "\n";
}

void PrefabApp::writeReport()
{
    const String path = mWorkDir + Dir::getNativeSeparator() + "result.txt";
    FileDataStream fs;
    if (fs.open(path.c_str(),
        FileDataStream::E_MODE_TRUNCATE | FileDataStream::E_MODE_WRITE_ONLY
        | FileDataStream::E_MODE_TEXT))
    {
        fs.write((void *)mReport.c_str(), mReport.length());
        fs.close();
    }
}

void PrefabApp::dumpHierarchy(Scene *scene, const char *label)
{
    APP_LOG_DEBUG("===== Hierarchy: %s =====", label);
    mReport += String("===== Hierarchy: ") + label + " =====\n";
    if (scene == nullptr)
    {
        APP_LOG_DEBUG("(scene is null)");
        mReport += "(scene is null)\n";
        return;
    }

    const String tree = scene->printSceneHierarchy(true);
    mReport += tree;
    if (!tree.empty() && tree.back() != '\n')
    {
        mReport += "\n";
    }
}

void PrefabApp::buildCamera(Transform3D *parent, const Vector3 &lookAt)
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
    camera->setClearColor(ColorRGB::GRAY);
    camera->setRenderTarget(rt);
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    const Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);

    // 略抬高、拉远，正面看向覆盖后的方块位置
    const Vector3 eye = lookAt + Vector3(3.0f, 2.5f, -5.0f);
    camera->lookAt(eye, lookAt, Vector3::UP);

    go->addComponent<FrustumBound>();
}

void PrefabApp::buildLights(Scene *scene, Transform3D *parent)
{
    AmbientLightPtr ambient = scene->getRootGameObject()->addComponent<AmbientLight>();
    ambient->setColor(ColorRGB::WHITE);
    ambient->setIntensity(0.45f);

    GameObjectPtr go = GameObject::create("DirectionalLight");
    Transform3DPtr node = go->addComponent<Transform3D>();
    parent->addChild(node);

    const Vector3 eye(-2.0f, 3.0f, -4.0f);
    Matrix3 matR;
    matR.lookAt_LH(eye, Vector3::ZERO, Vector3::UP);
    node->setOrientation(Quaternion(matR));
    node->setPosition(eye);

    DirectionalLightPtr light = go->addComponent<DirectionalLight>();
    light->setColor(ColorRGB::WHITE);
    light->setDiffuseIntensity(1.0f);
    light->setSpecularIntensity(1.0f);
}

bool PrefabApp::setupPreview(Scene *scene, GameObject *prefabRoot, const Vector3 &lookAt)
{
    if (scene == nullptr || prefabRoot == nullptr)
    {
        logStep("SetupPreview", false, "scene or prefabRoot is null");
        return false;
    }

    // Bundle 支持 UUID 与 ShaderLab→Shader 别名重定向；FileSystem 做不到这一点。
    const String bundlePath = Dir::getResourcePath("Assets/samples/bundle");
    ArchivePtr bundle = T3D_ARCHIVE_MGR.loadArchive(
        bundlePath, "BundleFileSystem", Archive::AccessMode::kReadOnly);
    if (bundle == nullptr)
    {
        logStep("SetupPreview", false, "load bundle failed: " + bundlePath);
        return false;
    }

    mPreviewMesh = T3D_MESH_MGR.loadMesh(bundle.get(), "cube.tmesh");
    if (mPreviewMesh == nullptr)
    {
        logStep("SetupPreview", false, "load cube.tmesh failed");
        return false;
    }

    Transform3D *rootXform = scene->getRootTransform();
    buildLights(scene, rootXform);
    buildCamera(rootXform, lookAt);

    // 方块挂到 Prefab 实例根上，跟着覆盖后的 Transform 走
    Geometry *geometry = nullptr;
    GameObjectPtr cube = GameObject::createWithMesh(
        "PreviewCube", mPreviewMesh.get(), geometry, prefabRoot->getTransformNode());
    const bool ok = cube != nullptr && geometry != nullptr;
    logStep("SetupPreview", ok,
        ok ? ("lookAt=("
            + StringConverter::toString(lookAt.x()) + ","
            + StringConverter::toString(lookAt.y()) + ","
            + StringConverter::toString(lookAt.z()) + ")")
           : "createWithMesh failed");
    return ok;
}

bool PrefabApp::runPrefabTests()
{
    mPassCount = 0;
    mFailCount = 0;

    mReport.clear();

    mWorkDir = Dir::getAppPath() + Dir::getNativeSeparator() + "prefab_test";
    Dir::makeDir(mWorkDir);

    ArchivePtr fsArchive = T3D_ARCHIVE_MGR.loadArchive(
        mWorkDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kReadTruncate);
    if (fsArchive == nullptr)
    {
        logStep("MountWorkDir", false, "loadArchive failed");
        return false;
    }
    // 用可写模式挂载，测试需要把 Prefab / Scene 写回工作目录
    T3D_ASSET_MGR.init(AssetManager::Mode::kEditor);
    T3D_ASSET_MGR.unmountAll();
    T3D_ASSET_MGR.mount(fsArchive.get(), 0);
    logStep("MountWorkDir", true, mWorkDir);

    // --- Step 1: Create + Save ---
    ScenePtr scene = T3D_SCENE_MGR.createScene("PrefabTestScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);
    dumpHierarchy(scene.get(), "1.after scene init (empty)");

    GameObjectPtr proto = GameObject::createWithTransform("ProtoRoot");
    Transform3DPtr protoXform = proto->getComponent<Transform3D>();
    protoXform->setPosition(kPrefabDefaultPos);

    GameObjectPtr child = GameObject::createWithTransform("ProtoChild");
    Transform3DPtr childXform = child->getComponent<Transform3D>();
    childXform->setPosition(Vector3(0.5f, 0.0f, 0.0f));
    protoXform->addChild(childXform.get());
    dumpHierarchy(scene.get(), "2.after create proto tree (ProtoRoot+ProtoChild)");

    const String prefabName = "TestCube.tprefab";
    PrefabPtr prefab = T3D_ASSET_MGR.createPrefab(prefabName, proto);
    TResult saveRet = T3D_ASSET_MGR.savePrefab(prefabName, prefab.get());
    const String prefabPath = mWorkDir + Dir::getNativeSeparator() + prefabName;

    size_t savedSize = 0;
    {
        FileDataStream fs;
        if (fs.open(prefabPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            savedSize = (size_t)fs.size();
            fs.close();
        }
    }

    logStep("CreateSavePrefab", T3D_SUCCEEDED(saveRet) && savedSize > 0,
        prefabName + " ret=" + StringConverter::toString((int32_t)saveRet)
        + " size=" + StringConverter::toString((int32_t)savedSize));

    // 卸掉内存里的资源与原型对象，让下一步真正走反序列化，而不是命中缓存
    T3D_PREFAB_MGR.unload(prefab);
    prefab = nullptr;
    proto = nullptr;
    protoXform = nullptr;
    child = nullptr;
    childXform = nullptr;
    dumpHierarchy(scene.get(), "3.after unload prefab (proto destroyed)");

    // --- Step 2: Load + Instantiate ---
    PrefabPtr loaded = T3D_ASSET_MGR.loadPrefab(prefabName);
    bool loadOk = loaded != nullptr && loaded->getRootGameObject() != nullptr;
    logStep("LoadPrefab", loadOk,
        String("prefab=") + (loaded != nullptr ? "ok" : "null")
        + " root=" + (loaded != nullptr && loaded->getRootGameObject() != nullptr ? "ok" : "null"));

    GameObjectPtr instance = nullptr;
    PrefabInstancePtr link = nullptr;
    dumpHierarchy(scene.get(), "4.before instantiate prefab onto scene");
    if (loadOk)
    {
        instance = loaded->instantiate(scene.get(), scene->getRootTransform());
        if (instance != nullptr)
        {
            link = instance->getComponent<PrefabInstance>();
        }
    }
    dumpHierarchy(scene.get(), "5.after instantiate prefab onto scene");
    bool instOk = instance != nullptr && link != nullptr
        && link->getSourcePrefabUUID() == loaded->getUUID();
    logStep("InstantiateLinked", instOk,
        instOk ? link->getSourcePrefabUUID().toString() : "");

    // --- Step 3: Override record ---
    Transform3DPtr instXform = instance != nullptr
        ? instance->getComponent<Transform3D>() : nullptr;
    bool overrideOk = false;
    if (instXform != nullptr && link != nullptr)
    {
        instXform->setPosition(kOverridePos);
        TResult recRet = link->recordOverridesFromSource();
        overrideOk = T3D_SUCCEEDED(recRet) && !link->getModifications().empty();
        if (overrideOk)
        {
            bool foundPos = false;
            for (const auto &mod : link->getModifications())
            {
                if (mod.getPropertyPath() == "Position")
                {
                    foundPos = true;
                    break;
                }
            }
            overrideOk = foundPos;
        }
    }
    logStep("RecordOverride", overrideOk,
        overrideOk ? StringConverter::toString((int32_t)link->getModifications().size()) : "");
    dumpHierarchy(scene.get(), "6.after record override (Position -> override pos)");

    // --- Step 4 / 5: Scene persistence + reload ---
    const String sceneName = "PrefabTest.tscene";
    bool scenePersistOk = false;
    ScenePtr previewScene = nullptr;
    GameObject *previewRoot = nullptr;

    if (instance != nullptr && link != nullptr)
    {
        TResult sceneSaveRet = T3D_SCENE_MGR.saveScene(fsArchive.get(), sceneName, scene.get());
        logStep("SaveScene", T3D_SUCCEEDED(sceneSaveRet), sceneName);

        // 重新加载场景之前：先把当前挂着 Prefab 的场景树打出来
        dumpHierarchy(scene.get(), "7.before reload scene (with prefab instance)");

        // Clear current scene reference and reload
        T3D_SCENE_MGR.setCurrentScene(nullptr);
        scene = nullptr;
        instance = nullptr;
        link = nullptr;

        ScenePtr reloaded = T3D_SCENE_MGR.loadScene(fsArchive.get(), sceneName);
        if (reloaded != nullptr)
        {
            T3D_SCENE_MGR.setCurrentScene(reloaded);
            dumpHierarchy(reloaded.get(), "8.after reload scene");

            PrefabInstance *foundLink = nullptr;
            GameObject *foundRoot = nullptr;
            for (const auto &item : reloaded->getAllGameObjects())
            {
                PrefabInstancePtr pi = item.second->getComponent<PrefabInstance>();
                if (pi != nullptr)
                {
                    foundLink = pi.get();
                    foundRoot = item.second.get();
                    break;
                }
            }

            if (foundLink != nullptr)
            {
                const bool modsPersisted = !foundLink->getModifications().empty();

                // 先打回 Prefab 默认值，再重放覆盖，验证覆盖可复现
                Transform3DPtr rx = foundRoot->getComponent<Transform3D>();
                if (rx != nullptr)
                {
                    rx->setPosition(kPrefabDefaultPos);
                }
                dumpHierarchy(reloaded.get(), "9.after reset to prefab default pos");

                TResult applyRet = foundLink->applyOverridesToInstance();
                dumpHierarchy(reloaded.get(), "10.after applyOverridesToInstance");

                Vector3 pos = rx != nullptr ? rx->getPosition() : Vector3::ZERO;
                const bool posMatch = Math::realEqual(pos.x(), kOverridePos.x())
                    && Math::realEqual(pos.y(), kOverridePos.y())
                    && Math::realEqual(pos.z(), kOverridePos.z());
                scenePersistOk = modsPersisted && T3D_SUCCEEDED(applyRet) && posMatch;

                if (scenePersistOk)
                {
                    previewScene = reloaded;
                    previewRoot = foundRoot;
                }
            }
        }
    }
    logStep("ScenePersistApplyOverrides", scenePersistOk);

    APP_LOG_DEBUG("PrefabApp summary: pass=%d fail=%d", mPassCount, mFailCount);
    mReport += "summary: pass=" + StringConverter::toString(mPassCount)
        + " fail=" + StringConverter::toString(mFailCount) + "\n";
    writeReport();

    if (scenePersistOk && previewScene != nullptr && previewRoot != nullptr)
    {
        setupPreview(previewScene.get(), previewRoot, kOverridePos);
        dumpHierarchy(previewScene.get(), "11.after setupPreview (camera/lights/cube)");
        mReport += "preview: cube follows PrefabInstance at override position; close window to exit\n";
        writeReport();
    }

    return mFailCount == 0;
}

TResult PrefabApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    const bool ok = runPrefabTests();
    if (!ok)
    {
        APP_LOG_DEBUG("PrefabApp tests FAILED");
    }
    else
    {
        APP_LOG_DEBUG("PrefabApp tests ALL PASSED — preview scene is live, close window to exit");
    }

    return T3D_OK;
}

void PrefabApp::applicationWillTerminate()
{
    mPreviewMesh = nullptr;
}
