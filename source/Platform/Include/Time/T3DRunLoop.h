/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __T3D_RUN_LOOP_H__
#define __T3D_RUN_LOOP_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"
#include <map>


namespace Tiny3D
{
#define T3D_INVALID_LOOP_ID     0

    class RunLoopObserver;

    /**
     * @class T3DRunLoop
     * @brief 运行循环对象
     * @note 该类是系统循环，按照系统循环计时
     */
    class T3D_PLATFORM_API RunLoop
    {
        T3D_DISABLE_COPY(RunLoop);

    public:
        /**
         * @brief Constructor for T3DRunLoop.
         */
        RunLoop();

        /**
         * @brief Destructor for T3DRunLoop.
         */
        ~RunLoop();

        /**
         * @brief 启动系统循环计时器.
         * @param [in] unInterval : 定时毫秒数
         * @param [in] bRepeat : 是否循环计时器
         * @return 循环ID
         */
        uint32_t start(uint32_t unInterval, bool bRepeat, RunLoopObserver *pObserver);

        /**
         * @brief 简单的启动系统循环，按照循环次数来回调
         * @param : void
         * @return 循环ID
         */
        uint32_t start(RunLoopObserver *pObserver);

        /**
         * @brief 关闭循环计时器.
         * @param [in] unLoopID : 循环ID，调用start接口返回
         * @return 对应的循环计时器ID有效就返回true，否则返回false
         */
        bool stop(uint32_t unLoopID);

        /**
         * @brief 执行计时器，并且派发回调
         */
        void execute();

    protected:
        struct RunLoopInfo
        {
            uint64_t            ullLastTimestamp;   /// 上次计时时间戳
            uint64_t            ullInterval;        /// 设定的时间间隔
            RunLoopObserver     *pObserver;         /// 回调对象
            bool                bRepeat;            /// 是否循环计时
            bool                bAlive;             /// 是否还有效
        };

        typedef std::map<uint32_t, RunLoopInfo> RunLoopInfos;
        typedef RunLoopInfos::iterator          RunLoopInfosItr;

        typedef std::pair<uint32_t, RunLoopInfo> RunLoopValue;

        RunLoopInfos    m_infos;
        static uint32_t m_unLoopID;
    };
}


#endif  /*__T3D_RUN_LOOP_H__*/