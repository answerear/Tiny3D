/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_SG_NODE_H__
#define __T3D_SG_NODE_H__


#include "Misc/T3DNode.h"
#include "T3DMath.h"
#include "T3DVector3.h"
#include "T3DMatrix3.h"
#include "T3DMatrix4.h"
#include "Misc/T3DSmartPtr.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @class SGNode
     * @brief Scene Graph中的结点基类
     */
    class T3D_ENGINE_API SGNode : public Node
    {
        friend class SceneManager;

        /**
         * @brief 禁止直接使用拷贝构造函数拷贝，请使用clone接口代替复制
         */
        T3D_DISABLE_COPY(SGNode);

    protected:
        /**
         * @brief 默认构造函数
         * @param [in] unID : 标识结点的唯一ID，默认值是由引擎自动生成唯一ID
         * @note 这里构造函数是protected访问权限，主要是为了防止外部直接new来构造对象，
         *  本类是无法构造直接对象，请使用各派生类的create代替new来构造对象
         */
        SGNode(uint32_t unID = E_NID_AUTOMATIC);

    public:
        /**
         * @brief 析构函数
         */
        virtual ~SGNode();

        /**
         * @brief 设置用户数据
         * @param [in] data : 用户数据
         * @return void
         * @note 该数据可以是用户设置的任何整型甚至内存地址，具体由使用者自己定义和解析
         * @see long_t getUserData() const
         * @see void setUserData(const ObjectPtr &rkObject)
         * @see const ObjectPtr &getUserObject() const
         * @see ObjectPtr getUserObject()
         */
        void setUserData(long_t data);

        /**
         * @brief 获取用户数据
         * @return 返回用户数据
         * @note 返回的数据是由用户通过setUserData设置的数据，具体由使用者自己定义和解析
         * @see void setUserData(long_t data)
         */
        long_t getUserData() const;

        /**
         * @brief 设置用户数据对象
         * @param [in] rkObject : 对象指针
         * @return void
         * @note 该对象是Object类的任何派生类的指针，具体由使用者自己定义和解析
         * @see void setUserData(long_t data)
         * @see long_t getUserData() const
         * @see const ObjectPtr &getUserObject() const
         * @see ObjectPtr getUserObject()
         */
        void setUserObject(const ObjectPtr &rkObject);

        /**
         * @brief 获取用户数据对象
         * @return 获取用户数据对象
         * @note 返回的数据对象是由用户通过setUserObject设置的对象，具体由使用者自己定义和解析
         * @see void setUserData(long_t data)
         * @see long_t getUserData()
         * @see void setUserObject(const ObjectPtr &rkObject)
         * @see const ObjectPtr &getUserObject() const
         * @see ObjectPtr getUserObject()
         */
        const ObjectPtr &getUserObject() const;

        /**
        * @brief 获取用户数据对象
        * @return 获取用户数据对象
        * @note 返回的数据对象是由用户通过setUserObject设置的对象，具体由使用者自己定义和解析
        * @see void setUserData(long_t data)
        * @see long_t getUserData()
        * @see void setUserObject(const ObjectPtr &rkObject)
        * @see const ObjectPtr &getUserObject() const
        * @see const ObjectPtr &getUserObject() const
        */
        ObjectPtr getUserObject();

        /**
         * @brief 设置结点是否需要重绘、重新计算标记
         * @param [in] isDirty : 结点数据是否脏了标记
         * @param [in] recursive : 是否需要递归所有子节点都设置上该标记，默认为只设置本结点
         * @return void
         * @see bool isDirty() const
         */
        virtual void setDirty(bool isDirty, bool recursive = false);

        /**
         * @brief 返回结点数据是否脏，需要重新绘制、计算等标记
         * @return 返回结点数据脏标记
         * @see void setDirty(bool isDirty, bool recursive)
         */
        bool isDirty() const;

        /**
         * @brief 设置结点是否可见
         * @param [in] visible : 可见标记
         * @return void
         * @note 通过本接口设置了不可见，则会直接在渲染流水线上移除该结点以及所有子结点，不参与渲染
         * @see bool isVisible() const
         */
        virtual void setVisible(bool visible);

        /**
         * @brief 返回结点是否可见
         * @return 返回结点可见性
         * @see void setVisible(bool visible)
         */
        bool isVisible() const;

    protected:
        /** 
         * @brief 更新本身的变换和所有子结点的变换
         * @return void
         * @note 派生类重写本函数以实现具体的变换更新策略
         */
        virtual void updateTransform();

        /**
         * @brief 视景体外物体剔除，递归调用所有子结点
         * @param [in] bound : 视景体区域
         * @param [in] queue : 渲染队列
         * @return void
         * @note 
         *  - 如果本身不在视景体内，则所有子结点上的物体都会被剔除，不参与渲染；
         *  - 如果本身在视景体内，则会递归调用子结点判断；
         *  - 如果不是可渲染结点，则无法加入的RenderQueue中；
         */
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue);

        /**
         * @brief 克隆结点属性
         * @param [in] node : 目标结点
         * @return void
         * @note 复制本结点所有属性给目标结点node
         */
        virtual void cloneProperties(const NodePtr &node) const;

    private:
        long_t      mUserData;      /// 保存用户数据
        ObjectPtr   mUserObject;    /// 保存用户数据对象

        bool        mIsDirty;       /// 结点数据是否脏了，需要重绘、重新计算等
        bool        mIsVisible;     /// 结点可见性
    };
}


#include "T3DSGNode.inl"


#endif  /*__T3D_SG_NODE_H__*/
