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

#ifndef __T3D_FACTORY_INTERFACE_H__
#define __T3D_FACTORY_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class IWindow;

    enum EPlatform
    {
        E_PLATFORM_UNKNOWN = 0,
        E_PLATFORM_WIN32,
        E_PLATFORM_MACOSX,
        E_PLATFORM_LINUX,
        E_PLATFORM_IOS,
        E_PLATFORM_ANDROID,
    };

    /**
     * @class IAdapterFactory
     * @brief 操作系统适配层工厂接口.
     * @note 不同操作系统平台需要实现继承该类实现具体平台接口
     */
    class FactoryInterface
    {
        T3D_DECLARE_INTERFACE(FactoryInterface);

    public:
        /**
         * @brief 创建操作系统平台相关的窗口.
         * @param [in] isGLWindow : 是否OpenGL窗口
         * @return 返回平台相关的窗口操作对象，需要用户调用delete释放资源
         */
        virtual IWindow *createPlatformWindow(bool isGLWindow) = 0;

        /**
         * @brief 获取当前平台类型.
         * @return 返回当前平台类型
         */
        virtual EPlatform getPlatform() = 0;
    };

    /**
     * @brief 创建操作系统相关的适配层工厂对象.
     * @note 不同操作系统平台需要实现该接口以返回操作系统适配层工厂对象
     * @return 返回适配层工厂对象，需要用户调用delete释放资源
     */
    FactoryInterface *createPlatformFactory();
}


#endif  /*__T3D_FACTORY_INTERFACE_H__*/
