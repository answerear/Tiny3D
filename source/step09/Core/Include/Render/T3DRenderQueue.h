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


#ifndef __T3D_RENDER_QUEUE_H__
#define __T3D_RENDER_QUEUE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 渲染组
     */
    class T3D_ENGINE_API RenderGroup : public Object
    {
    public:
        /**
         * @brief 构造函数
         */
        RenderGroup();

        /**
         * @brief 析构函数
         */
        virtual ~RenderGroup();
    };


    /**
     * @brief 渲染队列
     */
    class T3D_ENGINE_API RenderQueue : public Object
    {
    public:
        /** 
         * @brief 分组ID
         */
        enum GroupID
        {
            E_GRPID_NONE = 0,                   /**< 没有分组 */
            E_GRPID_BACKGROUND = 10,            /**< 背景分组 */
            E_GRPID_LIGHT = 15,                 /**< 灯光分组 */
            E_GRPID_SKY_BOX = 20,               /**< 天空盒分组 */
            E_GRPID_INDICATOR = 30,             /**< 坐标指示器分组 */
            E_GRPID_AUTOMATIC = 50,             /**< 自动分组 */
            E_GRPID_SOLID = 60,                 /**< 实体分组 */
            E_GRPID_WIREFRAME = 65,             /**< 线框分组 */
            E_GRPID_TRANSPARENT = 70,           /**< 半透明物体分组 */
            E_GRPID_TRANSPARENT_EFFECT = 80,    /**< 半透明特效分组 */
            E_GRPID_SHADOW = 90,                /**< 阴影分组 */
            E_GRPID_OVERLAY = 100               /**< UI分组 */
        };

        /**
         * @brief 创建渲染队列对象
         */
        static RenderQueuePtr create();

        /**
         * @brief 析构函数
         */
        virtual ~RenderQueue();

    protected:
        /**
         * @brief 构造函数
         */
        RenderQueue();

    protected:
        typedef std::map<GroupID, RenderGroupPtr>   RenderableGroup;
        typedef RenderableGroup::iterator           RenderableGroupItr;
        typedef RenderableGroup::const_iterator     RenderableGroupConstItr;

        typedef std::pair<GroupID, RenderGroupPtr>  RenderableGroupValue;

        RenderableGroup     mGroups;        /**< 渲染分组 */
    };
}


#endif  /*__T3D_RENDER_QUEUE_H__*/
