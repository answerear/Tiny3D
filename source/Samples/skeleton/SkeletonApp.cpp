

#include "SkeletonApp.h"
#include <Tiny3D.h>


using namespace Tiny3D;

#define TEST_MODEL_TORTOISE             0
#define TEST_MODEL_CAMEL                0
#define TEST_MODEL_SKELETON             1


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

#if TEST_MODEL_CAMEL 
    node->lookAt(Vector3(150, 150, 150), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(50, 50, 50);
    root->addChild(indicator);

#elif TEST_MODEL_TORTOISE
    node->lookAt(Vector3(200, 1000, 200), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(100, 100, 100);
    root->addChild(indicator);

#elif TEST_MODEL_SKELETON
    node->lookAt(Vector3(150, 150, 150), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(50, 50, 50);
    root->addChild(indicator);
#endif

    {
        // 相机结点
        SGCameraPtr camera = SGCamera::create();
        node->addChild(camera);
        camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

        Radian fovY(Math::PI * Real(0.5));
        Real ratio = Real(960) / Real(640);
        camera->setPerspective(fovY, ratio, 0.5, 5000.0);

        // 视口
        ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
        viewport->setBackgroundColor(Color4::BLACK);
    }

#if TEST_MODEL_CAMEL
    // 模型 #1 变换结点
    node = SGTransformNode::create();
    root->addChild(node);
    node->setPosition(0.0, -166.487442, 76.205284);
    {
        // 模型 #1 可见物体结点
        SGModelPtr model = SGModel::create("白骆驼.tmt");
        model->setRenderMode(SGModel::E_RENDER_SKELETON);
        node->addChild(model);
//         model->runAction("Take 001");
    }
#elif TEST_MODEL_TORTOISE
    node = SGTransformNode::create();
    root->addChild(node);
    {
        SGModelPtr model = SGModel::create("tortoise.tmt");
        node->addChild(model);
        model->setRenderMode(SGModel::E_RENDER_SKELETON);
        model->runAction("Take 001");
    }
#elif TEST_MODEL_SKELETON
    node = SGTransformNode::create();
    root->addChild(node);
    {
        SGModelPtr model = SGModel::create("skeleton.tmt");
        node->addChild(model);
        model->setRenderMode(SGModel::E_RENDER_SKELETON);
        model->runAction("Take 001");
    }
#endif

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
