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

#include "ComputeApp.h"

#include <cmath>


using namespace Tiny3D;


ComputeApp theApp;


ComputeApp::ComputeApp()
{
}

ComputeApp::~ComputeApp()
{
}

TResult ComputeApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    logCapabilities();
    buildScene();
    setupSelfTest();

    const TResult particleRet = mParticles.setup(mCamera);
    if (T3D_FAILED(particleRet))
    {
        APP_LOG_DEBUG("[ComputeApp] visual track setup failed, window will still run self-test.");
    }

    APP_LOG_DEBUG("[ComputeApp] keys: R retest, 1-8 single case, C CPU/GPU, V cull+indirect, P pause");
    return T3D_OK;
}

void ComputeApp::applicationWillTerminate()
{
    // theApp 是全局对象，它的析构在 main() 返回之后才跑，那时引擎已经拆干净了。
    // 所有指向 GPU 资源的智能指针都必须在这里断掉，晚一步 release 就是访问违例。
    mParticles.teardown();
    mCases.clear();
    mStatus.clear();
    mCamera = nullptr;
    mCameraXform = nullptr;
}

void ComputeApp::pollKeys()
{
    if (Input::getInstancePtr() == nullptr)
    {
        return;
    }

    const uint64_t frame = Time::frameCount();
    if (frame == mKeyFrame)
    {
        return;
    }

    auto pressed = [&](ScanCode code) -> bool
    {
        return T3D_INPUT.getKeyDown(code);
    };

    if (pressed(APP_SCANCODE_R))
    {
        requestSelfTest(-1);
        mKeyFrame = frame;
        return;
    }

    const ScanCode digitKeys[8] =
    {
        APP_SCANCODE_1, APP_SCANCODE_2, APP_SCANCODE_3, APP_SCANCODE_4,
        APP_SCANCODE_5, APP_SCANCODE_6, APP_SCANCODE_7, APP_SCANCODE_8
    };
    for (int32_t i = 0; i < 8; ++i)
    {
        if (pressed(digitKeys[i]))
        {
            requestSelfTest(i);
            mKeyFrame = frame;
            return;
        }
    }

    if (pressed(APP_SCANCODE_C))
    {
        const bool cpu = !mParticles.isCpuMode();
        mParticles.setCpuMode(cpu);
        APP_LOG_DEBUG("[ComputeApp] particle mode -> %s", cpu ? "CPU" : "GPU");
        mKeyFrame = frame;
    }
    else if (pressed(APP_SCANCODE_V))
    {
        if (!mParticles.isCullReady())
        {
            APP_LOG_DEBUG("[ComputeApp] GPU cull not available on this backend.");
        }
        else
        {
            const bool cull = !mParticles.isCullEnabled();
            mParticles.setCullEnabled(cull);
            APP_LOG_DEBUG("[ComputeApp] GPU cull+indirect -> %s", cull ? "ON" : "OFF");
        }
        mKeyFrame = frame;
    }
    else if (pressed(APP_SCANCODE_P))
    {
        const bool paused = !mParticles.isPaused();
        mParticles.setPaused(paused);
        APP_LOG_DEBUG("[ComputeApp] particle integrate -> %s", paused ? "paused" : "running");
        mKeyFrame = frame;
    }
}

void ComputeApp::requestSelfTest(int32_t onlyIndex)
{
    mSelfTestPending = true;
    mSelfTestOnly = onlyIndex;
    if (onlyIndex < 0)
    {
        APP_LOG_DEBUG("[ComputeApp] re-running all self-test cases ...");
    }
    else
    {
        APP_LOG_DEBUG("[ComputeApp] re-running case index %d ...", onlyIndex);
    }
}

void ComputeApp::orbitCamera()
{
    if (mCamera == nullptr)
    {
        return;
    }

    const float t = float(Time::time()) * 0.00015f;
    const Vector3 eye(cosf(t) * 12.0f, 5.5f, sinf(t) * 12.0f);
    mCamera->lookAt(eye, Vector3::ZERO, Vector3::UP);
}

void ComputeApp::onRender()
{
    pollKeys();
    orbitCamera();

    if (mSelfTestPending)
    {
        for (size_t i = 0; i < mCases.size(); ++i)
        {
            CaseStatus &status = mStatus[i];
            status.recordOk = false;
            status.passed = false;

            if (!status.runnable || !status.setupOk)
            {
                continue;
            }
            if (mSelfTestOnly >= 0 && int32_t(i) != mSelfTestOnly)
            {
                continue;
            }

            const TResult ret = mCases[i]->record();
            status.recordOk = T3D_SUCCEEDED(ret);
            if (!status.recordOk)
            {
                APP_LOG_DEBUG("[%s] record failed ! ERROR [%d]", mCases[i]->getName(), ret);
            }
        }
        return;
    }

    if (!mParticles.isReady())
    {
        return;
    }

    const float dt = float(Time::deltaTime()) * 0.001f;
    const float timeSeconds = float(Time::time()) * 0.001f;
    const TResult ret = mParticles.record(dt > 0.0f ? dt : 0.016f, timeSeconds);
    if (T3D_FAILED(ret))
    {
        APP_LOG_DEBUG("[particles] record failed ! ERROR [%d]", ret);
    }
}

void ComputeApp::onPostRender()
{
    if (mSelfTestPending)
    {
        for (size_t i = 0; i < mCases.size(); ++i)
        {
            CaseStatus &status = mStatus[i];
            if (!status.recordOk)
            {
                continue;
            }
            if (mSelfTestOnly >= 0 && int32_t(i) != mSelfTestOnly)
            {
                continue;
            }

            status.passed = T3D_SUCCEEDED(mCases[i]->verify());
        }

        mSelfTestPending = false;
        mSelfTestOnly = -1;
        reportSelfTest();
        return;
    }

    if (mParticles.isReady())
    {
        mParticles.consumeReadback();
    }
}

void ComputeApp::logCapabilities()
{
    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
    RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

    if (ctx == nullptr)
    {
        APP_LOG_DEBUG("[K0] no active RHI context !");
        return;
    }

    const String rendererName = (renderer != nullptr) ? renderer->getName() : "<unknown>";
    const RHICapabilities &caps = ctx->getCapabilities();

    APP_LOG_DEBUG("[K0] ---- RHI capabilities (%s) ----", rendererName.c_str());
    APP_LOG_DEBUG("[K0] compute=%d unorderedAccess=%d structuredBuffer=%d",
        caps.supportsCompute, caps.supportsUnorderedAccess, caps.supportsStructuredBuffer);
    APP_LOG_DEBUG("[K0] instancing=%d baseInstance=%d",
        caps.supportsInstancing, caps.supportsBaseInstance);
    APP_LOG_DEBUG("[K0] indirectDraw=%d indirectDispatch=%d appendConsume=%d readback=%d",
        caps.supportsIndirectDraw, caps.supportsIndirectDispatch,
        caps.supportsAppendConsumeBuffer, caps.supportsReadback);
    APP_LOG_DEBUG("[K0] maxDispatchGroupCount=[%u, %u, %u]",
        caps.maxDispatchGroupCount[0], caps.maxDispatchGroupCount[1], caps.maxDispatchGroupCount[2]);
    APP_LOG_DEBUG("[K0] maxComputeGroupSize=[%u, %u, %u]",
        caps.maxComputeGroupSize[0], caps.maxComputeGroupSize[1], caps.maxComputeGroupSize[2]);
    APP_LOG_DEBUG("[K0] maxComputeSharedMemory=%u bytes  maxUnorderedAccessSlots=%u",
        caps.maxComputeSharedMemory, caps.maxUnorderedAccessSlots);

    if (!caps.supportsCompute)
    {
        APP_LOG_DEBUG("[K0] compute is NOT supported by [%s], all compute cases will be skipped.",
            rendererName.c_str());
    }
}

void ComputeApp::setupSelfTest()
{
    buildComputeCases(mCases);
    mStatus.assign(mCases.size(), CaseStatus());

    RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
    if (ctx == nullptr)
    {
        return;
    }

    const RHICapabilities &caps = ctx->getCapabilities();
    const uint32_t backendBit = currentBackendBit();

    for (size_t i = 0; i < mCases.size(); ++i)
    {
        CaseStatus &status = mStatus[i];
        status.runnable = isCaseRunnable(*mCases[i], caps, backendBit, status.skipReason);

        if (!status.runnable)
        {
            APP_LOG_DEBUG("[%s] SKIP : %s", mCases[i]->getName(), status.skipReason.c_str());
            continue;
        }

        const TResult ret = mCases[i]->setup();
        status.setupOk = T3D_SUCCEEDED(ret);
        if (!status.setupOk)
        {
            APP_LOG_DEBUG("[%s] setup failed ! ERROR [%d]", mCases[i]->getName(), ret);
        }
    }
}

void ComputeApp::reportSelfTest()
{
    uint32_t passed = 0;
    uint32_t failed = 0;
    uint32_t skipped = 0;

    APP_LOG_DEBUG("---- compute self test result ----");

    for (size_t i = 0; i < mCases.size(); ++i)
    {
        const CaseStatus &status = mStatus[i];
        const char *name = mCases[i]->getName();

        if (!status.runnable)
        {
            APP_LOG_DEBUG("  SKIP  %s  (%s)", name, status.skipReason.c_str());
            ++skipped;
        }
        else if (status.passed)
        {
            APP_LOG_DEBUG("  PASS  %s", name);
            ++passed;
        }
        else
        {
            APP_LOG_DEBUG("  FAIL  %s", name);
            ++failed;
        }
    }

    APP_LOG_DEBUG("---- %u passed, %u failed, %u skipped ----", passed, failed, skipped);
    if (failed == 0)
    {
        APP_LOG_DEBUG("[ComputeApp] K1..K8 all passed");
    }
}

void ComputeApp::buildScene()
{
    ScenePtr scene = T3D_SCENE_MGR.createScene("ComputeScene");
    scene->init();
    T3D_SCENE_MGR.setCurrentScene(scene);

    GameObjectPtr go = GameObject::create("Root");
    Transform3DPtr root = go->addComponent<Transform3D>();
    scene->getRootTransform()->addChild(root);

    RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
    RenderTargetPtr rt = RenderTarget::create(rw);

    go = GameObject::create("MainCamera");
    Transform3DPtr camXform = go->addComponent<Transform3D>();
    root->addChild(camXform);

    CameraPtr camera = go->addComponent<Camera>();
    Viewport viewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    camera->setViewport(viewport);
    camera->setClearColor(ColorRGB(0.08f, 0.10f, 0.16f));
    camera->setRenderTarget(rt);
    camera->setOrder(0);

    const Real as = Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
    camera->setAspectRatio(as);
    camera->setProjectionType(Camera::Projection::kPerspective);
    camera->setFovY(Radian(Math::PI / 3.0f));
    camera->setNearPlaneDistance(0.1f);
    camera->setFarPlaneDistance(1000.0f);

    const Vector3 eye(12.0f, 5.5f, 0.0f);
    camera->lookAt(eye, Vector3::ZERO, Vector3::UP);

    mCamera = camera.get();
    mCameraXform = camXform.get();
}
