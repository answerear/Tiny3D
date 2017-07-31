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

#ifndef __T3D_FONT_FREE_TYPE_H__
#define __T3D_FONT_FREE_TYPE_H__


#include "Resource/T3DFont.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "Misc/T3DSmartPtr.h"

#define generic _generic    // keyword for C++/CX
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#undef generic


namespace Tiny3D
{
    class FontFreeType : public Font
    {
    public:
        static FontFreeTypePtr create(const String &name, int32_t fontSize, FontType fontType);

        virtual ~FontFreeType();

        /**
         * @brief 获取某一个字符的宽度
         * @note 
         *  - 对于没有缓存的字符，这个接口调用会有性能上的损耗，而且调用完后，该字符也不会被缓存
         *  - 对于已经缓存过的字符，这个直接从缓存中返回
         */
        virtual int32_t getCharWidth(int32_t code) const override;

        /**
         * @brief 获取某一个字符的高度
         * @note 对于没有缓存的字符，这个接口调用会有性能上的损耗，而且调用完后，该字符也不会被缓存
         */
        virtual int32_t getCharHeight(int32_t code) const override;

        /**
         * @brief 获取某一个字符的宽高
         * @note 
         *  - 对于没有缓存的字符，这个接口调用会有性能上的损耗，而且调用完后，该字符也不会被缓存
         *  - 对于已经缓存过的字符，这个直接从缓存中返回
         *  - 这个接口相对于分别逐个调用getFontWidth(int32_t)和getFontHeight(int32_t)性能会高一些，因为这个只需要渲染一次到bitmap
         */
        virtual Size getCharSize(int32_t code) const override;

        /**
         * @brief 更新内容，主要是更新字体纹理
         * @param [in] text : UTF8的文本字符串
         * @param [out] material : 返回包含文本的材质对象
         * @param [out] set : 返回字符串信息
         * @return 成功更新字体材质和纹理，则返回true，否则返回false.
         * @see class Char
         */
        virtual bool updateContent(const String &text, MaterialPtr &material, CharSet &set) override;

        /**
         * @brief 通过freetype库加载bitmap
         * @param [in] code : 字符的UNICODE编码
         * @param [out] charSize : 返回字符的宽高
         * @return 加载位图成功返回ture，否则返回false
         */
        bool loadBitmap(int32_t code, Size &charSize);

        /**
         * @brief 把位图数据渲染到纹理指定区域
         * @param [in] texture : 纹理对象
         * @param [in] dstRect : 目标区域
         */
        bool renderAt(TexturePtr texture, const Rect &dstRect);

        FT_Face getFontFace() const { return mFTFace; }

    protected:
        FontFreeType(const String &name, int32_t fontSize, FontType fontType);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        bool loadFreeType(DataStream &stream);

    protected:
        FT_Library      mFTLibrary;
        FT_Face         mFTFace;
    };
}


#endif  /*__T3D_FONT_FREE_TYPE_H__*/
