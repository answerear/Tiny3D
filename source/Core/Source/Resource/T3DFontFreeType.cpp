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

#include "Resource/T3DFontFreeType.h"
#include "Resource/T3DFontManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DFreeTypeAtlas.h"
#include "Render/T3DHardwarePixelBuffer.h"


namespace Tiny3D
{
    FontFreeTypePtr FontFreeType::create(const String &name, int32_t fontSize, FontType fontType)
    {
        FontFreeTypePtr font = new FontFreeType(name, fontSize, fontType);
        font->release();
        return font;
    }

    FontFreeType::FontFreeType(const String &name, int32_t fontSize, FontType fontType)
        : Font(name, fontSize, fontType)
        , mFTLibrary(nullptr)
        , mFTFace(nullptr)
        , mData(nullptr)
    {

    }

    FontFreeType::~FontFreeType()
    {
        if (isLoaded())
        {
            unload();
        }
    }

    bool FontFreeType::load()
    {
        bool ret = false;

        do 
        {
            ArchivePtr archive;

            MemoryDataStream *stream = new MemoryDataStream();

            if (T3D_ARCHIVE_MGR.getArchive(mName, archive))
            {
                if (archive->read(mName, *stream))
                {
                    ret = loadFreeType(*stream);

                    if (ret)
                    {
                        T3D_SAFE_DELETE(mData);
                        mData = stream;
                    }
                }
            }

            ret = true;
        } while (0);

        return ret;
    }

    void FontFreeType::unload()
    {
        FT_Done_FreeType(mFTLibrary);
        T3D_SAFE_DELETE(mData);
        Font::unload();
    }

    ResourcePtr FontFreeType::clone() const
    {
        return FontFreeType::create(mName, mFontSize, mFontType);
    }

    bool FontFreeType::loadFreeType(DataStream &stream)
    {
        bool ret = false;

        do 
        {
            // 初始化freetype
            if (FT_Init_FreeType(&mFTLibrary))
            {
                T3D_LOG_ERROR("Could not init FreeType library !");
                break;
            }

            // 加载一个字体
            FT_Face face;
            uint8_t *data;
            size_t size = stream.read(data);
            FT_Error error;

            if (error = FT_New_Memory_Face(mFTLibrary, data, size, 0, &face))
            {
                T3D_LOG_ERROR("Could not open font face !");
                break;
            }

            // 选择一个字符表，当前选择UNICODE
            if (error = FT_Select_Charmap(face, FT_ENCODING_UNICODE))
            {
                T3D_LOG_ERROR("Select charmap failed !");
                break;
            }

            // 设置字符大小，基于26.6规则设定
            FT_F26Dot6 ftSize = (FT_F26Dot6)(mFontSize << 6);
            int32_t dpi = 72;
            if (error = FT_Set_Char_Size(face, ftSize, 0, dpi, dpi))
            {
                T3D_LOG_ERROR("Set font size failed !");
                break;
            }

            // 设置字体中最大字符宽度
            mFontWidth = (face->size->metrics.height >> 6);

            // 设置字体中最大字符高度
            mFontHeight = (face->size->metrics.max_advance >> 6);

            // 设置字体基线
            mFontBaseline = (face->size->metrics.ascender >> 6);

            // 设置字体外观
            mFTFace = face;

            ret = true;
        } while (0);

        return ret;
    }

    int32_t FontFreeType::getCharWidth(int32_t code) const
    {
        int32_t charWidth = 0;

        if (mFTFace != nullptr)
        {
            CharPtr ch;
            if (FreeTypeAtlas::getInstance().hasCharacter(code, ch))
            {
                charWidth = ch->mArea.width();
            }
            else
            {
                // 缓存中没有，这就麻烦了，只能通过freetype库去计算了
                FT_Error ftError = FT_Load_Char(mFTFace, code, FT_LOAD_RENDER);
                if (ftError == 0)
                {
                    charWidth = std::max(mFTFace->glyph->bitmap.width, (uint32_t)(mFTFace->glyph->metrics.horiAdvance >> 6));
                }
            }
        }

        return charWidth;
    }

    int32_t FontFreeType::getCharHeight(int32_t code) const
    {
        int32_t charHeight = 0;

        if (mFTFace != nullptr)
        {
            CharPtr ch;
            if (FreeTypeAtlas::getInstance().hasCharacter(code, ch))
            {
                charHeight = ch->mArea.height();
            }
            else
            {
                // 缓存中没有，这就麻烦了，只能通过freetype库去计算了
                FT_Error ftError = FT_Load_Char(mFTFace, code, FT_LOAD_RENDER);
                if (ftError == 0)
                {
                    charHeight = mFTFace->glyph->bitmap.rows;
                }
            }
        }

        return charHeight;
    }

    Size FontFreeType::getCharSize(int32_t code) const
    {
        int32_t charWidth = 0;
        int32_t charHeight = 0;

        if (mFTFace != nullptr)
        {
            CharPtr ch;
            if (FreeTypeAtlas::getInstance().hasCharacter(code, ch))
            {
                charWidth = ch->mArea.width();
                charHeight = ch->mArea.height();
            }
            else
            {
                FT_Error ftError = FT_Load_Char(mFTFace, code, FT_LOAD_RENDER);
                if (ftError == 0)
                {
                    charWidth = std::max(mFTFace->glyph->bitmap.width, (uint32_t)(mFTFace->glyph->metrics.horiAdvance >> 6));
                    charHeight = mFTFace->glyph->bitmap.rows;
                }
            }
        }

        return Size(charWidth, charHeight);
    }

    bool FontFreeType::updateContent(const String &text, MaterialPtr &material, CharSet &set, Size &size)
    {
        return FreeTypeAtlas::getInstance().updateContent(this, text, material, set, size);
    }

    bool FontFreeType::loadBitmap(int32_t code, Size &charSize)
    {
        bool ret = false;

        do 
        {
            FT_Error error = FT_Load_Char(mFTFace, code, FT_LOAD_RENDER);
            if (error != 0)
                break;

            int32_t stepX = (mFTFace->glyph->metrics.horiAdvance >> 6);
            int32_t charWidth = std::max(mFTFace->glyph->bitmap.width, (uint32_t)stepX);

            charSize.width = charWidth;
            charSize.height = mFontHeight;
            
            ret = true;
        } while (0);

        return ret;
    }

    bool FontFreeType::renderAt(TexturePtr texture, const Rect &dstRect)
    {
        bool ret = false;

        do 
        {
            uint8_t *srcData = mFTFace->glyph->bitmap.buffer;
            int32_t srcPitch = mFTFace->glyph->bitmap.pitch;
            int32_t srcWidth = mFTFace->glyph->bitmap.width;
            int32_t srcHeight = mFTFace->glyph->bitmap.rows;

            int32_t dstPitch = 0;
            uint8_t *dstData = (uint8_t *)texture->getPixelBuffer()->lock(dstRect, HardwareBuffer::E_HBL_WRITE_ONLY, dstPitch);            
            int32_t dstWidth = dstRect.width();
            int32_t dstHeight = dstRect.height();

            int32_t startX = (mFTFace->glyph->bitmap_left < 0 ? 0 : mFTFace->glyph->bitmap_left);
            int32_t endX = startX + srcWidth;

            int32_t startY = mFontBaseline - mFTFace->glyph->bitmap_top;
            int32_t endY = startY + srcHeight;

            const int32_t PIXEL_SIZE = 4;
            const int32_t PIXEL_SIZE_SHIFT = 2;

            // 这里因为freetype中获取到的bitmap的Y正方向是朝上的，跟纹理UV坐标刚好反的，所以逐行相反复制
            uint8_t *srcLine = srcData + srcPitch * (srcHeight - 1);
            uint8_t *dstLine = dstData + dstPitch * dstRect.top + PIXEL_SIZE * dstRect.left;

            for (int32_t y = 0; y < dstRect.height(); ++y)
            {
                for (int32_t x = 0; x < dstRect.width(); ++x)
                {
                    int32_t index = (x << PIXEL_SIZE_SHIFT);

                    if (x >= startX && x < endX
                        && y >= startY && y < endY)
                    {
                        // 在复制范围内，复制字符位图数据，把灰度图值存放在alpha通道中，
                        // 防止后面改变字体颜色后丢失了灰度值，需要重新解析freetype，进一步提高性能
                        dstLine[index] = srcLine[x];        // blue
                        dstLine[index + 1] = srcLine[x];    // green
                        dstLine[index + 2] = srcLine[x];    // red
                        dstLine[index + 3] = srcLine[x];    // alpha
                    }
                    else
                    {
                        // 不在范围内的，但是在字符范围中，其他像素全部置成透明值
                        dstLine[index] = dstLine[index + 1] = dstLine[index + 2] = dstLine[index + 3] = 0;
                    }
                }

                srcLine -= srcPitch;
                dstLine += dstPitch;
            }

            texture->getPixelBuffer()->unlock();

            ret = true;
        } while (0);

        return ret;
    }
}
