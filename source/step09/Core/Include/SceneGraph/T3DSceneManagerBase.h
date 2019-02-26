﻿/*******************************************************************************
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


#ifndef __T3D_SCENE_MANAGER_BASE_H__
#define __T3D_SCENE_MANAGER_BASE_H__


#include "SceneGraph/T3DSGNode.h"


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
         * @brief 更新并渲染场景
         * @param [in] camera : 渲染场景使用的相机
         * @return 调用成功返回 T3D_OK
         * @remarks 场景插件实现本接口，实现自己的更新和渲染逻辑
         */
        virtual TResult renderScene(SGCameraPtr camera) = 0;

        /**
         * @brief 获取场景根结点
         * @return 返回场景根结点对象
         * @remarks 场景插件实现本接口，实现自己的更新和渲染逻辑
         */
        virtual SGNodePtr getRoot() const = 0;

        /**
         * @brief 创建一个3D变换结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个3D变换结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGTransform3DPtr createTransform3D(SGNodePtr parent, 
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个相机结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个相机结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGCameraPtr createCamera(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个光源结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个光源结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGLightPtr createLight(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个模型结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个模型结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGModelPtr createModel(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个可渲染网格结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个可渲染网格结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGMeshPtr createMesh(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个可渲染坐标轴结点 
         * @param [in] X : X轴长度
         * @param [in] Y : Y轴长度
         * @param [in] Z : Z轴长度
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个可渲染坐标轴结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGAxisPtr createAxis(Real X, Real Y, Real Z, SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个可渲染的四边形结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个可渲染的四边形结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGQuadPtr createQuad(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个可渲染的公告板结点
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个可渲染的公告板结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGBillboardPtr createBillboard(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个可渲染立方体结点
         * @param [in] center : 立方体中心
         * @param [in] extent : 立方体三个轴方向上的长度
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个可渲染立方体结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGBoxPtr createBox(const Vector3 &center, const Vector3 &extent,
            SGNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) = 0;

        /**
         * @brief 创建一个可渲染球体结点
         * @param [in] center : 球心
         * @param [in] radius : 球半径
         * @param [in] parent : 父结点
         * @param [in] uID : 结点ID，默认自动生成ID
         * @return 返回一个可渲染球体结点
         * @remarks 场景插件实现本接口，可以在创建同时构建自己的场景管理关系
         *      并进行场景管理。
         */
        virtual SGSpherePtr createSphere(const Vector3 &center, Real radius,
            SGNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) = 0;

    protected:
        /**
         * @brief 构造函数
         */
        SceneManagerBase();
    };
}


#endif  /*__T3D_SCENE_MANAGER_BASE_H__*/
