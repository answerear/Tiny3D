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

#include "CameraApp.h"


using namespace Tiny3D;


CameraApp theApp;


CameraApp::CameraApp()
{
}

CameraApp::~CameraApp()
{
}

bool CameraApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    // create scene
    ScenePtr scene = T3D_SCENE_MGR.createScene("TestScene");
    T3D_SCENE_MGR.setCurrentScene(scene);
    
    // root game object
    GameObjectPtr go = GameObject::create("TestScene");
    scene->addRootGameObject(go);
    Transform3DPtr root = go->addComponent<Transform3D>();

    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);
    
    CameraPtr camera;

    // center camera
    go = GameObject::create("CenterCamera");
    Transform3DPtr center = go->addComponent<Transform3D>();
    root->addChild(center);
    camera = go->addComponent<Camera>();
    camera->setOrder(1);
    Viewport vpCenter {0.25f, 0.25f, 0.5f, 0.5f, 0.0f, 1.0f};
    camera->setViewport(vpCenter);
    camera->setClearColor(ColorRGB::RED);
    camera->setRenderTarget(rt);
    scene->addCamera(camera);
    
    // left camera
    go = GameObject::create("LeftCamera");
    Transform3DPtr left = go->addComponent<Transform3D>();
    root->addChild(left);
    camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vpLeft {0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vpLeft);
    camera->setClearColor(ColorRGB::GREEN);
    camera->setRenderTarget(rt);
    scene->addCamera(camera);

    // right camera
    go = GameObject::create("RightCamera");
    Transform3DPtr right = go->addComponent<Transform3D>();
    root->addChild(right);    
    camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vpRight {0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f};    
    camera->setViewport(vpRight);
    camera->setClearColor(ColorRGB::BLUE);
    camera->setRenderTarget(rt);
    scene->addCamera(camera);

    return true;
}

void CameraApp::applicationWillTerminate() 
{
    
}

