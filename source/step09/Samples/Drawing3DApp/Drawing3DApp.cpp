/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include "Drawing3DApp.h"


using namespace Tiny3D;


#define OBJ_MASK_SCENE         1


Drawing3DApp theApp;


Drawing3DApp::Drawing3DApp()
    : SampleApp()
{
}

Drawing3DApp::~Drawing3DApp()
{
}

bool Drawing3DApp::applicationDidFinishLaunching()
{
    SGNodePtr root = T3D_SCENE_MGR.getRoot();

    RenderWindowPtr renderWindow = T3D_AGENT.getDefaultRenderWindow();

    // 相机
    SGCameraPtr camera = T3D_SCENE_MGR.createCamera(root);
    camera->lookAt(Vector3(0.0f, 0.0f, 2.0f), Vector3::ZERO, Vector3::UNIT_Y);
    camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);
    size_t halfW = renderWindow->getWidth() >> 1;
    size_t halfH = renderWindow->getHeight() >> 1;
    camera->setProjectionParams(-halfW, halfW, -halfH, halfH, 0.5f, 1000.0f);
    camera->setObjectMask(OBJ_MASK_SCENE);

    // 视口
    ViewportPtr viewport = renderWindow->addViewport(camera, 1, 
        REAL_ZERO, REAL_ZERO, REAL_ONE, REAL_ONE);
    viewport->setBkgndColor(Color4::BLACK);

    // 箱子
    Vector3 extent(1.0f, 1.0f, 1.0f);
    SGBoxPtr box = T3D_SCENE_MGR.createBox(Vector3::ZERO, extent, root);
    box->setPosition(Vector3(-2.5f, 0.0f, 0.0f));
    box->setCameraMask(OBJ_MASK_SCENE);

    // 球
    SGSpherePtr sphere = T3D_SCENE_MGR.createSphere(Vector3::ZERO, 1.0f, root);
    sphere->setPosition(Vector3(2.0f, 0.0f, 0.0f));
    sphere->setCameraMask(OBJ_MASK_SCENE);

    // 空间四边形
    

    return true;
}


