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

#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__


#include "ComputeKernel.h"

#include <vector>


/**
 * \brief K9 / K10 粒子可视轨
 * \remarks GPU 积分写结构化缓冲，uavBarrier 后 VS 当 SRV 读，renderInstanced 画
 *          billboard。按 C 切 CPU 参考实现；按 V 走视锥剔除 + copyStructureCount +
 *          renderIndexedIndirect。设计见 ComputeApp-Sample-Design-todo.md §5.2。
 */
class ParticleSystem
{
public:
    static const uint32_t kParticleCount = 65536;

    struct Particle
    {
        float position[3];
        float life;
        float velocity[3];
        float size;
    };

    static_assert(sizeof(Particle) == 32, "Particle must be 32 bytes (two float4s)");

    TResult setup(Tiny3D::Camera *camera);

    /**
     * \brief 释放全部 GPU 资源引用
     * \remarks 必须在 applicationWillTerminate 里调用。theApp 是全局对象，析构发生在
     *          main() 返回之后，那时 Agent 连同 RenderStateManager、SyncObject 都已经
     *          销毁，再走 Object::release 就是往已经析构的锁上加锁，直接访问违例。
     */
    void teardown();

    /// 在 onRender 里录制：积分（GPU 或 CPU）+ 可选剔除 + 绘制
    TResult record(float dt, float timeSeconds);

    /// 在 onPostRender 里消费 K10 的计数回读
    TResult consumeReadback();

    void setCpuMode(bool cpu) { mCpuMode = cpu; }
    bool isCpuMode() const { return mCpuMode; }

    void setCullEnabled(bool enabled) { mCullEnabled = enabled && mCullReady; }
    bool isCullEnabled() const { return mCullEnabled; }
    bool isCullReady() const { return mCullReady; }

    void setPaused(bool paused) { mPaused = paused; }
    bool isPaused() const { return mPaused; }

    bool isReady() const { return mReady; }

    uint32_t lastVisibleCount() const { return mLastVisibleCount; }

private:
    void integrateCpu(float dt, float timeSeconds, const float attractor[4]);
    void packMatrix(float *dst, const Tiny3D::Matrix4 &m) const;
    TResult uploadParticles();
    TResult uploadCb(Tiny3D::ConstantBuffer *cb, const void *data, size_t bytes);
    Tiny3D::ShaderVariantPtr compileGraphics(const char *name, Tiny3D::SHADER_STAGE stage,
        const ComputeShaderSource &source);
    bool isGLFamily() const;
    uint32_t uavSlot(uint32_t srvCount, uint32_t uRegister) const;

    /// 显式清空 CS 的 UAV 槽位，避免同一资源同时当 UAV 与 SRV
    static void unbindComputeUAVs(Tiny3D::RHIContext *ctx, uint32_t startSlot, uint32_t count);

    /// 绘制结束后清空 VS 的 SRV 槽位，避免下一帧当 UAV 绑回去时撞 hazard
    static void unbindVSStructuredBuffers(Tiny3D::RHIContext *ctx, uint32_t startSlot, uint32_t count);

    /// dispatch 结束后清空 CS 的 SRV 槽位，同上
    static void unbindCSStructuredBuffers(Tiny3D::RHIContext *ctx, uint32_t startSlot, uint32_t count);

    Tiny3D::Camera             *mCamera {nullptr};

    ComputeKernelPtr            mUpdateKernel;
    ComputeKernelPtr            mCullKernel;
    Tiny3D::ShaderVariantPtr    mDrawVS;
    Tiny3D::ShaderVariantPtr    mDrawCpuVS;
    Tiny3D::ShaderVariantPtr    mDrawPS;
    std::vector<Tiny3D::ShaderPtr> mDummyShaders;

    Tiny3D::StructuredBufferPtr mParticles;
    Tiny3D::StructuredBufferPtr mVisible;
    Tiny3D::StructuredBufferPtr mDrawArgs;
    Tiny3D::IndexBufferPtr      mIndexBuffer;
    Tiny3D::VertexBufferPtr     mCpuVB;
    Tiny3D::VertexDeclarationPtr mCpuDecl;
    Tiny3D::VertexDeclarationPtr mGpuDecl;

    Tiny3D::ConstantBufferPtr   mUpdateCB;
    Tiny3D::ConstantBufferPtr   mCullCB;
    Tiny3D::ConstantBufferPtr   mDrawCB;

    Tiny3D::RasterizerStatePtr      mRasterizer;
    Tiny3D::DepthStencilStatePtr    mDepthState;
    Tiny3D::BlendStatePtr           mBlendState;

    std::vector<Particle>       mCpuParticles;
    Tiny3D::ReadbackHandle      mCountHandle {};
    uint32_t                    mLastVisibleCount {kParticleCount};

    bool mReady {false};
    bool mCullReady {false};
    bool mCpuMode {false};
    bool mCullEnabled {false};
    bool mPaused {false};
    bool mPendingCountReadback {false};
};


#endif  /*__PARTICLE_SYSTEM_H__*/
