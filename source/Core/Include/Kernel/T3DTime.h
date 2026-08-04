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
     * \brief 引擎全局时间单例（对齐 Unity 的 Time）
     * \remarks
     *   - 时间量一律用 uint64_t 毫秒，与 DateTime::currentMSecsSinceEpoch() 对齐，避免浮点累计漂移。
     *   - timeScale 用千分比整数：1000 = 1.0x、500 = 0.5x、0 = 暂停。
     *   - 每帧由 Agent::beginFrame() 调用一次 tick()，本帧 deltaTime 在帧内固定。
     *   - start / tick / stepFixed 为 private，setFixedDeltaTime / setMaximumDeltaTime 为 protected，
     *     仅友元 Agent 可驱动，避免 Scene / 脚本误调。
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

        /// 本帧未缩放的真实间隔（ms），经 mMaxDeltaTime 钳制
        uint64_t getUnscaledDeltaTime() const { return mUnscaledDelta; }

        /// 累计已缩放时间（ms）
        uint64_t getTime() const { return mTime; }

        /// 累计真实时间（ms）
        uint64_t getUnscaledTime() const { return mUnscaledTime; }

        /// 固定步长（ms），供 FixedUpdate 循环使用
        uint64_t getFixedDeltaTime() const { return mFixedDeltaTime; }

        /// 已调用 tick 的帧数
        uint64_t getFrameCount() const { return mFrameCount; }

        /// 时间缩放千分比，1000 = 1.0x
        uint32_t getTimeScale() const { return mTimeScale; }

        /**
         * \brief 运行期设置时间缩放（慢放 / 暂停）
         * \param [in] permille : 千分比，1000 = 1.0x，0 = 暂停
         */
        void setTimeScale(uint32_t permille) { mTimeScale = permille; }

        // —— Unity 风味的静态便捷包装（返回值单位：毫秒）——
        /// 等价于 getInstance().getDeltaTime()
        static uint64_t deltaTime() { return getInstance().getDeltaTime(); }
        /// 等价于 getInstance().getUnscaledDeltaTime()
        static uint64_t unscaledDeltaTime() { return getInstance().getUnscaledDeltaTime(); }
        /// 等价于 getInstance().getTime()
        static uint64_t time() { return getInstance().getTime(); }
        /// 等价于 getInstance().getUnscaledTime()
        static uint64_t unscaledTime() { return getInstance().getUnscaledTime(); }
        /// 等价于 getInstance().getFixedDeltaTime()
        static uint64_t fixedDeltaTime() { return getInstance().getFixedDeltaTime(); }
        /// 等价于 getInstance().getFrameCount()
        static uint64_t frameCount() { return getInstance().getFrameCount(); }
        /// 等价于 getInstance().getTimeScale()
        static uint32_t timeScale() { return getInstance().getTimeScale(); }

    protected:
        /**
         * \brief 设置固定步长（ms）；0 会被钳为 1
         * \param [in] ms : 固定步长毫秒数
         */
        void setFixedDeltaTime(uint64_t ms) { mFixedDeltaTime = (ms == 0 ? 1 : ms); }

        /**
         * \brief 设置单帧真实 dt 的钳制上限（ms）
         * \param [in] ms : 上限毫秒数，用于墙钟回退 / 跳变保护
         */
        void setMaximumDeltaTime(uint64_t ms) { mMaxDeltaTime = ms; }

    private:
        /**
         * \brief 采样启动基准时刻，清零累计时间与帧计数
         * \note 由 Agent::initTime() 在创建单例后调用一次
         */
        void start();

        /**
         * \brief 推进一帧：采样墙钟、钳制 raw dt、按 timeScale 缩放并累加 FixedUpdate 累加器
         * \note 由 Agent::beginFrame() 每帧调用一次；墙钟回退时 raw 钳为 0，超过 mMaxDeltaTime 时钳到上限
         */
        void tick();

        /**
         * \brief 若累加器足够则消费一个 fixedDeltaTime 并返回 true
         * \return 累加器 >= fixedDeltaTime 时扣减一步并返回 true，否则返回 false
         * \note 由 Agent 的 FixedUpdate 循环驱动；慢放 / 暂停通过缩放后的 mDeltaTime 间接生效
         */
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
