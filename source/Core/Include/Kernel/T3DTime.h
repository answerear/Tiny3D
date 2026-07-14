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

#ifndef __T3D_TIME_H__
#define __T3D_TIME_H__


#include "T3DPrerequisites.h"
#include "T3DSingleton.h"


namespace Tiny3D
{
    /**
     * \brief 引擎层全局时间单例（对齐 Unity 的 Time）
     * \remarks
     *   - 所有保存与返回的时间量一律使用 uint64_t 毫秒，不使用浮点数，
     *     与底层 DateTime::currentMSecsSinceEpoch() 天然对齐，累计不漂移。
     *   - timeScale 是比例系数（非时间量），用千分比整数表示：
     *     1000 = 1.0x、500 = 0.5x、0 = 暂停。
     *   - 每帧由 Agent::beginFrame() 调用一次 tick()，缓存本帧 deltaTime；
     *     用户在该帧任意位置读取 deltaTime() 都得到同一固定值。
     *   - start / tick / stepFixed 三个驱动接口为 private，配置写入
     *     setFixedDeltaTime / setMaximumDeltaTime 为 protected，
     *     仅将 Agent 声明为 friend，杜绝外部（含 Scene / 用户脚本）误调。
     */
    class T3D_ENGINE_API Time : public Singleton<Time>
    {
        /// 仅 Agent 可驱动 start / tick / stepFixed，并注入配置
        friend class Agent;

    public:
        Time() = default;
        ~Time() override = default;

        // —— 实例只读访问（单位：毫秒 ms；帧内固定值）——

        /// 本帧已应用 timeScale 的间隔（ms）
        uint64_t getDeltaTime() const { return mDeltaTime; }

        /// 本帧未缩放的真实间隔（ms）
        uint64_t getUnscaledDeltaTime() const { return mUnscaledDelta; }

        /// 累计已缩放时间（ms）
        uint64_t getTime() const { return mTime; }

        /// 累计真实时间（ms）
        uint64_t getUnscaledTime() const { return mUnscaledTime; }

        /// 固定步长（ms）
        uint64_t getFixedDeltaTime() const { return mFixedDeltaTime; }

        /// 已渲染帧数
        uint64_t getFrameCount() const { return mFrameCount; }

        /// 时间缩放，千分比，1000 = 1.0x
        uint32_t getTimeScale() const { return mTimeScale; }

        /// 运行期慢放 / 暂停（公开），千分比
        void setTimeScale(uint32_t permille) { mTimeScale = permille; }

        // —— Unity 风味的静态便捷包装（返回值单位：毫秒）——
        static uint64_t deltaTime() { return getInstance().getDeltaTime(); }
        static uint64_t unscaledDeltaTime() { return getInstance().getUnscaledDeltaTime(); }
        static uint64_t time() { return getInstance().getTime(); }
        static uint64_t unscaledTime() { return getInstance().getUnscaledTime(); }
        static uint64_t fixedDeltaTime() { return getInstance().getFixedDeltaTime(); }
        static uint64_t frameCount() { return getInstance().getFrameCount(); }
        static uint32_t timeScale() { return getInstance().getTimeScale(); }

    protected:
        // —— 配置写入：仅供 Agent（友元）在 init 注入；即便友元也走 setter，
        //    不直接写成员，保持不变量 / 后续校验的单一入口 ——
        void setFixedDeltaTime(uint64_t ms) { mFixedDeltaTime = (ms == 0 ? 1 : ms); }
        void setMaximumDeltaTime(uint64_t ms) { mMaxDeltaTime = ms; }

    private:
        // —— 驱动：仅 Agent（友元）可调用 ——

        /// init 时采样基准时刻，清零累计量
        void start();

        /// beginFrame 每帧推进一次，计算并缓存本帧 dt
        void tick();

        /// 消费一个固定步长，供 Agent 的 FixedUpdate 循环驱动
        bool stepFixed();

    private:
        uint64_t mStartMSec {0};        ///< 启动基准（currentMSecsSinceEpoch）
        uint64_t mLastMSec {0};         ///< 上一帧时刻 (ms)
        uint64_t mDeltaTime {0};        ///< 本帧已缩放间隔 (ms)
        uint64_t mUnscaledDelta {0};    ///< 本帧真实间隔 (ms)
        uint64_t mTime {0};             ///< 累计已缩放 (ms)
        uint64_t mUnscaledTime {0};     ///< 累计真实 (ms)
        uint64_t mFixedDeltaTime {20};  ///< 固定步长，默认 20ms(=50Hz)，由 Settings 注入
        uint64_t mMaxDeltaTime {333};   ///< dt clamp 上限，默认 333ms，由 Settings 注入
        uint32_t mTimeScale {1000};     ///< 千分比，1000 = 1.0x
        uint64_t mFrameCount {0};       ///< 帧计数
        uint64_t mFixedAccumulator {0}; ///< FixedUpdate 累加器 (ms)
    };

    #define T3D_TIME    Time::getInstance()

    T3D_EXTERN_SINGLETON(Time)
}


#endif  /*__T3D_TIME_H__*/

