/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
*******************************************************************************/

#include "FontApp.h"


FontApp theApp;

using namespace Tiny3D;


FontApp::FontApp()
{

}

FontApp::~FontApp()
{

}

bool FontApp::applicationDidFinishLaunching()
{
    Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
    renderer->setRenderMode(Renderer::E_RM_WIREFRAME);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();

    // 相机变换结点
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->lookAt(Vector3(-1.0, 1.0, 2.0), Vector3::ZERO, Vector3::UNIT_Y);

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

    T3D_FONT_MGR.setDefaultFontName("FZLanTingYuanS-DB1-GB.ttf");

    SGTransform2DPtr node1 = SGTransform2D::create();
    root->addChild(node1);

    node1->setPosition(Vector2(480, 320));

    {
        SGText2DPtr text = SGText2D::create("Hello world !", 10);
        node1->addChild(text);
    }

    return true;
}
