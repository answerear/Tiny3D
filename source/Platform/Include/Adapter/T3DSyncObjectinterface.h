/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#ifndef __T3D_SYNC_OBJECT_INTERFACE_H__
#define __T3D_SYNC_OBJECT_INTERFACE_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"


namespace Tiny3D
{
    /**
     * \brief 临界区
     */
    class ICriticalSection
    {
        T3D_DECLARE_INTERFACE(ICriticalSection);

    public:
        virtual TResult lock() = 0;
     
        /**
         * \brief 进入临界区
         * \param [in] timeout : 超时时间，-1 表示没有超时 
         * \return 调用失败会返回对应的错误码，调用成功后会等到获得进入临界区权限才返回 T3D_OK
         */
        virtual TResult tryLock(uint32_t timeout) = 0;

        /**
         * \brief 离开临界区
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unlock() = 0;
    };

    /**
     * \brief 互斥量
     */
    class IMutex
    {
        T3D_DECLARE_INTERFACE(IMutex);

    public:
        virtual TResult lock() = 0;
     
        /**
         * \brief 上锁
         * \param [in] timeout : 超时时间，-1 表示没有超时 
         * \return 调用失败会返回对应的错误码，调用成功后会等到获得锁才返回 T3D_OK
         */
        virtual TResult tryLock(uint32_t timeout) = 0;

        /**
         * \brief 解锁
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unlock() = 0;
    };

    class IRecursiveMutex
    {
        T3D_DECLARE_INTERFACE(IRecursiveMutex);

    public:
        virtual TResult lock() = 0;
        
        /**
         * \brief 上锁
         * \param [in] timeout : 超时时间，-1 表示没有超时 
         * \return 调用失败会返回对应的错误码，调用成功后会等到获得锁才返回 T3D_OK
         */
        virtual TResult tryLock(uint32_t timeout) = 0;

        /**
         * \brief 解锁
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unlock() = 0;
    };

    /**
     * \brief 信号量
     */
    class ISemaphore
    {
        T3D_DECLARE_INTERFACE(ISemaphore);

    public:
        virtual TResult lock() = 0;
     
        /**
         * \brief 上锁
         * \param [in] timeout : 超时时间，-1 表示没有超时 
         * \return 调用失败会返回对应的错误码，调用成功后会等到获得锁才返回 T3D_OK
         */
        virtual TResult tryLock(uint32_t timeout) = 0;

        /**
         * \brief 解锁
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult unlock() = 0;
    };

    /**
     * \brief 事件
     */
    class IEvent
    {
        T3D_DECLARE_INTERFACE(IEvent);

    public:
        /**
         * \brief 等待事件触发
         * \param [in] cs : 事件临界区
         * \param [in] timeout : 超时时间，-1 表示没有超时 
         * \return 调用失败会返回对应的错误码，调用成功后等待 trigger() 调用才返回 T3D_OK
         */
        virtual TResult wait(uint32_t timeout) = 0;

        /**
         * \brief 触发事件
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult trigger() = 0;

        /**
         * \brief 重置事件
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult reset() = 0;
    };

    /**
     * \brief 条件变量
     */
    class IWaitCondition
    {
        T3D_DECLARE_INTERFACE(IWaitCondition);

    public:
        /**
         * \brief 等待条件变量触发
         * \param [in] cs : 临界区对象 
         * \param [in] timeout :  超时时间，-1 表示没有超时 
         * \return 调用失败会返回对应的错误码，调用成功后等待 wakeOne() 或 wakeAll() 调用才返回 T3D_OK
         */
        virtual TResult wait(uint32_t timeout) = 0;

        /**
         * \brief 唤醒一个等待条件变量
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult wakeOne() = 0;

        /**
         * \brief 唤醒所有等待条件变量
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult wakeAll() = 0;
    };
}

#endif  /*__T3D_SYNC_OBJECT_INTERFACE_H__*/
