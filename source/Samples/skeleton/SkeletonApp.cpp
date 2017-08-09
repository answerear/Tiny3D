/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
*******************************************************************************/

#include "SkeletonApp.h"
#include <Tiny3D.h>
#include <sstream>


SkeletonApp theApp;


using namespace Tiny3D;

#define TEST_MODEL_TORTOISE             0
#define TEST_MODEL_CAMEL                1
#define TEST_MODEL_SKELETON             0
#define TEST_MODEL_KNIGHT               0
#define TEST_MODEL_SINBAD               0

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

bool SkeletonApp::applicationDidFinishLaunching()
{
    Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
    renderer->setLightEnabled(true);
    renderer->setRenderMode(Renderer::E_RM_WIREFRAME);
#if !TEST_MODEL_SKELETON //&& !TEST_MODEL_CAMEL && !TEST_MODEL_SINBAD
    renderer->setRenderMode(Renderer::E_RM_SOLID);
#endif
    renderer->setAmbientLight(Color4::WHITE);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();

    // ����任���
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->setName("Camera Transform");

#if TEST_MODEL_CAMEL || TEST_MODEL_SINBAD
    node->lookAt(Vector3(150, 50, 150), Vector3::ZERO, Vector3::UNIT_Y);
//     node->lookAt(Vector3(1509, 509, 1509), Vector3::ZERO, Vector3::UNIT_Y);

    // ����
    SGAxisPtr axis = SGAxis::create(50, 50, 50);
    root->addChild(axis);

#elif TEST_MODEL_TORTOISE
    node->lookAt(Vector3(-2500, 2000, 2500), Vector3::ZERO, Vector3::UNIT_Y);

    // ����
    SGIndicatorPtr indicator = SGIndicator::create(1000, 1000, 1000);
    root->addChild(indicator);

#elif TEST_MODEL_SKELETON
    node->lookAt(Vector3(400, 1000, 400), Vector3::ZERO, Vector3::UNIT_Y);

    // ����
    SGIndicatorPtr indicator = SGIndicator::create(50, 50, 50);
    root->addChild(indicator);
    indicator->setName("Indicator");

#elif TEST_MODEL_KNIGHT
    node->lookAt(Vector3(0, 20, 20), Vector3::ZERO, Vector3::UNIT_Y);

    // ����
    SGIndicatorPtr indicator = SGIndicator::create(10, 10, 10);
    root->addChild(indicator);

#endif

    {
        // ������
        SGCameraPtr camera = SGCamera::create();
        node->addChild(camera);
        camera->setName("Main Camera");
        camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

        Radian fovY(Math::PI * Real(0.5) / 90 * 27);

        Real ratio = Real(960) / Real(640);
        camera->setPerspective(fovY, ratio, 0.5, 10000);

        // �ӿ�
        ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
        viewport->setBackgroundColor(Color4::BLACK);
    }

#if TEST_MODEL_CAMEL
    // ģ�� #1 �任���
    node = SGTransformNode::create();
    root->addChild(node);
    node->setScale(0.2f, 0.2f, 0.2f);
    {
        // ģ�� #1 �ɼ�������
        SGModelPtr model = SGModel::create("������.t3t");
        node->addChild(model);
        model->runAction("�ܲ�");
    }
#elif TEST_MODEL_SINBAD
    // ģ�� #1 �任���
    node = SGTransformNode::create();
    root->addChild(node);
    node->setScale(8.0f, 8.0f, 8.0f);
    {
        // ģ�� #1 �ɼ�������
        SGModelPtr model = SGModel::create("Sinbad.t3t");
        node->addChild(model);
        model->runAction("Dance");
}
#elif TEST_MODEL_TORTOISE
    node = SGTransformNode::create();
    root->addChild(node);
//     node->setOrientation(Degree(-90), Vector3::UNIT_X);
    node->setPosition(0.0f, 0.0f, 0.0f);
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
