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

            ObjectPtr       mBlock;         /// 字符对应区块信息对象
            int32_t         mCode;          /// 字符编码
            Rect            mArea;          /// 字符对应材质中纹理信息
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
         * @brief 返回字体大小
         */
        int32_t getFontSize() const { return mFontSize; }

        /**
         * @brief 返回字体类型，是TRUETYPE还是IMAGE图像
         * @see enum FontType
         */
        FontType getFontType() const { return mFontType; }

        /**
         * @brief 获取当前字体最大字符的宽度
         */
        int32_t getFontWidth() const { return mFontWidth; }
        /**
         * @brief 获取当前字体最大字符的高度
         */
        int32_t getFontHeight() const { return mFontHeight; }

        /**
         * @brief 获取当前字体的基线，返回值是向下是正直，相对于字符上边的偏移
         */
        int32_t getFontBaseline() const { return mFontBaseline; }

        /**
         * @brief 获取某一个字符的宽度
         * @note 
         *  - 对于没有缓存的字符，这个接口调用会有性能上的损耗，而且调用完后，该字符也不会被缓存
         *  - 对于已经缓存过的字符，这个直接从缓存中返回
         */
        virtual int32_t getCharWidth(int32_t code) const = 0;

        /**
         * @brief 获取某一个字符的高度
         * @note 对于没有缓存的字符，这个接口调用会有性能上的损耗，而且调用完后，该字符也不会被缓存
         */
        virtual int32_t getCharHeight(int32_t code) const = 0;

        /**
         * @brief 获取某一个字符的宽高
         * @note 
         *  - 对于没有缓存的字符，这个接口调用会有性能上的损耗，而且调用完后，该字符也不会被缓存
         *  - 对于已经缓存过的字符，这个直接从缓存中返回
         *  - 这个接口相对于分别逐个调用getFontWidth(int32_t)和getFontHeight(int32_t)性能会高一些，因为这个只需要渲染一次到bitmap
         */
        virtual Size getCharSize(int32_t code) const = 0;

        /**
         * @brief 更新内容，主要是更新字体纹理
         * @param [in] text : UTF8的文本字符串
         * @param [out] material : 返回包含文本的材质对象
         * @param [out] set : 返回字符串信息
         * @param [out] size : 字符串大小
         * @return 成功更新字体材质和纹理，则返回true，否则返回false.
         * @see class Char
         */
        virtual bool updateContent(const String &text, MaterialPtr &material, CharSet &set, Size &size) = 0;

    protected:
        Font(const String &name, int32_t fontSize, FontType fontType);

    protected:
        int32_t     mFontSize;      /// 字号
        int32_t     mFontWidth;     /// 字体中最大字符宽度
        int32_t     mFontHeight;    /// 字体中最大字符高度
        int32_t     mFontBaseline;  /// 字体基线
        FontType    mFontType;      /// 字体类型，@see enum FontType
    };
}


#endif  /*__T3D_FONT_H__*/
