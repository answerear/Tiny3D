

#include "Resource/T3DFont.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"

#define generic _generic    // keyword for C++/CX
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#undef generic


namespace Tiny3D
{
    FontPtr Font::create(const String &name)
    {
        FontPtr font = new Font(name);
        font->release();
        return font;
    }

    Font::Font(const String &name)
        : Resource(name)
    {

    }

    Font::~Font()
    {

    }

    Resource::Type Font::getType() const
    {
        return Resource::E_TYPE_FONT;
    }

    bool Font::load()
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

    void Font::unload()
    {
        Resource::unload();
    }

    ResourcePtr Font::clone() const
    {
        return Font::create(mName);
    }

    bool Font::loadFreeType(DataStream &stream)
    {
        FT_Library ftLibrary;
        if (FT_Init_FreeType(&ftLibrary))
        {
            T3D_LOG_ERROR("Could not init FreeType library !");
            return false;
        }

        FT_Face face;
        uint8_t *data;
        size_t size = stream.read(data);

        if (FT_New_Memory_Face(ftLibrary, data, size, 0, &face))
        {
            T3D_LOG_ERROR("Could not open font face !");
            return false;
        }

//         FT_F26Dot6 ftSize = (FT_F26Dot6)();

        FT_Done_FreeType(ftLibrary);

        return true;
    }
}
