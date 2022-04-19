/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_PLUGIN_H__
#define __T3D_PLUGIN_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 插件基类，所有插件均需从此类派生并实现其接口
     */
    class T3D_ENGINE_API Plugin
    {
    public:
        T3D_DECLARE_INTERFACE(Plugin);

        /**
         * @brief 获取插件名称
         */
        virtual const String &getName() const = 0;

        /**
         * @brief 安装插件
         */
        virtual TResult install() = 0;

        /**
         * @brief 启动插件
         */
        virtual TResult startup() = 0;

        /**
         * @brief 关闭插件
         */
        virtual TResult shutdown() = 0;

        /**
         * @brief 卸载插件
         */
        virtual TResult uninstall() = 0;
    };
}


#endif  /*__T3D_PLUGIN_H__*/
