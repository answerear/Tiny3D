/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
***************************************************************************************************/

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
    renderer->setRenderMode(Renderer::E_RM_SOLID);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();

    // 相机变换结点
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->lookAt(Vector3(0.0, 0.0, 5.0), Vector3::ZERO, Vector3::UNIT_Y);

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

    size_t fontSize = 40;

    Real y = 4.0;
    SGTransformNodePtr node1 = SGTransformNode::create();
    root->addChild(node1);
    node1->setScale(0.02, 0.02, 0.02);
//     node1->setPosition(Vector2(480, 320));
    node1->setPosition(0, y, 0);
    {
        SGText2DPtr text = SGText2D::create(u8"你好，Hi ! 这里要写长点", fontSize);
//        SGText2DPtr text = SGText2D::create(u8"不知道", 40);
        node1->addChild(text);
    }

    y -= 2.0;
    node1 = SGTransformNode::create();
    root->addChild(node1);
    node1->setScale(0.02, 0.02, 0.02);
    node1->setPosition(0, y, 0);
    {
        SGText2DPtr text = SGText2D::create(u8"，再长点，让其超过上限，", fontSize);
        node1->addChild(text);
    }

    y -= 2.0;
    node1 = SGTransformNode::create();
    root->addChild(node1);
    node1->setScale(0.02, 0.02, 0.02);
    node1->setPosition(0, y, 0);
    {
        SGText2DPtr text = SGText2D::create(u8"重新分配纹理？", fontSize);
        node1->addChild(text);
    }

    y -= 2.0;
    node1 = SGTransformNode::create();
    root->addChild(node1);
    node1->setScale(0.02, 0.02, 0.02);
    node1->setPosition(0, y, 0);
    {
        SGText2DPtr text = SGText2D::create(u8"不过不知道这样子是否足够了", fontSize);
        node1->addChild(text);
    }

    y -= 2.0;
    node1 = SGTransformNode::create();
    root->addChild(node1);
    node1->setScale(0.02, 0.02, 0.02);
    node1->setPosition(0, y, 0);
    {
        SGText2DPtr text = SGText2D::create(u8"这是第一个字体显示程序！", fontSize);
        node1->addChild(text);
    }

    y -= 2.0;
    node1 = SGTransformNode::create();
    root->addChild(node1);
    node1->setScale(0.02, 0.02, 0.02);
    node1->setPosition(0, y, 0);
    {
        SGText2DPtr text = SGText2D::create(u8"Manual的解释：因为它能灵活定制，可以和其他类型Joint一起配合使用，还可以从其他已知版本的Joint汲取灵感，创造属于自己的Joint。\
            我的想法：其他Joint更多表示物体间的物理状态，而这个可以模拟人、人和物之间的物理状态，这里要模拟类似人的动作，所以用Configurable Joint。\
            2. 反复调Configurable Joint及Rigidbody相关参数，以达到一个自然的跳起后弹起的运动过程。注意先将Player放在Y轴合适位置（设置为1），然后开启Play Mode执行程序，这种调参数的方法以后会用到多次。\
            1) 首先调Y Drive（Y轴驱动力）（注1）。注意，先调Spring，数字越大，弹起越高；再调Damper，跟Spring相反，让物体停止震荡（），数字越大，物体就越快恢复初始位置。\
            注1：\
            如果你的版本是5.3以上，视频里给出的设置可能不太一样，我这边5.4调整的是Position Spring(弹性)，Positon Damper（减震器）。\
            注2：\
            变量越多越不好调，最好在清楚知道每个变量的含义下一次调节一个变量。\
            2) 然后调Rigidbody - Drag（注3），它设置空气阻力，空气阻力会影响物体受重力下落后的移动的距离和速度（就是下落快慢），数字无穷的话，物体就不会下落，因为和重力抵消了。\
            注3：\
            Unity - Manual: Rigidbody\
            3) 整体调出来的感觉大致是，模仿人跳起脱离地面，下落接触地面后双腿弯曲，最后双腿直立弹起表示缓冲。落到地面调节Spring，数字表示你能多快落到地面，双腿缓冲动作需要配合调节Damper和Drag，Drag表示腿弯曲的速度，Damper表示腿直起来的速度，最后还要配合调Y Drive - Maximum Force。\
            4) 调完后他用了个小技巧，可以避免你在运行时调节的参数，运行结束后没了，需要重新填一遍，那就是运行结束前Copy Component，再Paste到Prefab里。我试了下\
            3. 为地板加个Box Collider，这样Player如果弹到地板上，就会被地板挡住，不会穿过去。\
            4. 写代码。\
            代码只涉及两个类 : PlayerController和PlayerMotor，解决了三个功能：\
            1) 按下键盘空格实现向上调。\
            2) 落下时可控制Configurable Joint的参数。\
            3) 修改第一视角摄像机（Player内部Camera）在X轴的旋转范围", fontSize);
    }

    return true;
}
