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

#include "RmlUiApp.h"

#include "T3DRmlCanvas.h"
#include "UI/T3DUISystem.h"

#include <cstdlib>

#define ARCHIVE_TYPE_FS         "FileSystem"
#define ARCHIVE_TYPE_ANDROID    "AndroidAsset"


using namespace Tiny3D;


RmlUiApp theApp;


RmlUiApp::RmlUiApp()
{
}

RmlUiApp::~RmlUiApp()
{
}

TResult RmlUiApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    (void)argc;
    (void)argv;

    UISystem *uiSystem = T3D_AGENT.getUISystem();
    if (uiSystem == nullptr)
    {
        T3D_LOG_ERROR("RmlUiApp", "UISystem is not registered.");
        return T3D_ERR_FAIL;
    }

    T3D_LOG_INFO("RmlUiApp", "UISystem [%s] is ready.", uiSystem->getName().c_str());

#if defined(T3D_OS_ANDROID)
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_ANDROID, "", Archive::AccessMode::kRead);
#else
    ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_FS, Dir::getAppPath(), Archive::AccessMode::kRead);
#endif
    if (archive == nullptr)
    {
        T3D_LOG_ERROR("RmlUiApp", "Asset archive is not available.");
        return T3D_ERR_FAIL;
    }

    T3D_ASSET_MGR.init(AssetManager::Mode::kRuntime);
    T3D_ASSET_MGR.mount(archive, 0);

    ScenePtr scene = T3D_SCENE_MGR.createScene("RmlUiScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    GameObjectPtr rootObject = GameObject::create("Root");
    Transform3DPtr root = rootObject->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);

    RenderWindowPtr window = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr target = RenderTarget::create(window);

    GameObjectPtr cameraObject = GameObject::create("MainCamera");
    Transform3DPtr cameraTransform = cameraObject->addComponent<Transform3D>();
    root->addChild(cameraTransform);

    CameraPtr camera = cameraObject->addComponent<Camera>();
    camera->setViewport(Viewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});
    camera->setClearFlags(Camera::ClearFlags::kSolidColor);
    camera->setClearColor(ColorRGB(0.08f, 0.10f, 0.16f));
    camera->setRenderTarget(target);
    camera->setOrder(0);

    const Real aspect = Real(window->getDescriptor().Width) / Real(window->getDescriptor().Height);
    camera->setAspectRatio(aspect);
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);
    camera->lookAt(Vector3(0.0f, 0.0f, 8.0f), Vector3::ZERO, Vector3::UP);

    RmlCanvasPtr canvas = cameraObject->addComponent<RmlCanvas>();
    if (canvas == nullptr)
    {
        T3D_LOG_ERROR("RmlUiApp", "Failed to add RmlCanvas.");
        return T3D_ERR_FAIL;
    }

    TArray<String> documents;
    documents.push_back("assets/samples/ui/main.rml");
    canvas->setDocuments(documents);
    return T3D_OK;
}

bool RmlUiApp::pollEvents()
{
    // Phase 0 验收要观察退出顺序。设置 T3D_RMLUI_SMOKE=1 时启动后立即退出。
    const char *smoke = std::getenv("T3D_RMLUI_SMOKE");
    if (smoke != nullptr && smoke[0] == '1')
    {
        return false;
    }
    return SampleWindowApp::pollEvents();
}
