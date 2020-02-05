﻿/*******************************************************************************
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


#ifndef __T3D_SCENE_MANAGER_BASE_H__
#define __T3D_SCENE_MANAGER_BASE_H__


#include "Scene/T3DSceneNode.h"
#include "Component/T3DQuad.h"


namespace Tiny3D
{
    /**
     * @brief 场景管理基类
     * @remarks 可以根据不同场景管理算法实现各自的场景插件
     */
    class T3D_ENGINE_API SceneManagerBase : public Object
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~SceneManagerBase();

        /**
         * @brief 更新场景树
         * @return 调用成功返回 T3D_OK
         * @remarks 场景插件实现本接口，实现自己的更新逻辑
         */
        virtual TResult update() = 0;

        /**
         * @brief 渲染场景
         * @param [in] viewport : 渲染的视口对象
         * @return 调用成功返回 T3D_OK
         * @remarks 场景插件实现本接口，实现自己的渲染逻辑
         */
        virtual TResult render(ViewportPtr viewport) = 0;

        /**
         * @brief 获取场景根结点
         * @return 返回场景根结点对象
         * @remarks 场景插件实现本接口，实现自己的更新和渲染逻辑
         */
        virtual SceneNodePtr getRoot() const = 0;

        virtual SceneNodePtr createSceneNode(SceneNodePtr parent, 
            bool autoAddTransform = true, ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @fn  TResult addRenderable(RenderablePtr renderable);
         * @brief   添加可渲染对象到对应相机队列，用于视锥体剔除
         * @param [in]  renderable  : 可渲染对象.
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult addRenderable(Renderable *renderable) = 0;

        /**
         * @fn  TResult removeRenderable(RenderablePtr renderable);
         * @brief   根据camera mask来移除可渲染对象
         * @param [in]  renderable  : 可渲染对象.
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult removeRenderable(Renderable *renderable) = 0;

    protected:
        /**
         * @brief 构造函数
         */
        SceneManagerBase();
    };
}


#endif  /*__T3D_SCENE_MANAGER_BASE_H__*/
