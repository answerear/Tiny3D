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

#ifndef __T3D_SG_TEXT2D_H__
#define __T3D_SG_TEXT2D_H__


#include "SceneGraph/T3DSGRenderable.h"
#include "Resource/T3DFont.h"


namespace Tiny3D
{
    /**
     * @class SGText2D
     * @brief ������ʾ2D�ı�����Ⱦ����
     */
    class T3D_ENGINE_API SGText2D : public SGRenderable
    {
    public:
        /**
         * @brief ����2D�ı���Ⱦ������
         * @param [in] text : �ı��ַ�����Ĭ�Ͽմ�
         * @param [in] fontSize : �����С��Ĭ��10��
         * @param [in] textColor : �ı���ɫ��Ĭ�ϰ�ɫ
         * @param [in] fontName : �����ֿ����ƣ�Ĭ����FontManager�������õ�
         * @param [in] uID : ���Ψһ��ʶ��Ĭ��Ϊ�����Զ�����
         * @return �����ɹ�����һ���¶��󣬴���ʧ���򷵻�nullptr
         * @see void FontManager::setDefaultFontName(const String &fontName)
         */
        static SGText2DPtr create(const String &text = "", size_t fontSize = 10, 
            const Color4 &textColor = Color4::WHITE, const String &fontName = "", 
            uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief ��������
         */
        virtual ~SGText2D();

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
         * @brief �����ı�
         * @param [in] text : �ı��ַ���
         * @return void
         * @see const String &getText() const
         */
        void setText(const String &text);

        /**
         * @brief ��ȡ�ı�
         * @return �����ı��ַ���
         * @see void setText(const String &text)
         */
        const String &getText() const { return mText; }

        /**
         * @brief �����ı���ɫ
         * @param [in] color : �ı���ɫ
         * @return void
         * @see const Color4 &getTextColor() const
         */
        void setTextColor(const Color4 &color);

        /**
         * @brief ��ȡ�ı���ɫ
         * @return �����ı���ɫ
         * @see void setTextColor(const Color4 &color)
         */
        const Color4 &getTextColor() const;

        /**
         * @brief ����ê��λ���ı���������λ��
         * @param [in] pos : ê��λ�ã���λ�ǰٷֱȣ���ֵ����0��1֮��
         * @return void
         * @see const Vector2 &getAnchorPos() const
         */
        void setAnchorPos(const Vector2 &pos);

        /**
         * @brief ��ȡê��λ��
         * @return ����ê��λ�ã����ص��ǰٷֱȣ����Ǿ���ֵ����ֵ����0��1֮��
         * @see void setAnchorPos(const Vector2 &pos)
         */
        const Vector2 &getAnchorPos() const;

    protected:
        /**
         * @brief Ĭ�Ϲ��캯��
         */
        SGText2D(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief ��ʼ��
         * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
         */
        bool init(const String &text, size_t fontSize, const Color4 &texColor, const String &fontName);

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

        /**
         * @brief �����ı����¶���ȸ����ַ���Ⱦ�����Ϣ
         * @return ���³ɹ�����true�����򷵻�false
         */
        bool updateVertices();

    protected:
        struct Vertex
        {
            Vector3     position;
            Vector2     texcoord;
            uint32_t    diffuse;
        };

        VertexDataPtr       mVertexData;    /// ��������
        IndexDataPtr        mIndexData;     /// ��������
        MaterialPtr         mMaterial;      /// ����
        FontPtr             mFont;          /// �ı���Ӧ������
        Vector2             mAnchorPos;     /// ê��λ��
        Size                mSize;          /// �ַ�����Ⱦ����Ĵ�С
        String              mText;          /// �ı��ַ���
        Font::CharSet       mCharSet;       /// �ַ�����Ӧ���ַ���
    };
}


#endif  /*__T3D_SG_SPRITE_H__*/
