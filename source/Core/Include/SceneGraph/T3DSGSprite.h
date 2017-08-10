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

#ifndef __T3D_SG_SPRITE_H__
#define __T3D_SG_SPRITE_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGSprite : public SGRenderable
    {
    public:
        /**
         * @brief ����sprite������
         * @param [in] uID : ���Ψһ��ʶ��Ĭ��Ϊ�����Զ�����
         */
        static SGSpritePtr create(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief ��������
         */
        virtual ~SGSprite();

        /**
         * @brief �Ӹ���̳У���д���ض�Ӧ�Ľ������
         * @return ���ؽ������
         * @see enum Type
         */
        virtual Type getNodeType() const override;

        /**
         * @brief �Ӹ���̳У���д�����¡��������ʵ�ֱ������ĸ��Ʋ���
         * @return ����һ���µ�3D�任������
         */
        virtual NodePtr clone() const override;

        /**
         * @brief ��������UV����
         * @param [in] index : ��������
         * @param [in] uv : ��������
         * @return void
         */
        void setTextureUV(size_t index, const Vector2 &uv);

        /**
         * @brief ���������������
         * @param [in] rect : ��������
         * @return void
         * @note ����ʹ�ñ��ӿ�ֱ����������UV
         */
        void setTextureUV(const Rect &rect);

        /**
         * @brief ����ĳ��������ɫ��Ĭ�϶��ǰ�ɫ
         * @param [in] index : ��������
         * @param [in] color : ������ɫ
         * @return void
         */
        void setVertexColor(size_t index, const Color4 &color);

        /**
         * @brief �������ж�����ɫ��Ĭ�϶��ǰ�ɫ
         * @param [in] color : ������ɫ
         * @return void
         */
        void setVerticesColor(const Color4 &color);

        /**
         * @brief ����ê��λ�ھ�����������λ��
         * @param [in] pos : ê��λ�ã���λ�ǰٷֱȣ���ֵ����0��1֮��
         * @return void
         */
        void setAnchorPos(const Vector2 &pos);

        /**
         * @brief ��ȡê��λ��
         * @return ����ê��λ�ã����ص��ǰٷֱȣ����Ǿ���ֵ����ֵ����0��1֮��
         */
        const Vector2 &getAnchorPos() const;

        /**
         * @brief ���þ����С
         * @param [in] size : ��Сֵ
         * @return void
         * @note �����С����ֱ�����Ż����Ĵ�С������ʵ����Ļ�ϵ����ش�С
         */
        void setSize(const Size &size);

        /**
         * @brief ��ȡ�����С
         * @return ���ؾ����С
         * @note �����С����ֱ�����Ż����Ĵ�С������ʵ����Ļ�ϵ����ش�С
         */
        const Size &getSize() const;

    protected:
        /**
         * @brief Ĭ�Ϲ��캯��
         */
        SGSprite(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief ��ʼ��
         * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
         */
        bool init();

        /**
         * @brief �Ӹ���̳У���д��ʵ�������Ӿ�������޳�
         * @param [in] bound : �Ӿ�������
         * @param [in] queue : ��Ⱦ����
         * @return void
         */
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        /**
         * @brief �Ӹ���̳У����ز��ʶ������Ⱦ��
         */
        virtual MaterialPtr getMaterial() const override;

        /**
         * @brief �Ӹ���̳У�������ȾͼԪ���͸���Ⱦ��
         */
        virtual Renderer::PrimitiveType getPrimitiveType() const override;

        /**
         * @brief �Ӹ���̳У����ض������ݸ���Ⱦ��
         */
        virtual VertexDataPtr getVertexData() const override;

        /**
         * @brief �Ӹ���̳У����ض����������ݸ���Ⱦ��
         */
        virtual IndexDataPtr getIndexData() const override;

        /**
         * @brief �Ӹ���̳У������Ƿ�ʹ�ö�����������
         */
        virtual bool isIndicesUsed() const override;

    protected:
        /**
         * @brief ����ʹ�õĶ���ṹ����
         */
        struct Vertex
        {
            Vector3     position;       /// ����λ��
            Vector2     texcoord;       /// ����UV����
            Color4      color;          /// ������ɫ
        };

        VertexDataPtr       mVertexData;    /// ��������
        MaterialPtr         mMaterial;      /// ��������
        Vector2             mAnchorPos;     /// ê��λ��
        Size                mSize;
    };
}


#endif  /*__T3D_SG_SPRITE_H__*/
