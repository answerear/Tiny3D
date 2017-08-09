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

#ifndef __T3D_SG_TRANSFORM_2D_H__
#define __T3D_SG_TRANSFORM_2D_H__


#include "SceneGraph/T3DSGNode.h"
#include "T3DMath.h"
#include "T3DVector2.h"
#include "T3DTransform.h"


namespace Tiny3D
{
    /**
     * @brief �� Scene Graph ��2D����ʹ�õĿռ�任���
     */
    class T3D_ENGINE_API SGTransform2D : public SGNode
    {
    public:
        /**
         * @brief ��������
         * @param [in] unID : ���Ψһ��ʶ��Ĭ�����Զ�����
         * @return ����һ��2D�任������
         */
        static SGTransform2DPtr create(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief ��������
         */
        virtual ~SGTransform2D();

        /**
         * @brief �Ӹ���̳�����д�ӿڣ���ȡ�������
         * @return ���ؽ������
         */
        virtual Type getNodeType() const override;

        /**
         * @brief �����ڸ��ڵ�ռ�����ϵ��λ��
         * @param [in] rkPos : �ڸ��ڵ�ռ�����ϵ�µ�λ��
         */
        void setPosition(const Vector2 &rkPos);

        /**
         * @brief �����ڸ��ڵ�ռ�����ϵ��λ��
         * @param [in] x : ���ڵ�ռ�����ϵ�µ�x����
         * @param [in] y : ���ڵ�ռ�����ϵ�µ�y����
         */
        void setPosition(Real x, Real y);

        /**
         * @brief �����ڸ��ڵ�ռ�����ϵ�µ�λ��
         * @return ���ص�λ��
         */
        const Vector2 &getPosition() const;

        /**
         * @brief �����ڸ����ռ�����ϵ�µĳ���
         * @param [in] rkQ : ������Ԫ��
         * @return void
         * @note ��Ȼ��2D�ռ�ı任��Ȼ����ת����ʵ�ֱַ���X��Y��Z���������ת��
         *  ����������������Ԫ�ر�ʾ��ת
         * @see void setOrientation(Real w, Real x, Real y, Real z)
         */
        void setOrientation(const Quaternion &rkQ);

        /**
         * @brief �����ڸ����ռ�����ϵ�µĳ���
         * @param [in] w : ��Ԫ�ص�w����
         * @param [in] x : ��Ԫ�ص�x����
         * @param [in] y : ��Ԫ�ص�y����
         * @param [in] z : ��Ԫ�ص�z����
         * @return void
         * @see void setOrientation(const Quaternion &rkQ)
         */
        void setOrientation(Real w, Real x, Real y, Real z);

        /**
         * @brief �����ڸ����ռ�����ϵ�µĳ���
         * @param [in] radian : ��ת����
         * @param [in] axis : �����ռ��µ���ת��
         * @return void
         * @see void setOrientation(const Quaternion &rkQ)
         */
        void setOrientation(const Radian &radian, const Vector3 &axis);

        /**
         * @brief ��ȡ�ڸ����ռ�����ϵ�µĳ���
         * @return ����һ����ʾ�������Ԫ�ض���
         */
        const Quaternion &getOrientation() const;

        /**
         * @brief �����ڸ����ռ�����ϵ�µ�����
         * @param [in] rkScale : ����ֵ
         * @return void
         * @see void setScale(Real x, Real y)
         */
        void setScale(const Vector2 &rkScale);

        /**
         * @brief �����ڸ����ռ�����ϵ�µ�����
         * @param [in] x : X������ֵ
         * @param [in] y : Y������ֵ
         * @return void
         * @see void setScale(const Vector2 &rkScale)
         */
        void setScale(Real x, Real y);

        /**
         * @brief ��ȡ�ڸ����ռ�����ϵ�µ�����ֵ
         * @return ����һ����ʾ����������ֵ��������ÿ��������ʾһ���������ֵ
         */
        const Vector2 &getScale() const;

        /**
         * @brief ƽ��һ��ƫ��ֵ
         * @param [in] rkOffset : �����������ϵ�ƫ��ֵ
         * @return void
         */
        void translate(const Vector2 &rkOffset);

        /**
         * @brief ƽ��һ��ƫ��ֵ
         * @param [in] x : X����ƫ��ֵ
         * @param [in] y : Y����ƫ��ֵ
         * @return void
         */
        void translate(Real x, Real y);

        /**
         * @brief ��һ��������ָ�����ƶ�ĳһ������
         * @param [in] rkOrientation : ��������
         * @param [in] step : �ƶ�����
         * @return void
         */
        void translate(const Vector2 &rkOrientation, Real step);

        /**
         * @brief �ڵ�ǰ��������תһ����Ԫ�ر�ʾ����ת�任
         * @param [in] rkQ : ��ʾ��ת�任����Ԫ��
         * @return void
         */
        void rotate(const Quaternion &rkQ);

        /**
         * @brief �ڵ�ǰ��������תһ��������ת����Ƹ������ת�Ǳ�ʾ����ת�任
         * @param [in] axis : ��ת��
         * @param [in] radians : ��ת�ǣ���λ������
         * @return void
         * @see void rotate(const Vector3 &axis, const Degree &degrees)
         */
        void rotate(const Vector3 &axis, const Radian &radians);

        /**
         * @brief �ڵ�ǰ��������תһ��������ת����Ƹ������ת�Ǳ�ʾ����ת�任
         * @param [in] axis : ��ת��
         * @param [in] degrees : ��ת�ǣ���λ���Ƕ�
         * @return void
         * @see void rotate(const Vector3 &axis, const Radian &radians)
         */
        void rotate(const Vector3 &axis, const Degree &degrees);

        /**
         * @brief �ڵ�ǰ��������X����ת����б��
         * @param [in] degrees : ��б�ǣ���λ���Ƕ�
         * @return void
         * @see void pitch(const Radian &radians)
         */
        void pitch(const Degree &degrees);

        /**
         * @brief �ڵ�ǰ��������Y����ת�������
         * @param [in] degrees : ����ǣ���λ���Ƕ�
         * @return void
         * @see void yaw(const Radian &radians)
         */
        void yaw(const Degree &degrees);

        /**
         * @brief �ڵ�ǰ��������Z����ת��������
         * @param [in] degrees : �����ǣ���λ���Ƕ�
         * @return void
         * @see void roll(const Radian &radians)
         */
        void roll(const Degree &degrees);

        /**
         * @brief �ڵ�ǰ��������X����ת����б��
         * @param [in] degrees : ��б�ǣ���λ������
         * @return void
         * @see void pitch(const Degree &degrees)
         */
        void pitch(const Radian &radians);

        /**
         * @brief �ڵ�ǰ��������Y����ת�������
         * @param [in] degrees : ����ǣ���λ������
         * @return void
         * @see void yaw(const Degree &degrees)
         */
        void yaw(const Radian &radians);

        /**
         * @brief �ڵ�ǰ��������Z����ת��������
         * @param [in] degrees : �����ǣ���λ������
         * @return void
         * @see void roll(const Degree &degrees)
         */
        void roll(const Radian &radians);

        /**
         * @brief �ڵ�ǰ��С������
         * @param [in] rkScale : ����ֵ
         * @return void
         * @see void scale(Real x, Real y)
         */
        void scale(const Vector2 &rkScale);

        /**
         * @brief �ڵ�ǰ��С������
         * @param [in] x : X������ֵ
         * @param [in] y : Y������ֵ
         * @param [in] z : Z������ֵ
         * @return void
         * @see void scale(const Vector2 &rkScale)
         */
        void scale(Real x, Real y);

        /**
         * @brief ��ȡ�ֲ�������ı任
         * @return ����һ���ֲ�������ı任����
         */
        virtual const Transform &getLocalToWorldTransform();

        /**
         * @brief ��ȡ�ֲ��任
         * @return ����һ���ֲ��任����
         */
        Transform getLocalTransform() const;

        /**
         * @brief �Ӹ���̳У���д�����¡��������ʵ�ֱ������ĸ��Ʋ���
         * @return ����һ���µ�3D�任������
         */
        virtual NodePtr clone() const override;
        
    protected:
        /**
         * @brief �Ӹ���̳У���д����Ӧ���ҵ�������֪ͨ�¼�
         * @param [in] parent : ��������
         * @return void
         */
        virtual void onAttachParent(const NodePtr &parent) override;

        /**
         * @brief �Ӹ���̳У���д����Ӧ���Ӹ�������ߵ�֪ͨ�¼�
         * @param [in] parent : ��������
         * @return void
         */
        virtual void onDetachParent(const NodePtr &parent) override;

        /**
         * @brief �Ӹ���̳У���дʵ�ָ��±任����
         * @return void
         */
        virtual void updateTransform() override;

        /**
         * @brief �Ӹ���̳У���дʵ��3D�任�����������ݵĸ���
         * @param [in] node : �µĽ�����
         * @return void
         */
        virtual void cloneProperties(const NodePtr &node) const override;

    protected:
        /**
         * @brief Ĭ�Ϲ��캯��
         * @param [in] unID : ���Ψһ��ʶ��Ĭ�����Զ�����
         */
        SGTransform2D(uint32_t uID = E_NID_AUTOMATIC);

        Vector2     mPosition;          /// ���ڵ�����ϵ�µľֲ�λ��
        Quaternion  mOrientation;       /// ���ڵ�����ϵ�µľֲ�����
        Vector2     mScale;             /// ���ڵ�����ϵ�µľֲ���С

        Transform   mWorldTransform;    /// �Ӿֲ�������ı任����
    };
}


#include "T3DSGTransform2D.inl"


#endif  /*__T3D_SG_TRANSFORM_2D_H__*/
