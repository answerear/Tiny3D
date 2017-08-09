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
     * @brief ���νṹ�Ľ�����
     */
    class T3D_ENGINE_API Node : public Object
    {
    public:
        /**
         * @brief ���ID
         */
        enum ID
        {
            E_NID_AUTOMATIC = -1,       /// �Զ�����ID
            E_NID_INVALID = 0,          /// ��ЧID
        };

        /**
         * @brief �������
         */
        enum Type
        {
            E_NT_UNKNOWN = 0,           /// δ֪�������

            E_NT_TRANSFORM,             /// 3D�任���
            E_NT_TRANSFORM2D,           /// 2D�任���
            E_NT_BONE,                  /// �����任���

            E_NT_CAMERA,                /// ������
            E_NT_MODEL,                 /// ģ�ͽ��

            E_NT_LIGHT,                 /// �ƹ���
            E_NT_GEOMETRY,              /// ����������
            E_NT_MESH,                  /// ������
            E_NT_SPHERE,                /// ����������
            E_NT_BOX,                   /// ��������
            E_NT_SKELETON,              /// ����Ⱦ�Ĺ������
            E_NT_AXIS,                  /// ��������
            E_NT_QUAD,                  /// �ı��ν��
            E_NT_SPRITE,                /// ������
            E_NT_TEXT2D,                /// 2D�ı����
        };

        /**
         * @brief ��������
         */
        virtual ~Node();

        /**
         * @brief ���ؽ������
         * @return ���ؽ������
         * @see enum Type
         * @note ÿ�������඼Ҫʵ�ֱ��ӿڣ��Է��ؾ�������ͣ������ʵ�֣����޷�ʵ����������
         */
        virtual Type getNodeType() const = 0;

        /**
         * @brief ���ؽ��ȫ��Ψһ��ʶID
         * @return ���ؽ��ID
         * @see enum ID
         */
        uint32_t getNodeID() const;

        /**
         * @brief ���ý������
         * @param [in] name : �������
         * @return void
         * @see const String &getName() const
         */
        void setName(const String &name);

        /**
         * @brief ��ȡ�������
         * @return ���ؽ������
         * @see void setName(const String &name)
         */
        const String &getName() const;

        /**
         * @brief ���һ���ӽ��
         * @param [in] node : �ӽ�����
         * @return void
         * @see void removeChild(const NodePtr &node, bool cleanup)
         * @see void removeChild(uint32_t nodeID, bool cleanup)
         */
        virtual void addChild(const NodePtr &node);

        /**
         * @brief �Ƴ�һ���ӽ��
         * @param [in] node : �ӽ�����
         * @param [in] cleanup : �Ƿ�ɾ���ӽ�㣬true��ʾɾ����false��ʾ�����Ƴ�������ɾ���ӽ��
         * @return void
         * @see void removeChild(uint32_t nodeID, bool cleanup)
         */
        virtual void removeChild(const NodePtr &node, bool cleanup);

        /**
         8 @brief �Ƴ�һ���ӽ��
         * @param [in] nodeID : ���Ψһ��ʶID
         * @param [in] cleanup : �Ƿ�ɾ���ӽ�㣬true��ʾɾ����false��ʾ�����Ƴ�������ɾ���ӽ��
         * @return void
         * @see void removeChild(const NodePtr &node, bool cleanup)
         */
        virtual void removeChild(uint32_t nodeID, bool cleanup);

        /**
         * @brief �Ƴ������ӽ��
         * @param [in] cleanup : �Ƿ�ɾ�������ӽ�㣬true��ʾɾ����false��ʾ�����Ƴ�������ɾ���ӽ��
         * @return void
         */
        virtual void removeAllChildren(bool cleanup);

        /**
         * @brief �Ӹ�����Ƴ��Լ�
         * @param [in] cleanup : �Ƿ�ɾ���Լ���true��ʾɾ����false��ʾ�����Ƴ�������ɾ���Լ�
         * @return void
         * @note ��cleanup��true���ú󣬱������Ѿ����ͷ��ˣ������ڷ����κν��ӿں�����
         */
        void removeFromParent(bool cleanup);

        /**
         * @brief ��ȡ�����ӽ��
         * @return ���������ӽ������
         */
        const Children &getChildren() const;

        /**
         * @brief ����ָ�����ID���ӽ��
         * @param [in] unNodeID : �ӽ��ID
         * @return �����ӽ�����
         * @see NodePtr getChild(uint32_t unNodeID)
         */
        const NodePtr &getChild(uint32_t unNodeID) const;

        /**
         * @brief ����ָ�����ID���ӽ��
         * @param [in] unNodeID : �ӽ��ID
         * @return �����ӽ�����
         * @see NodePtr getChild(uint32_t unNodeID)
         */
        NodePtr getChild(uint32_t unNodeID);

        /**
         * @brief ����ָ�����Ƶ��ӽ��
         * @param [in] name : �ӽ������
         * @return �����ӽ�����
         * @see NodePtr getChild(const String &name)
         */
        const NodePtr &getChild(const String &name) const;

        /**
         * @brief ����ָ�����Ƶ��ӽ��
         * @param [in] name : �ӽ������
         * @return �����ӽ�����
         * @see const NodePtr &getChild(const String &name) const
         */
        NodePtr getChild(const String &name);

        /**
         * @brief ��ȡ�����
         * @return ���ظ�������
         * @see NodePtr getParent()
         */
        const NodePtr &getParent() const;

        /**
         * @brief ��ȡ�����
         * @return ���ظ�������
         * @see const NodePtr &getParent() const
         */
        NodePtr getParent();

        /**
         * @brief ����һ���½��������
         * @return �����½�����
         * @note ��������Ҫ��д���ӿ���ʵ�������Ķ����Ʋ���
         */
        virtual NodePtr clone() const = 0;

    protected:
        /**
         * @brief Ĭ�Ϲ��캯��
         * @param [in] uID : ���Ψһ��ʶ
         */
        Node(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief ���ƽ�����������
         * @param [in] node : �½��
         * @return void
         * @note ��д��clone������һ�㶼��Ҫ��д��������ʵ�־������������ݵĸ���
         */
        virtual void cloneProperties(const NodePtr &node) const;

        /**
         * @brief ���ҵ�Scene Graph��ʱ�ص�֪ͨ
         * @param [in] parent : ����ȥ�ĸ����
         * @return void
         */
        virtual void onAttachParent(const NodePtr &parent);

        /**
         * @brief ����Scene Graph��������ʱ�ص�֪ͨ
         * @param [in] parent : �������ĸ����
         * @return void
         */
        virtual void onDetachParent(const NodePtr &parent);

    private:
        /**
         * @brief ����ȫ��Ψһ��ʶ
         * @return ����ȫ��Ψһ��ʶ
         */
        uint32_t makeGlobalID() const;

    private:
        uint32_t    mID;            /// ���ID
        String      mName;          /// ���ý������

        NodePtr     mParent;        /// �����

    protected:
        Children    mChildren;      /// �ӽ��
    };
}


#include "T3DNode.inl"


#endif  /*__T3D_NODE_H__*/
