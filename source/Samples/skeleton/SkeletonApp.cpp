

#include "SkeletonApp.h"
#include <Tiny3D.h>
#include <sstream>

using namespace Tiny3D;

#define TEST_MODEL_TORTOISE             0
#define TEST_MODEL_CAMEL                1
#define TEST_MODEL_SKELETON             0
#define TEST_MODEL_KNIGHT               0

void printNode(NodePtr node, size_t tabCount)
{
    std::stringstream ss;
    size_t i = 0;
    for (i = 0; i < tabCount; ++i)
    {
        ss<<"    ";
    }

    T3D_LOG_DEBUG("%sNode [%u] Type %d, name : %s", ss.str().c_str(), node->getNodeID(), node->getNodeType(), node->getName().c_str());
    if (node->getNodeType() == Node::E_NT_TRANSFORM || node->getNodeType() == Node::E_NT_BONE)
    {
        SGTransformNodePtr transform = smart_pointer_cast<SGTransformNode>(node);
        Matrix4 T(transform->getPosition());
        Matrix4 R(transform->getOrientation());
        Matrix4 S(transform->getScale().x(), transform->getScale().y(), transform->getScale().z(), 1.0f);
        Matrix4 M = T * R * S;
        T3D_LOG_DEBUG("%sTransform [ %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ]", ss.str().c_str(),
            M[0][0], M[0][1], M[0][2], M[0][3],
            M[1][0], M[1][1], M[1][2], M[1][3],
            M[2][0], M[2][1], M[2][2], M[2][3],
            M[3][0], M[3][1], M[3][2], M[3][3]);
    }

    ++tabCount;
    auto itr = node->getChildren().begin();
    while (itr != node->getChildren().end())
    {
        auto child = *itr;
        printNode(child, tabCount);
        ++itr;
    }
}

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
#if !TEST_MODEL_SKELETON && !TEST_MODEL_CAMEL
    renderer->setRenderMode(Renderer::E_RM_SOLID);
#endif
    renderer->setAmbientLight(Color4::WHITE);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();

    // 相机变换结点
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->setName("Camera Transform");

#if TEST_MODEL_CAMEL 
    node->lookAt(Vector3(150, 150, 150), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(50, 50, 50);
    root->addChild(indicator);

#elif TEST_MODEL_TORTOISE
    node->lookAt(Vector3(-2500, 2000, 2500), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(1000, 1000, 1000);
    root->addChild(indicator);

#elif TEST_MODEL_SKELETON
    node->lookAt(Vector3(400, 1000, 400), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(50, 50, 50);
    root->addChild(indicator);
    indicator->setName("Indicator");

#elif TEST_MODEL_KNIGHT
    node->lookAt(Vector3(0, 20, 20), Vector3::ZERO, Vector3::UNIT_Y);

    // 坐标
    SGIndicatorPtr indicator = SGIndicator::create(10, 10, 10);
    root->addChild(indicator);

#endif

    {
        // 相机结点
        SGCameraPtr camera = SGCamera::create();
        node->addChild(camera);
        camera->setName("Main Camera");
        camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

        Radian fovY(Math::PI * Real(0.5) / 90 * 27);
        Real ratio = Real(960) / Real(640);
        camera->setPerspective(fovY, ratio, 0.5, 10000);

        // 视口
        ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
        viewport->setBackgroundColor(Color4::BLACK);
    }

#if TEST_MODEL_CAMEL
    // 模型 #1 变换结点
    node = SGTransformNode::create();
    root->addChild(node);
//     node->setPosition(0.0, -166.487442, 76.205284);
    node->setScale(0.2f, 0.2f, 0.2f);
    {
        // 模型 #1 可见物体结点
        SGModelPtr model = SGModel::create("白骆驼.t3t");
//         model->setRenderMode(SGModel::E_RENDER_SKELETON);
        node->addChild(model);
        model->runAction("站立");
    }
#elif TEST_MODEL_TORTOISE
    node = SGTransformNode::create();
    root->addChild(node);
//     node->setOrientation(Degree(-90), Vector3::UNIT_X);
    node->setPosition(0.0f, 500.0f, 0.0f);
    {
        SGModelPtr model = SGModel::create("tortoise.t3t");
        node->addChild(model);
//         model->setRenderMode(SGModel::E_RENDER_SKELETON);
        model->runAction("Take 001");
    }
#elif TEST_MODEL_SKELETON
    node = SGTransformNode::create();
    node->setName("Default");
    root->addChild(node);
    Quaternion Q1, Q2;
    Degree degree1(-90.0f);
    Q1.fromAngleAxis(degree1, Vector3::UNIT_X);
    Degree degree2(11.0f);
    Q2.fromAngleAxis(degree2, Vector3::UNIT_Z);
    Quaternion Q = Q2 * Q1;
    Matrix4 R(Q);

    Matrix4 M(
        2.493333, 0.000000, -0.484655, 0.000000,
        0.484655, 0.000000, 2.493333, 0.000000,
        0.000000, -2.540000, 0.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 1.000000);
    Vector3 t, s;
    Quaternion r;
    M.decomposition(t, s, r);

//     node->setOrientation(Q);
//     auto node1 = SGTransformNode::create();
//     node->addChild(node1);
//     node1->setOrientation(Q1);
    {
        SGModelPtr model = SGModel::create("skeleton.t3t");
        node->addChild(model);
//         model->setRenderMode(SGModel::E_RENDER_SKELETON);
        model->runAction("Take 001");
    }
#elif TEST_MODEL_KNIGHT
    node = SGTransformNode::create();
    root->addChild(node);
    {
        SGModelPtr model = SGModel::create("knight.t3t");
        node->addChild(model);
        model->runAction("Walk");
    }
#endif

    printNode(root, 0);

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
