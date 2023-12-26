/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "CameraApp.h"

#include "Resource/T3DSceneManager.h"


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

