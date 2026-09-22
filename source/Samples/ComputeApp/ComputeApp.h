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

#ifndef __COMPUTE_APP_H__
#define __COMPUTE_APP_H__


#include "../Common/SampleApp.h"
#include "ComputeCases.h"
#include "ParticleSystem.h"

#include <vector>


/**
 * \brief Compute shader / UAV 能力验证 Sample
 * \remarks 设计见 doc/todo/ComputeApp-Sample-Design-todo.md。
 *          compute 只在 D3D11 Window、OpenGL4 (4.3+)、OpenGLES3 (3.1+) 上可用，
 *          其余后端是 T3D_RHI_UNSUPPORTED stub，本 Sample 靠能力位跳过而非崩溃。
 */
class ComputeApp : public SampleWindowApp
{
public:
    ComputeApp();
    ~ComputeApp() override;

protected:  /// from Tiny3D::Application
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationWillTerminate() override;

    /// 自检轨录制 GPU 命令的唯一合法位置：在 beginRender / endRender 之间
    void onRender() override;

    /// 自检轨回读比对的位置：unmap 会阻塞等 GPU，必须在 endRender 之后
    void onPostRender() override;

protected:
    /// 只建相机与清屏，compute 的 GPU 命令一律在 onRender 里手工录制，不进管线
    void buildScene();

    /**
     * \brief K0：把后端能力位原样打进日志
     * \remarks 后续用例据此决定跳过与否。这也是 RHI-Compute 文档 §12.7 建议的
     *          stub 密度审计手段 —— 号称完整的后端若能力位大面积为 false，日志里一眼可见。
     */
    void logCapabilities();

    /// 建用例、按能力位与后端掩码判定可跑性、跑 setup
    void setupSelfTest();

    /// 把每个用例的结论汇总打一遍
    void reportSelfTest();

    /// 热键：R 重跑自检，1-8 单跑，C CPU/GPU，V 剔除，P 暂停
    void pollKeys();

    /// 相机绕原点慢转，让 K10 可见数随时间变化
    void orbitCamera();

    /// 重跑自检：R 全部，或只跑指定下标
    void requestSelfTest(int32_t onlyIndex = -1);

protected:
    /// 一个用例在本轮自检中的状态
    struct CaseStatus
    {
        bool            runnable {false};
        String          skipReason;
        bool            setupOk {false};
        bool            recordOk {false};
        bool            passed {false};
    };

    std::vector<ComputeCasePtr> mCases;
    std::vector<CaseStatus>     mStatus;

    ParticleSystem              mParticles;
    Tiny3D::Camera             *mCamera {nullptr};
    Tiny3D::Transform3D        *mCameraXform {nullptr};

    /// 自检轨只在被请求的那一帧跑
    bool        mSelfTestPending {true};
    int32_t     mSelfTestOnly {-1};
    uint64_t    mKeyFrame {~uint64_t(0)};
};


#endif  /*__COMPUTE_APP_H__*/
