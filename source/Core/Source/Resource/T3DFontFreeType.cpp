

#include "Resource/T3DFontFreeType.h"
#include "Resource/T3DFontManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DFreeTypeAtlas.h"


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
    {

    }

    FontFreeType::~FontFreeType()
    {

    }

    bool FontFreeType::load()
    {
        bool ret = false;

        ArchivePtr archive;
        MemoryDataStream stream;

        if (T3D_ARCHIVE_MGR.getArchive(mName, archive))
        {
            if (archive->read(mName, stream))
            {
                ret = loadFreeType(stream);
            }
        }

        return ret;
    }

    void FontFreeType::unload()
    {
        FT_Done_FreeType(mFTLibrary);

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

            if (FT_New_Memory_Face(mFTLibrary, data, size, 0, &face))
            {
                T3D_LOG_ERROR("Could not open font face !");
                break;
            }

            // 选择一个字符表，当前选择UNICODE
            FT_Select_Charmap(face, FT_ENCODING_UNICODE);

            // 设置字符大小，基于26.6规则设定
            FT_F26Dot6 ftSize = (FT_F26Dot6)(mFontSize * (1 << 6));
            int32_t dpi = 72;
            if (FT_Set_Char_Size(face, ftSize, 0, dpi, dpi))
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

    bool FontFreeType::updateContent(const String &text, MaterialPtr &material, CharSet &set)
    {
        return FreeTypeAtlas::getInstance().updateContent(this, text, material, set);
    }

    bool FontFreeType::loadBitmap(int32_t code, Size &charSize)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    bool FontFreeType::renderAt(TexturePtr texture, const Rect &dstRect)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }
}
