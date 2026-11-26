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


#include "Render/T3DGPUProfiler.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIQuery.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
        constexpr size_t kOverflowScope = static_cast<size_t>(-1);

        double toMilliseconds(uint64_t begin, uint64_t end)
        {
            if (end <= begin)
            {
                return 0.0;
            }
            return static_cast<double>(end - begin) / 1000000.0;
        }
    }

    //--------------------------------------------------------------------------

    GPUProfilerPtr GPUProfiler::create()
    {
        return T3D_NEW GPUProfiler();
    }

    //--------------------------------------------------------------------------

    GPUProfiler::~GPUProfiler()
    {
    }

    //--------------------------------------------------------------------------

    bool GPUProfiler::ensureQueries(RHIContext *ctx, Frame &frame)
    {
        if (frame.frameBegin == nullptr)
        {
            frame.frameBegin = ctx->createQuery(QueryType::kTimestamp);
        }
        if (frame.frameEnd == nullptr)
        {
            frame.frameEnd = ctx->createQuery(QueryType::kTimestamp);
        }
        return frame.frameBegin != nullptr && frame.frameEnd != nullptr;
    }

    //--------------------------------------------------------------------------

    void GPUProfiler::beginFrame(RHIContext *ctx)
    {
        if (ctx == nullptr)
        {
            return;
        }

        if (!mChecked)
        {
            mChecked = true;
            mSupported = ctx->getCapabilities().supportsTimestampQuery;
            if (!mSupported)
            {
                T3D_LOG_WARNING(LOG_TAG_RENDER, "GPUProfiler : timestamp query is not supported by this RHI backend, GPU profiling disabled");
            }
        }

        if (!mSupported)
        {
            return;
        }

        if (mInFrame)
        {
            endFrame(ctx);
        }

        // 按帧序从旧到新回收；旧帧没好时新帧也不看，保证结果单调前进
        while (true)
        {
            Frame *oldest = nullptr;
            for (Frame &frame : mFrames)
            {
                if (frame.pending && (oldest == nullptr || frame.frameIndex < oldest->frameIndex))
                {
                    oldest = &frame;
                }
            }

            if (oldest == nullptr)
            {
                break;
            }

            FrameState state = tryResolve(ctx, *oldest);
            if (state == FrameState::kPending)
            {
                break;
            }

            oldest->pending = false;
            if (state == FrameState::kFailed)
            {
                mDroppedFrames++;
            }
        }

        mCurrentFrame = static_cast<uint32_t>(mFrameCounter % kFrameLatency);
        Frame &frame = mFrames[mCurrentFrame];
        if (frame.pending)
        {
            // 在途帧数超过了 kFrameLatency，这一槽的结果来不及读就要被覆盖
            frame.pending = false;
            mDroppedFrames++;
        }

        if (!ensureQueries(ctx, frame))
        {
            mSupported = false;
            return;
        }

        mFrameCounter++;
        frame.frameIndex = mFrameCounter;
        frame.used = 0;
        mScopeStack.clear();

        ctx->endQuery(frame.frameBegin);
        mInFrame = true;
    }

    //--------------------------------------------------------------------------

    void GPUProfiler::endFrame(RHIContext *ctx)
    {
        if (!mInFrame || ctx == nullptr)
        {
            return;
        }

        while (!mScopeStack.empty())
        {
            endScope(ctx);
        }

        Frame &frame = mFrames[mCurrentFrame];
        ctx->endQuery(frame.frameEnd);
        frame.pending = true;
        mInFrame = false;
    }

    //--------------------------------------------------------------------------

    void GPUProfiler::beginScope(RHIContext *ctx, const String &name)
    {
        if (!mInFrame || ctx == nullptr)
        {
            return;
        }

        Frame &frame = mFrames[mCurrentFrame];
        if (frame.used >= kMaxScopesPerFrame)
        {
            mScopeStack.push_back(kOverflowScope);
            return;
        }

        if (frame.scopes.size() <= frame.used)
        {
            frame.scopes.resize(frame.used + 1);
        }

        Scope &scope = frame.scopes[frame.used];
        if (scope.begin == nullptr)
        {
            scope.begin = ctx->createQuery(QueryType::kTimestamp);
        }
        if (scope.end == nullptr)
        {
            scope.end = ctx->createQuery(QueryType::kTimestamp);
        }
        if (scope.begin == nullptr || scope.end == nullptr)
        {
            mScopeStack.push_back(kOverflowScope);
            return;
        }

        scope.name = name;
        scope.depth = static_cast<uint32_t>(mScopeStack.size());
        scope.closed = false;
        ctx->endQuery(scope.begin);
        mScopeStack.push_back(frame.used);
        frame.used++;
    }

    //--------------------------------------------------------------------------

    void GPUProfiler::endScope(RHIContext *ctx)
    {
        if (!mInFrame || ctx == nullptr || mScopeStack.empty())
        {
            return;
        }

        size_t index = mScopeStack.back();
        mScopeStack.pop_back();
        if (index == kOverflowScope)
        {
            return;
        }

        Scope &scope = mFrames[mCurrentFrame].scopes[index];
        ctx->endQuery(scope.end);
        scope.closed = true;
    }

    //--------------------------------------------------------------------------

    GPUProfiler::FrameState GPUProfiler::tryResolve(RHIContext *ctx, Frame &frame)
    {
        auto check = [](QueryStatus status) -> FrameState
        {
            switch (status)
            {
            case QueryStatus::kReady:
                return FrameState::kReady;
            case QueryStatus::kPending:
                return FrameState::kPending;
            default:
                return FrameState::kFailed;
            }
        };

        QueryResult frameBegin, frameEnd;
        FrameState state = check(ctx->resolveQuery(frame.frameBegin, frameBegin));
        if (state != FrameState::kReady)
        {
            return state;
        }
        state = check(ctx->resolveQuery(frame.frameEnd, frameEnd));
        if (state != FrameState::kReady)
        {
            return state;
        }

        Timings timings;
        for (size_t i = 0; i < frame.used; ++i)
        {
            const Scope &scope = frame.scopes[i];
            if (!scope.closed)
            {
                continue;
            }

            QueryResult begin, end;
            state = check(ctx->resolveQuery(scope.begin, begin));
            if (state != FrameState::kReady)
            {
                return state;
            }
            state = check(ctx->resolveQuery(scope.end, end));
            if (state != FrameState::kReady)
            {
                return state;
            }

            const double ms = toMilliseconds(begin.timestamp, end.timestamp);
            auto itr = std::find_if(timings.begin(), timings.end(),
                [&scope](const Timing &t)
                {
                    return t.depth == scope.depth && t.name == scope.name;
                });
            if (itr == timings.end())
            {
                Timing timing;
                timing.name = scope.name;
                timing.depth = scope.depth;
                timing.callCount = 1;
                timing.milliseconds = ms;
                timings.push_back(timing);
            }
            else
            {
                itr->callCount++;
                itr->milliseconds += ms;
            }
        }

        mTimings = std::move(timings);
        mFrameTime = toMilliseconds(frameBegin.timestamp, frameEnd.timestamp);
        mResolvedFrameIndex = frame.frameIndex;
        return FrameState::kReady;
    }

    //--------------------------------------------------------------------------
}
