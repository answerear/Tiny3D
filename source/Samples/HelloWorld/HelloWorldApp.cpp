

#include "HelloWorldApp.h"
#include <Tiny3D.h>


using namespace Tiny3D;

HelloWorldApp::HelloWorldApp()
{

}

HelloWorldApp::~HelloWorldApp()
{

}

int32_t HelloWorldApp::go()
{
    Entrance *entrance = new Entrance("../../../../media/config/Tiny3D.cfg");

    entrance->setApplicationListener(this);

    RenderWindow *renderWindow = nullptr;
    Entrance::getInstance().initialize(true, renderWindow);
    mRenderWindow = renderWindow;
    Entrance::getInstance().run();

    delete entrance;

    return 0;
}

bool HelloWorldApp::applicationDidFinishLaunching()
{
    T3D_ENTRANCE.getActiveRenderer()->setRenderMode(Renderer::E_RM_WIREFRAME);
//     T3D_ENTRANCE.getActiveRenderer()->setCullingMode(Renderer::E_CULL_NONE);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();
    
    // 相机变换结点
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);

    node->lookAt(Vector3(0.0, 2.0, 4.0), Vector3::ZERO, Vector3::UNIT_Y);

    // 相机结点
    SGCameraPtr camera = SGCamera::create();
    node->addChild(camera);

    camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

    Radian fovY(Math::PI * Real(0.5));
    Real ratio = Real(960) / Real(640);
    camera->setPerspective(fovY, ratio, 1.0, 1000.0);

    // 立方体变换结点
    node = SGTransformNode::create();
    root->addChild(node);
    node->setPosition(1.0, 0.0, 0.0);
//     node->setOrientation(Radian(Math::PI * Real(0.5)), Vector3(0.0, 1.0, 0.0));

    SGTransformNodePtr node1 = SGTransformNode::create();
    node->addChild(node1);
    node1->setPosition(2.0, 0.0, 0.0);
//     node1->setScale(1.2, 1.2, 1.2);
    node1->setOrientation(Radian(Math::PI / Real(6.0)), Vector3(0.0, 1.0, 0.0));

    // 立方体 #1 mesh
    SGMeshPtr mesh = SGMesh::create();
    node1->addChild(mesh);
    mesh->loadBox();

    SGTransformNodePtr node2 = SGTransformNode::create();
    node->addChild(node2);
    node2->setPosition(-1.0, 0.0, 0.0);

    // #2 Mesh
    mesh = SGMesh::create();
    node2->addChild(mesh);
    mesh->loadBox();

    SGTransformNodePtr node3 = SGTransformNode::create();
    node->addChild(node3);
    node3->setPosition(-4.0, 0.0, 0.0);

    // #3 Mesh
    mesh = SGMesh::create();
    node3->addChild(mesh);
    mesh->loadSphere();

    node = SGTransformNode::create();
    root->addChild(node);
    node->setPosition(-10.0, 0.0, 0.0);

    mesh = SGMesh::create();
    node->addChild(mesh);
    mesh->loadBox();

    ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
    viewport->setBackgroundColor(Color4::BLACK);

    return true;
}

void HelloWorldApp::applicationDidEnterBackground()
{

}

void HelloWorldApp::applicationWillEnterForeground()
{

}

void HelloWorldApp::applicationWillTerminate()
{

}
