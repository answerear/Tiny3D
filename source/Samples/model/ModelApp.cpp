/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
*******************************************************************************/

#include "ModelApp.h"


ModelApp theApp;


using namespace Tiny3D;


ModelApp::ModelApp()
{

}

ModelApp::~ModelApp()
{

}

bool ModelApp::applicationDidFinishLaunching()
{
    Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
    renderer->setLightEnabled(true);
    renderer->setRenderMode(Renderer::E_RM_WIREFRAME);
    renderer->setAmbientLight(Color4::WHITE);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();
    
    // ����任���
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->lookAt(Vector3(-10.0f, 10.0f, 10.0f), Vector3(0.0f, 3.0f, 0.0f), Vector3::UNIT_Y);

    Real width = mRenderWindow->getWidth();
    Real height = mRenderWindow->getHeight();

    {
        // ������
        SGCameraPtr camera = SGCamera::create();
        node->addChild(camera);
        camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

        Radian fovY(Math::PI * Real(0.5) / 90 * 27);
        Real ratio = width / height;
        camera->setPerspective(fovY, ratio, 0.5, 10000.0);

        // �ӿ�
        ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
        viewport->setBackgroundColor(Color4::BLACK);
    }

    // ģ�� #1 �任���
    node = SGTransformNode::create();
    root->addChild(node);
    node->setPosition(0.0, 0.0, 0.0);
    {
        // ģ�� #1 �ɼ�������
        SGModelPtr model = SGModel::create("WarriorKnight.t3t");
        node->addChild(model);
    }

    return true;
}
