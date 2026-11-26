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

#ifndef __T3D_GPU_PROFILER_H__
#define __T3D_GPU_PROFILER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 基于时间戳查询的 GPU 分段计时器
     * \remarks 每帧在 beginFrame / endFrame 之间用 beginScope / endScope 标出若干段，
     *      结果在若干帧之后才能拿到，getTimings 返回的是最近一帧已完成的数据。
     *      查询按帧槽轮转复用，任何读取都不等 GPU。所有调用都在主线程，
     *      并且必须位于 RHIContext::beginRender / endRender 之间
     */
    class T3D_ENGINE_API GPUProfiler : public Object
    {
    public:
        /// 一段计时结果
        struct Timing
        {
            String      name {};            ///< 段名
            uint32_t    depth {0};          ///< 嵌套深度，0 为最外层
            uint32_t    callCount {0};      ///< 同名同深度的段在该帧出现的次数
            double      milliseconds {0.0}; ///< 累计 GPU 耗时（毫秒）
        };

        using Timings = TArray<Timing>;

        /// 同时在途的帧数，超过这个延迟还没完成的帧会被丢弃
        static constexpr uint32_t kFrameLatency = 5;

        /// 单帧最大段数，超出的段只打调试分组不计时
        static constexpr uint32_t kMaxScopesPerFrame = 64;

        static GPUProfilerPtr create();

        ~GPUProfiler() override;

        /**
         * \brief 开始一帧计时，并尝试回收之前帧的结果
         * \param [in] ctx : RHI 上下文；后端不支持时间戳时整个分析器变成空操作
         */
        void beginFrame(RHIContext *ctx);

        /**
         * \brief 结束一帧计时，未闭合的段会被自动闭合
         */
        void endFrame(RHIContext *ctx);

        /**
         * \brief 开始一段计时，可嵌套
         * \param [in] ctx : RHI 上下文
         * \param [in] name : 段名；同一帧内同名同深度的段会被合并
         */
        void beginScope(RHIContext *ctx, const String &name);

        /**
         * \brief 结束最近一次 beginScope 开始的段
         */
        void endScope(RHIContext *ctx);

        /// 后端是否支持时间戳查询；首次 beginFrame 之后才有意义
        bool isSupported() const { return mSupported; }

        /// 最近一帧已完成的分段计时，按首次出现顺序排列
        const Timings &getTimings() const { return mTimings; }

        /// 最近一帧已完成的整帧 GPU 耗时（毫秒）
        double getFrameTime() const { return mFrameTime; }

        /// 最近一帧已完成结果对应的帧序号；0 表示还没有结果
        uint64_t getResolvedFrameIndex() const { return mResolvedFrameIndex; }

        /// 因结果超时或 GPU 时钟不连续而丢弃的帧数
        uint64_t getDroppedFrameCount() const { return mDroppedFrames; }

    protected:
        GPUProfiler() = default;

        struct Scope
        {
            String      name {};
            uint32_t    depth {0};
            RHIQueryPtr begin {nullptr};
            RHIQueryPtr end {nullptr};
            bool        closed {false};
        };

        struct Frame
        {
            TArray<Scope>   scopes {};
            size_t          used {0};
            RHIQueryPtr     frameBegin {nullptr};
            RHIQueryPtr     frameEnd {nullptr};
            uint64_t        frameIndex {0};
            bool            pending {false};
        };

        enum class FrameState : uint32_t
        {
            kReady,
            kPending,
            kFailed,
        };

        /// 查询当前帧槽的结果是否齐全；齐全时写入 mTimings
        FrameState tryResolve(RHIContext *ctx, Frame &frame);

        bool ensureQueries(RHIContext *ctx, Frame &frame);

        Frame       mFrames[kFrameLatency] {};
        uint32_t    mCurrentFrame {0};
        bool        mInFrame {false};
        bool        mSupported {true};
        bool        mChecked {false};
        uint64_t    mFrameCounter {0};
        TArray<size_t>  mScopeStack {};

        Timings     mTimings {};
        double      mFrameTime {0.0};
        uint64_t    mResolvedFrameIndex {0};
        uint64_t    mDroppedFrames {0};
    };
}


#endif  /*__T3D_GPU_PROFILER_H__*/
