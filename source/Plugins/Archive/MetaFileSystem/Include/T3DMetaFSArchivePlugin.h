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


#pragma once


#include "T3DMetaFSArchivePrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 文件档案结构插件
     */
    class MetaFSArchivePlugin : public Plugin
    {
    public:
        /**
         * @brief 默认构造函数
         */
        MetaFSArchivePlugin();

        /**
         * @brief 析构函数
         */
        virtual ~MetaFSArchivePlugin();

        /**
         * @brief 获取插件名称
         */
        const String &getName() const override;

        /**
         * @brief 安装插件
         */
        TResult install() override;

        /**
         * @brief 启动插件
         */
        TResult startup() override;

        /**
         * @brief 关闭插件
         */
        TResult shutdown() override;

        /**
         * @brief 卸载插件
         */
        TResult uninstall() override;

    protected:
        /// 插件名称
        String mName {};
    };

}

