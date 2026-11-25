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

#include "SRGBApp.h"


using namespace Tiny3D;

SRGBApp theApp;

namespace
{
    /// The check passes have run at least once before this frame records the readbacks
    const uint32_t kReadbackFrame = 3;
}

//------------------------------------------------------------------------------

SRGBApp::SRGBApp()
{
}

SRGBApp::~SRGBApp()
{
}

TResult SRGBApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    ScenePtr scene = T3D_SCENE_MGR.createScene("SRGBChart");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    GameObjectPtr root = GameObject::create("SRGBChart");
    Transform3DPtr rootXform = root->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(rootXform);

    // The scene stays empty : the camera effect replaces the whole image with the chart
    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    GameObjectPtr go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    rootXform->addChild(xform);

    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB::BLACK);
    camera->setRenderTarget(RenderTarget::create(rw));
    camera->setProjectionType(Camera::Projection::kOrthographic);
    camera->setAspectRatio(Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height));
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(10.0f);
    go->addComponent<FrustumBound>();

    mTest = go->addComponent<SRGBTestBehaviour>();
    return T3D_OK;
}

void SRGBApp::applicationWillTerminate()
{
    mTest = nullptr;
}

void SRGBApp::onRender()
{
    if (mTest != nullptr && !mTest->isVerified() && mTest->getFramesRendered() >= kReadbackFrame)
    {
        mTest->requestReadback();
    }
}

void SRGBApp::onPostRender()
{
    if (mTest != nullptr && mTest->isReadbackPending())
    {
        mTest->verifyReadback();
    }
}
