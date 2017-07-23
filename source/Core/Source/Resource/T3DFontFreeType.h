
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
        static FontFreeTypePtr create(const String &name, int32_t fontSize, FontType fontType, const FontConfig &fontConfig);

        virtual ~FontFreeType();

        virtual bool updateContent(const String &text, MaterialPtr &material, CharSet &set) override;

    protected:
        typedef std::map<int32_t, CharPtr>  CharMap;
        typedef CharMap::iterator           CharMapItr;
        typedef CharMap::const_iterator     CharMapConstItr;
        typedef CharMap::value_type         CharMapValue;

        struct Block : public Object
        {
            size_t  size;       /// 字体大小
            Rect    area;       /// block的区域
            CharMap charmap;    /// 字符集
        };

        T3D_DECLARE_SMART_PTR(Block);

        typedef std::map<size_t, BlockPtr>  BlockMap;
        typedef BlockMap::iterator          BlockMapItr;
        typedef BlockMap::const_iterator    BlockMapConstItr;
        typedef BlockMap::value_type        BlockMapValue;

        typedef std::map<String, BlockMap>  Blocks;
        typedef Blocks::iterator            BlocksItr;
        typedef Blocks::const_iterator      BlocksConstItr;
        typedef Blocks::value_type          BlocksValue;

        FontFreeType(const String &name, int32_t fontSize, FontType fontType, const FontConfig &fontConfig);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        bool loadFreeType(DataStream &stream);

        /**
         * @brief 查找charmap
         * @param [in] code : 字符编码
         * @param [out] block : 返回的区块对象
         * @param [out] ch : 返回的字符信息对象
         * @return 查找到返回true，否则返回false
         */
        bool lookupCharMap(int32_t code, BlockPtr &block, CharPtr &ch);

        /**
         * @brief 插入字符到charmap
         * @param [in] code : 字符编码
         * @param [in] block : 区块对象
         * @param [out] ch : 返回的字符信息对象
         * @return 插入成功返回true，否则返回false
         */
        bool insertCharMap(int32_t code, BlockPtr block, CharPtr &ch);

        /**
         * @brief 查找适合的区块
         * @param [out] block : 返回的区块对象
         * @return 查找到适合的返回true，否则返回false
         */
        bool lookupBlock(BlockPtr &block);

        /**
         * @brief 插入一个区块
         * @param [out] block : 返回的区块对象
         * @return 插入成功返回true，否则返回false
         */
        bool insertBlock(BlockPtr &block);

    protected:
        Blocks          mBlocks;
        FT_Library      mFTLibrary;
        FT_Face         mFTFace;
    };
}


#endif  /*__T3D_FONT_FREE_TYPE_H__*/
