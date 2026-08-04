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

#ifndef __T3D_PERFORMANCE_H__
#define __T3D_PERFORMANCE_H__


#include "T3DPrerequisites.h"


//-----------------------------------------------------------------------------
// 性能检测开关宏
// 注释掉下面这行即可在 release 下关闭所有性能检测功能，避免额外损耗
//-----------------------------------------------------------------------------
#define T3D_ENABLE_PERFORMANCE


namespace Tiny3D
{
    /**
     * \brief 静态性能采样工具，基于 DateTime::currentMicroSeconds() 记录代码段耗时
     */
    class T3D_ENGINE_API Performance
    {
    public:
        /**
         * \brief 开始采样，记录起始微秒时间戳
         * \param [in] name : 采样名称；若不存在则创建条目
         */
        static void beginSample(const char *name);

        /**
         * \brief 结束采样，累加本次耗时
         * \param [in] name : 与 beginSample 配对的采样名称
         * \note 仅当 name 已存在于 mSamples 时才更新统计；须先调用 beginSample
         */
        static void endSample(const char *name);

        /**
         * \brief 获取指定采样最近一次耗时
         * \param [in] name : 采样名称
         * \return 最近一次微秒数；无记录时返回 0
         */
        static int64_t getLastSampleMicroseconds(const char *name);

        /**
         * \brief 获取指定采样累计耗时
         * \param [in] name : 采样名称
         * \return 累计微秒数；无记录时返回 0
         */
        static int64_t getTotalSampleMicroseconds(const char *name);

        /**
         * \brief 获取指定采样调用次数
         * \param [in] name : 采样名称
         * \return 调用次数；无记录时返回 0
         */
        static uint32_t getSampleCount(const char *name);

        /**
         * \brief 通过 T3D_LOG_INFO 输出全部采样的 count、last、total、avg
         */
        static void logAllSamples();

        /**
         * \brief 清空全部采样数据
         */
        static void reset();

    private:
        /**
         * \brief 单个采样点的运行时统计
         */
        struct SampleData
        {
            int64_t     startTime {0};      ///< beginSample 记录的起始时间（微秒）
            int64_t     lastDuration {0};   ///< 最近一次耗时（微秒）
            int64_t     totalDuration {0};  ///< 累计耗时（微秒）
            uint32_t    count {0};          ///< 完成次数
        };

        /// 采样名称 → 统计数据
        static TMap<String, SampleData> mSamples;
    };
}


//-----------------------------------------------------------------------------
// 便捷宏：T3D_ENABLE_PERFORMANCE 打开时展开为实际调用，关闭时展开为空
//-----------------------------------------------------------------------------
#ifdef T3D_ENABLE_PERFORMANCE
    #define T3D_PERF_BEGIN(name)            Tiny3D::Performance::beginSample(name)
    #define T3D_PERF_END(name)              Tiny3D::Performance::endSample(name)
    #define T3D_PERF_LOG()                  Tiny3D::Performance::logAllSamples()
    #define T3D_PERF_RESET()                Tiny3D::Performance::reset()
    #define T3D_PERF_GET_LAST_US(name)      Tiny3D::Performance::getLastSampleMicroseconds(name)
    #define T3D_PERF_GET_TOTAL_US(name)     Tiny3D::Performance::getTotalSampleMicroseconds(name)
    #define T3D_PERF_GET_COUNT(name)        Tiny3D::Performance::getSampleCount(name)
#else
    #define T3D_PERF_BEGIN(name)            ((void)0)
    #define T3D_PERF_END(name)              ((void)0)
    #define T3D_PERF_LOG()                  ((void)0)
    #define T3D_PERF_RESET()                ((void)0)
    #define T3D_PERF_GET_LAST_US(name)      (0)
    #define T3D_PERF_GET_TOTAL_US(name)     (0)
    #define T3D_PERF_GET_COUNT(name)        (0)
#endif


#endif  /*__T3D_PERFORMANCE_H__*/
