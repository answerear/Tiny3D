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

#ifndef __T3D_NODE_H__
#define __T3D_NODE_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @class Node
     * @brief 树形结构的结点基类
     */
    class T3D_ENGINE_API Node : public Object
    {
    public:
        /**
         * @brief 结点ID
         */
        enum ID
        {
            E_NID_AUTOMATIC = -1,       /// 自动生成ID
            E_NID_INVALID = 0,          /// 无效ID
        };

        /**
         * @brief 结点类型
         */
        enum Type
        {
            E_NT_UNKNOWN = 0,           /// 未知结点类型

            E_NT_TRANSFORM,             /// 3D变换结点
            E_NT_TRANSFORM2D,           /// 2D变换结点
            E_NT_BONE,                  /// 骨骼变换结点

            E_NT_CAMERA,                /// 相机结点
            E_NT_MODEL,                 /// 模型结点

            E_NT_LIGHT,                 /// 灯光结点
            E_NT_GEOMETRY,              /// 几何物体结点
            E_NT_MESH,                  /// 网格结点
            E_NT_SPHERE,                /// 球型物体结点
            E_NT_BOX,                   /// 立方体结点
            E_NT_SKELETON,              /// 可渲染的骨骼结点
            E_NT_AXIS,                  /// 坐标轴结点
            E_NT_QUAD,                  /// 四边形结点
            E_NT_SPRITE,                /// 精灵结点
            E_NT_TEXT2D,                /// 2D文本结点
        };

        /**
         * @brief 析构函数
         */
        virtual ~Node();

        /**
         * @brief 返回结点类型
         * @return 返回结点类型
         * @see enum Type
         * @note 每个派生类都要实现本接口，以返回具体的类型，如果不实现，则无法实例化对象了
         */
        virtual Type getNodeType() const = 0;

        /**
         * @brief 返回结点全局唯一标识ID
         * @return 返回结点ID
         * @see enum ID
         */
        uint32_t getNodeID() const;

        /**
         * @brief 设置结点名称
         * @param [in] name : 结点名称
         * @return void
         * @see const String &getName() const
         */
        void setName(const String &name);

        /**
         * @brief 获取结点名称
         * @return 返回结点名称
         * @see void setName(const String &name)
         */
        const String &getName() const;

        /**
         * @brief 添加一个子结点
         * @param [in] node : 子结点对象
         * @return void
         * @see void removeChild(const NodePtr &node, bool cleanup)
         * @see void removeChild(uint32_t nodeID, bool cleanup)
         */
        virtual void addChild(const NodePtr &node);

        /**
         * @brief 移除一个子结点
         * @param [in] node : 子结点对象
         * @param [in] cleanup : 是否删除子结点，true表示删除，false表示仅仅移除而并不删除子结点
         * @return void
         * @see void removeChild(uint32_t nodeID, bool cleanup)
         */
        virtual void removeChild(const NodePtr &node, bool cleanup);

        /**
         8 @brief 移除一个子结点
         * @param [in] nodeID : 结点唯一标识ID
         * @param [in] cleanup : 是否删除子结点，true表示删除，false表示仅仅移除而并不删除子结点
         * @return void
         * @see void removeChild(const NodePtr &node, bool cleanup)
         */
        virtual void removeChild(uint32_t nodeID, bool cleanup);

        /**
         * @brief 移除所有子结点
         * @param [in] cleanup : 是否删除所有子结点，true表示删除，false表示仅仅移除而并不删除子结点
         * @return void
         */
        virtual void removeAllChildren(bool cleanup);

        /**
         * @brief 从父结点移除自己
         * @param [in] cleanup : 是否删除自己，true表示删除，false表示仅仅移除而并不删除自己
         * @return void
         * @note 当cleanup用true调用后，本身结点已经被释放了，则不能在访问任何结点接口和数据
         */
        void removeFromParent(bool cleanup);

        /**
         * @brief 获取所有子结点
         * @return 返回所有子结点链表
         */
        const Children &getChildren() const;

        /**
         * @brief 返回指定结点ID的子结点
         * @param [in] unNodeID : 子结点ID
         * @return 返回子结点对象
         * @see NodePtr getChild(uint32_t unNodeID)
         */
        const NodePtr &getChild(uint32_t unNodeID) const;

        /**
         * @brief 返回指定结点ID的子结点
         * @param [in] unNodeID : 子结点ID
         * @return 返回子结点对象
         * @see NodePtr getChild(uint32_t unNodeID)
         */
        NodePtr getChild(uint32_t unNodeID);

        /**
         * @brief 返回指定名称的子结点
         * @param [in] name : 子结点名称
         * @return 返回子结点对象
         * @see NodePtr getChild(const String &name)
         */
        const NodePtr &getChild(const String &name) const;

        /**
         * @brief 返回指定名称的子结点
         * @param [in] name : 子结点名称
         * @return 返回子结点对象
         * @see const NodePtr &getChild(const String &name) const
         */
        NodePtr getChild(const String &name);

        /**
         * @brief 获取父结点
         * @return 返回父结点对象
         * @see NodePtr getParent()
         */
        const NodePtr &getParent() const;

        /**
         * @brief 获取父结点
         * @return 返回父结点对象
         * @see const NodePtr &getParent() const
         */
        NodePtr getParent();

        /**
         * @brief 复制一个新结点对象出来
         * @return 返回新结点对象
         * @note 派生类需要重写本接口以实现其具体的对象复制操作
         */
        virtual NodePtr clone() const = 0;

    protected:
        /**
         * @brief 默认构造函数
         * @param [in] uID : 结点唯一标识
         */
        Node(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief 复制结点的属性数据
         * @param [in] node : 新结点
         * @return void
         * @note 重写了clone方法的一般都需要重写本方法以实现具体类属性数据的复制
         */
        virtual void cloneProperties(const NodePtr &node) const;

        /**
         * @brief 结点挂到Scene Graph上时回调通知
         * @param [in] parent : 挂上去的父结点
         * @return void
         */
        virtual void onAttachParent(const NodePtr &parent);

        /**
         * @brief 结点从Scene Graph上拿下来时回调通知
         * @param [in] parent : 拿下来的父结点
         * @return void
         */
        virtual void onDetachParent(const NodePtr &parent);

    private:
        /**
         * @brief 生成全局唯一标识
         * @return 返回全局唯一标识
         */
        uint32_t makeGlobalID() const;

    private:
        uint32_t    mID;            /// 结点ID
        String      mName;          /// 给该结点命名

        NodePtr     mParent;        /// 父结点

    protected:
        Children    mChildren;      /// 子结点
    };
}


#include "T3DNode.inl"


#endif  /*__T3D_NODE_H__*/
