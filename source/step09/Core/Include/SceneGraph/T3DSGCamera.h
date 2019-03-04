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
         * @brief 投影类型
         */
        enum ProjectionType
        {
            E_PT_ORTHOGRAPHIC = 0,      /**< 正交投影矩阵 */
            E_PT_PERSPECTIVE,           /**< 透视投影矩阵 */
        };

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
         * @brief 重写基类接口
         * @see Node::Type Node::getNodeType() const
         */
        virtual Type getNodeType() const override;

        /**
         * @brief 重写基类接口
         * @see NodePtr Node::clone() const
         */
        virtual NodePtr clone() const override;

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
         * @brief 设置投影类型
         * @param [in] type : 投影类型
         * @return void
         * @see enum ProjectionType
         */
        void setProjectionType(ProjectionType type);

        /**
         * @brief 获取投影类型
         * @return 返回投影类型
         * @see enum ProjectionType
         */
        ProjectionType getProjectionType() const;

        /**
         * @brief 设置投影变换需要的参数
         * @param [in] left : 视锥体左边界
         * @param [in] right : 视锥体右边界
         * @param [in] top : 视锥体上边界
         * @param [in] bottom : 视锥体下边界
         * @param [in] nearDist : 近平面距离
         * @param [in] farDist : 远平面距离
         */
        void setProjectionParams(Real left, Real right, Real top, Real bottom,
            Real nearDist, Real farDist);

        /**
         * @brief 获取宽高比
         * @return 返回宽高比
         */
        Real getAspectRatio() const;

        /**
         * @brief 获取纵向视角大小
         * @return 返回纵向视角大小
         */
        Radian getFovY() const;

        /**
         * @brief 获取近平面距离
         * @return 返回近平面距离
         */
        Real getNearPlaneDistance() const;

        /**
         * @brief 获取远平面距离
         * @return 返回远平面距离
         */
        Real getFarPlaneDistance() const;

        /**
         * @brief 获取相机的视锥体碰撞体
         * @return 返回相机关联的视锥体碰撞体 
         */
        BoundPtr getBound() const;

        /**
         * @brief 获取相机关联的视口对象
         */
        ViewportPtr getViewport() const;

        /**
         * @brief 获取观察空间变换矩阵
         * @return 返回观察空间变换矩阵
         */
        const Matrix4 &getViewMatrix() const;

        /**
         * @brief 获取投影变换矩阵
         * @return 返回投影变换矩阵
         */
        const Matrix4 &getProjectionMatrix() const;

        /**
         * @brief 重写基类接口
         * @see void SGNode::setDirty(bool isDirty, bool recursive = false)
         */
        virtual void setDirty(bool isDirty, bool recursive = false) override;

    protected:
        /**
         * @brief 构造函数
         */
        SGCamera(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 初始化对象
         */
        virtual TResult init();

        /**
         * @brief 实现基类接口
         */
        virtual TResult cloneProperties(NodePtr node) const override;

        /**
         * @brief 实现基类接口
         */
        virtual void updateTransform() override;

    protected:
        BoundPtr        mBound;         /**< 视锥体碰撞体 */
        ViewportPtr     mViewport;      /**< 关联本相机的视口对象 */

        ProjectionType  mProjType;      /**< 投影类型 */

        uint32_t        mObjectMask;    /**< 相机看到的物体掩码 */

        Real            mLeft;          /**< 视锥体左边界 */
        Real            mRight;         /**< 视锥体右边界 */
        Real            mTop;           /**< 视锥体上边界 */
        Real            mBottom;        /**< 视锥体下边界 */
        Real            mNear;          /**< 近平面距离 */
        Real            mFar;           /**< 远平面距离 */

        mutable Matrix4 mViewMatrix;    /**< 观察变换矩阵 */
        mutable Matrix4 mProjMatrix;    /**< 投影变换矩阵 */

        mutable bool    mIsViewDirty;   /**< 是否需要更新观察变换矩阵 */
        mutable bool    mIsFrustumDirty;/**< 是否需要更新投影变换 */
    };
}


#include "T3DSGCamera.inl"


#endif  /*__T3D_SG_CAMERA_H__*/
