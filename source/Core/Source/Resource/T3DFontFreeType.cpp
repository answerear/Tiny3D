

#include "Resource/T3DFontFreeType.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"



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

            mFTFace = face;
            ret = true;
        } while (0);

        return ret;
    }

    void FontFreeType::updateContent(const String &text, MaterialPtr &material, size_t &u, size_t &v)
    {

    }
}
