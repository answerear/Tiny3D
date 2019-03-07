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


#ifndef __T3D_SG_NODE_H__
#define __T3D_SG_NODE_H__


#include "Kernel/T3DNode.h"


namespace Tiny3D
{
    /**
     * @brief Scene Graph 中的结点基类
     */
    class T3D_ENGINE_API SGNode : public Node
    {
        T3D_DISABLE_COPY(SGNode);

    public:
        /**
         * @brief 析构函数
         */
        virtual ~SGNode();

        /**
         * @brief 设置用户数据
         * @param [in] data : 用户数据
         * @return void
         * @note 该数据可以是用户设置的任何整型甚至内存地址，
         *      具体由使用者自己定义和解析
         * @see void *getUserData() const
         * @see void setUserData(ObjectPtr object)
         * @see ObjectPtr getUserObject() const
         */
        void setUserData(void *data);

        /**
         * @brief 获取用户数据
         * @return 返回用户数据
         * @note 返回的数据是由用户通过setUserData设置的数据，
         *      具体由使用者自己定义和解析
         * @see void setUserData(uint64_t data)
         */
        void *getUserData() const;

        /**
         * @brief 设置用户数据对象
         * @param [in] object : 对象指针
         * @return void
         * @note 该对象是Object类的任何派生类的指针，具体由使用者自己定义和解析
         * @see void setUserData(uint64_t data)
         * @see uint64_t getUserData() const
         * @see ObjectPtr getUserObject() const
         */
        void setUserObject(ObjectPtr object);

        /**
         * @brief 获取用户数据对象
         * @return 获取用户数据对象
         * @note 返回的数据对象是由用户通过setUserObject设置的对象，
         *       具体由使用者自己定义和解析
         * @see void setUserData(uint64_t data)
         * @see uint64_t getUserData()
         * @see void setUserObject(ObjectPtr &object)
         * @see ObjectPtr getUserObject() const
        */
        ObjectPtr getUserObject() const;

        /**
         * @brief 设置结点是否可见
         * @param [in] visible : 可见标记
         * @return void
         * @remarks 通过本接口设置了不可见，则会直接在渲染流水线上移除该结点以及
         *      所有子结点，不参与渲染，但是update更新还是有效的。
         * @see bool isVisible() const
         */
        virtual void setVisible(bool visible);

        /**
         * @brief 获取结点可见性
         * @return 返回结点可见性
         * @see void setVisible(bool visible)
         */
        bool isVisible() const;

        /**
         * @brief 设置结点是否可用
         * @param [in] enabled : 可用标记
         * @remarks 不可用的结点，update是不会调用的，同时不可用的结点也看不见
         * @see bool isEnabled() const
         */
        virtual void setEnabled(bool enabled);

        /**
         * @brief 获取结点可用性
         * @return 返回结点可用性
         * @see void setEnabled(bool enabled)
         */
        bool isEnabled() const;

        /**
         * @brief 设置结点使用对应相机的掩码
         * @remarks 只有跟相机掩码一致的结点才能在对应相机中渲染。 相机掩码可以
         *      通过“或”操作同时设置多个。
         * @see uint32_t getCameraMask() const
         */
        void setCameraMask(uint32_t mask);

        /**
         * @brief 获取结点对应相机的掩码
         * @return 返回相机掩码
         * @see void setCameraMask(uint32_t mask)
         */
        uint32_t getCameraMask() const;

        /**
         * @brief 递归遍历
         * @return void
         * @remarks 递归遍历，遍历到的结点会调用 updateTransform() 接口
         */
        virtual void visit();

        /**
         * @brief 视景体外物体剔除，递归调用所有子结点
         * @param [in] bound : 视锥体碰撞体
         * @param [in] queue : 渲染队列
         * @return void
         * @note
         *  - 如果本身不在视景体内，则所有子结点上的物体都会被剔除，不参与渲染；
         *  - 如果本身在视景体内，则会递归调用子结点判断；
         *  - 如果不是可渲染结点，则无法加入的RenderQueue中；
         */
        virtual void frustumCulling(BoundPtr bound, RenderQueuePtr queue);

    protected:
        /**
         * @brief 默认构造函数
         * @param [in] uID : 标识结点的唯一ID，默认值是由引擎自动生成唯一ID
         * @note 这里构造函数是protected访问权限，主要是为了防止外部直接new来构
         *  造对象， 本类是无法构造直接对象，请使用各派生类的create代替new来构造
         *  对象
         */
        SGNode(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 更新本身的变换
         * @return void
         * @note 派生类重写本函数以实现具体的变换更新策略
         */
        virtual void updateTransform();

        /**
         * @brief 克隆结点属性
         * @param [in] node : 目标结点
         * @return void
         * @note 复制本结点所有属性给目标结点node
         */
        virtual TResult cloneProperties(NodePtr node) const override;

    private:
        void        *mUserData;     /**< 保存用户数据 */
        ObjectPtr   mUserObject;    /**< 保存用户数据对象 */

        uint32_t    mCameraMask;    /**< 相机掩码 */
        bool        mIsVisible;     /**< 结点可见性 */
        bool        mIsEnabled;     /**< 结点可用性 */
    };
}


#include "T3DSGNode.inl"


#endif  /*__T3D_SG_NODE_H__*/
