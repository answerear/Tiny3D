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
     * @brief Scene Graph�еĽ�����
     */
    class T3D_ENGINE_API SGNode : public Node
    {
        friend class SceneManager;

        /**
         * @brief ��ֱֹ��ʹ�ÿ������캯����������ʹ��clone�ӿڴ��渴��
         */
        T3D_DISABLE_COPY(SGNode);

    protected:
        /**
         * @brief Ĭ�Ϲ��캯��
         * @param [in] unID : ��ʶ����ΨһID��Ĭ��ֵ���������Զ�����ΨһID
         * @note ���ﹹ�캯����protected����Ȩ�ޣ���Ҫ��Ϊ�˷�ֹ�ⲿֱ��new���������
         *  �������޷�����ֱ�Ӷ�����ʹ�ø��������create����new���������
         */
        SGNode(uint32_t unID = E_NID_AUTOMATIC);

    public:
        /**
         * @brief ��������
         */
        virtual ~SGNode();

        /**
         * @brief �����û�����
         * @param [in] data : �û�����
         * @return void
         * @note �����ݿ������û����õ��κ����������ڴ��ַ��������ʹ�����Լ�����ͽ���
         * @see long_t getUserData() const
         * @see void setUserData(const ObjectPtr &rkObject)
         * @see const ObjectPtr &getUserObject() const
         * @see ObjectPtr getUserObject()
         */
        void setUserData(long_t data);

        /**
         * @brief ��ȡ�û�����
         * @return �����û�����
         * @note ���ص����������û�ͨ��setUserData���õ����ݣ�������ʹ�����Լ�����ͽ���
         * @see void setUserData(long_t data)
         */
        long_t getUserData() const;

        /**
         * @brief �����û����ݶ���
         * @param [in] rkObject : ����ָ��
         * @return void
         * @note �ö�����Object����κ��������ָ�룬������ʹ�����Լ�����ͽ���
         * @see void setUserData(long_t data)
         * @see long_t getUserData() const
         * @see const ObjectPtr &getUserObject() const
         * @see ObjectPtr getUserObject()
         */
        void setUserObject(const ObjectPtr &rkObject);

        /**
         * @brief ��ȡ�û����ݶ���
         * @return ��ȡ�û����ݶ���
         * @note ���ص����ݶ��������û�ͨ��setUserObject���õĶ��󣬾�����ʹ�����Լ�����ͽ���
         * @see void setUserData(long_t data)
         * @see long_t getUserData()
         * @see void setUserObject(const ObjectPtr &rkObject)
         * @see const ObjectPtr &getUserObject() const
         * @see ObjectPtr getUserObject()
         */
        const ObjectPtr &getUserObject() const;

        /**
        * @brief ��ȡ�û����ݶ���
        * @return ��ȡ�û����ݶ���
        * @note ���ص����ݶ��������û�ͨ��setUserObject���õĶ��󣬾�����ʹ�����Լ�����ͽ���
        * @see void setUserData(long_t data)
        * @see long_t getUserData()
        * @see void setUserObject(const ObjectPtr &rkObject)
        * @see const ObjectPtr &getUserObject() const
        * @see const ObjectPtr &getUserObject() const
        */
        ObjectPtr getUserObject();

        /**
         * @brief ���ý���Ƿ���Ҫ�ػ桢���¼�����
         * @param [in] isDirty : ��������Ƿ����˱��
         * @param [in] recursive : �Ƿ���Ҫ�ݹ������ӽڵ㶼�����ϸñ�ǣ�Ĭ��Ϊֻ���ñ����
         * @return void
         * @see bool isDirty() const
         */
        virtual void setDirty(bool isDirty, bool recursive = false);

        /**
         * @brief ���ؽ�������Ƿ��࣬��Ҫ���»��ơ�����ȱ��
         * @return ���ؽ����������
         * @see void setDirty(bool isDirty, bool recursive)
         */
        bool isDirty() const;

        /**
         * @brief ���ý���Ƿ�ɼ�
         * @param [in] visible : �ɼ����
         * @return void
         * @note ͨ�����ӿ������˲��ɼ������ֱ������Ⱦ��ˮ�����Ƴ��ý���Լ������ӽ�㣬��������Ⱦ
         * @see bool isVisible() const
         */
        virtual void setVisible(bool visible);

        /**
         * @brief ���ؽ���Ƿ�ɼ�
         * @return ���ؽ��ɼ���
         * @see void setVisible(bool visible)
         */
        bool isVisible() const;

    protected:
        /** 
         * @brief ���±���ı任�������ӽ��ı任
         * @return void
         * @note ��������д��������ʵ�־���ı任���²���
         */
        virtual void updateTransform();

        /**
         * @brief �Ӿ����������޳����ݹ���������ӽ��
         * @param [in] bound : �Ӿ�������
         * @param [in] queue : ��Ⱦ����
         * @return void
         * @note 
         *  - ����������Ӿ����ڣ��������ӽ���ϵ����嶼�ᱻ�޳�����������Ⱦ��
         *  - ����������Ӿ����ڣ����ݹ�����ӽ���жϣ�
         *  - ������ǿ���Ⱦ��㣬���޷������RenderQueue�У�
         */
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue);

        /**
         * @brief ��¡�������
         * @param [in] node : Ŀ����
         * @return void
         * @note ���Ʊ�����������Ը�Ŀ����node
         */
        virtual void cloneProperties(const NodePtr &node) const;

    private:
        long_t      mUserData;      /// �����û�����
        ObjectPtr   mUserObject;    /// �����û����ݶ���

        bool        mIsDirty;       /// ��������Ƿ����ˣ���Ҫ�ػ桢���¼����
        bool        mIsVisible;     /// ���ɼ���
    };
}


#include "T3DSGNode.inl"


#endif  /*__T3D_SG_NODE_H__*/
