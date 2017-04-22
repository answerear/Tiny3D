

#include "SkeletonApp.h"
#include <Tiny3D.h>


using namespace Tiny3D;


SkeletonApp::SkeletonApp()
{

}

SkeletonApp::~SkeletonApp()
{

}

int32_t SkeletonApp::go()
{
    Entrance *entrance = new Entrance();

    entrance->setApplicationListener(this);

    RenderWindow *renderWindow = nullptr;
    Entrance::getInstance().initialize(true, renderWindow);
    mRenderWindow = renderWindow;
    Entrance::getInstance().run();

    delete entrance;

    return 0;
}

bool SkeletonApp::applicationDidFinishLaunching()
{
    Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
    renderer->setLightEnabled(true);
    renderer->setRenderMode(Renderer::E_RM_WIREFRAME);
    renderer->setAmbientLight(Color4::WHITE);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();

    // 相机变换结点
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->lookAt(Vector3(0.0, -3.0, 0.0), Vector3::ZERO, Vector3::UNIT_Z);

    {
        // 相机结点
        SGCameraPtr camera = SGCamera::create();
        node->addChild(camera);
        camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

        Radian fovY(Math::PI * Real(0.5));
        Real ratio = Real(960) / Real(640);
        camera->setPerspective(fovY, ratio, 0.5, 1000.0);

        // 视口
        ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
        viewport->setBackgroundColor(Color4::BLACK);
    }

    // 模型 #1 变换结点
    node = SGTransformNode::create();
    root->addChild(node);
    node->setPosition(0.0, 0.0, 0.0);
    //     node->setOrientation(Radian(-Math::PI*0.5), Vector3::UNIT_X);
    node->setScale(0.02, 0.02, 0.02);
    {
        // 模型 #1 可见物体结点
        SGModelPtr model = SGModel::create("skeleton.tmt");
        model->setRenderMode(SGModel::E_RENDER_SKELETON);
        node->addChild(model);
        model->runAction("Take 001");
    }

    return true;
}

void SkeletonApp::applicationDidEnterBackground()
{

}

void SkeletonApp::applicationWillEnterForeground()
{

}

void SkeletonApp::applicationWillTerminate()
{

}
