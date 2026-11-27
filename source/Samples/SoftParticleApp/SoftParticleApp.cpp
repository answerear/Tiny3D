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

#include "SoftParticleApp.h"
#include "SoftParticleBehaviour.h"


using namespace Tiny3D;

SoftParticleApp theApp;

//------------------------------------------------------------------------------

SoftParticleApp::SoftParticleApp()
{
}

SoftParticleApp::~SoftParticleApp()
{
}

TResult SoftParticleApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    ScenePtr scene = T3D_SCENE_MGR.createScene("SoftParticle");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    GameObjectPtr go = GameObject::create("Camera");
    Transform3DPtr xform = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(xform);

    CameraPtr camera = go->addComponent<Camera>();
    camera->setOrder(0);
    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(vp);
    camera->setClearColor(ColorRGB(0.03f, 0.04f, 0.08f));
    camera->setRenderTarget(rt);
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(60.0f);
    camera->lookAt(Vector3(6.0f, 2.0f, -4.0f), Vector3(0.5f, 1.2f, 0.5f), Vector3::UP);

    // The behaviour moves the camera along its orbit every frame
    go->addComponent<SoftParticleBehaviour>();

    return T3D_OK;
}

void SoftParticleApp::applicationWillTerminate()
{
}
