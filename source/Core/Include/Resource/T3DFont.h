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

#ifndef __T3D_FONT_H__
#define __T3D_FONT_H__


#include "T3DResource.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Font : public Resource
    {
    public:
        enum FontType
        {
            E_FT_TRUETYPE = 1,
            E_FT_IMAGE = 2,
        };

        class Char : public Object
        {
        public:
            Char()
                : mCode(0)
            {}

            ObjectPtr       mBlock;         /// �ַ���Ӧ������Ϣ����
            int32_t         mCode;          /// �ַ�����
            Rect            mArea;          /// �ַ���Ӧ������������Ϣ
        };

        T3D_DECLARE_SMART_PTR(Char);

        typedef std::vector<CharPtr>    CharSet;
        typedef CharSet::iterator       CharSetItr;
        typedef CharSet::const_iterator CharSetConstItr;
        typedef CharSet::value_type     CharSetValue;

        static FontPtr create(const String &name, int32_t fontSize, FontType fontType);

        virtual ~Font();

        virtual Type getType() const override;

        /**
         * @brief ���������С
         */
        int32_t getFontSize() const { return mFontSize; }

        /**
         * @brief �����������ͣ���TRUETYPE����IMAGEͼ��
         * @see enum FontType
         */
        FontType getFontType() const { return mFontType; }

        /**
         * @brief ��ȡ��ǰ��������ַ��Ŀ��
         */
        int32_t getFontWidth() const { return mFontWidth; }
        /**
         * @brief ��ȡ��ǰ��������ַ��ĸ߶�
         */
        int32_t getFontHeight() const { return mFontHeight; }

        /**
         * @brief ��ȡ��ǰ����Ļ��ߣ�����ֵ����������ֱ��������ַ��ϱߵ�ƫ��
         */
        int32_t getFontBaseline() const { return mFontBaseline; }

        /**
         * @brief ��ȡĳһ���ַ��Ŀ��
         * @note 
         *  - ����û�л�����ַ�������ӿڵ��û��������ϵ���ģ����ҵ�����󣬸��ַ�Ҳ���ᱻ����
         *  - �����Ѿ���������ַ������ֱ�Ӵӻ����з���
         */
        virtual int32_t getCharWidth(int32_t code) const = 0;

        /**
         * @brief ��ȡĳһ���ַ��ĸ߶�
         * @note ����û�л�����ַ�������ӿڵ��û��������ϵ���ģ����ҵ�����󣬸��ַ�Ҳ���ᱻ����
         */
        virtual int32_t getCharHeight(int32_t code) const = 0;

        /**
         * @brief ��ȡĳһ���ַ��Ŀ��
         * @note 
         *  - ����û�л�����ַ�������ӿڵ��û��������ϵ���ģ����ҵ�����󣬸��ַ�Ҳ���ᱻ����
         *  - �����Ѿ���������ַ������ֱ�Ӵӻ����з���
         *  - ����ӿ�����ڷֱ��������getFontWidth(int32_t)��getFontHeight(int32_t)���ܻ��һЩ����Ϊ���ֻ��Ҫ��Ⱦһ�ε�bitmap
         */
        virtual Size getCharSize(int32_t code) const = 0;

        /**
         * @brief �������ݣ���Ҫ�Ǹ�����������
         * @param [in] text : UTF8���ı��ַ���
         * @param [out] material : ���ذ����ı��Ĳ��ʶ���
         * @param [out] set : �����ַ�����Ϣ
         * @param [out] size : �ַ�����С
         * @return �ɹ�����������ʺ������򷵻�true�����򷵻�false.
         * @see class Char
         */
        virtual bool updateContent(const String &text, MaterialPtr &material, CharSet &set, Size &size) = 0;

    protected:
        Font(const String &name, int32_t fontSize, FontType fontType);

    protected:
        int32_t     mFontSize;      /// �ֺ�
        int32_t     mFontWidth;     /// ����������ַ����
        int32_t     mFontHeight;    /// ����������ַ��߶�
        int32_t     mFontBaseline;  /// �������
        FontType    mFontType;      /// �������ͣ�@see enum FontType
    };
}


#endif  /*__T3D_FONT_H__*/
