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


#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SceneGraph/T3DSGTransform3D.h"
#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * @brief 场景相机结点类
     */
    class T3D_ENGINE_API SGCamera : public SGTransform3D
    {
    public:
        /**
         * @brief 创建相机结点对象
         * @param [in] uID : 结点ID，默认自动生成
         * @return 返回一个相机结点对象
         */
        static SGCameraPtr create(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGCamera();

        /**
         * @brief 渲染当前相机看到的场景物体
         * @remarks 这个接口不能随便调用，是在Viewport里面调用的
         */
        virtual void renderScene(ViewportPtr viewport);

        /**
         * @brief 专门提供给挂相机结点的3D变换结点使用，用于构建UVN相机.
         * @param [in] pos : 相机位置
         * @param [in] obj : 相机观察物体位置
         * @param [in] up : 上方向向量
         * @return void
         * @note UVN相机通过此接口可以构造一个变换矩阵出来
         */
        void lookAt(const Vector3 &pos, const Vector3 &obj, const Vector3 &up);

        /**
         * @brief 设置相机能看见的场景结点掩码
         * @remarks 掩码可以通过“或”操作，设置多个掩码，只要场景结点中的相机掩码
         *      CameraMask 设置的跟本相机中其中一个掩码一致的，均能渲染到本相机
         *      对应的视口中。
         * @see uint32_t getObjectMask() const
         */
        void setObjectMask(uint32_t mask);

        /**
         * @brief 获取相机能看见的场景结点掩码
         * @see void setObjectMask(uint32_t mask)
         */
        uint32_t getObjectMask() const;

        /**
         * @brief 获取相机的视锥体碰撞体
         * @return 返回相机关联的视锥体碰撞体 
         */
        BoundPtr getBound() const;

        /**
         * @brief 获取相机关联的视口对象
         */
        ViewportPtr getViewport() const;

    protected:
        SGCamera(ID uID = E_NID_AUTOMATIC);

    protected:
        BoundPtr        mBound;
        ViewportPtr     mViewport;

        uint32_t        mObjectMask;
    };
}


#include "T3DSGCamera.inl"


#endif  /*__T3D_SG_CAMERA_H__*/
