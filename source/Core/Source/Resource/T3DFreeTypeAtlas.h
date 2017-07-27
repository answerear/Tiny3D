
#ifndef __T3D_FREE_TYPE_ATLAS_H__
#define __T3D_FREE_TYPE_ATLAS_H__


#include "Resource/T3DFontFreeType.h"


namespace Tiny3D
{
    class FreeTypeAtlas : public Singleton<FreeTypeAtlas>
    {
        friend class FontManager;

    public:
        virtual ~FreeTypeAtlas();

        bool hasCharacter(int32_t code, Font::CharPtr &ch) const;

        bool updateContent(FontFreeTypePtr font, const String &text, MaterialPtr &material, Font::CharSet &set);

    protected:
        // 存储所有字符对应的信息
        typedef std::map<int32_t, Font::CharPtr>    CharMap;
        typedef CharMap::iterator                   CharMapItr;
        typedef CharMap::const_iterator             CharMapConstItr;
        typedef CharMap::value_type                 CharMapValue;

        struct Block : public Object
        {
            Block()
                : size(0)
            {}

            ObjectPtr       face;       /// 区块所属的字体外观
            size_t          size;       /// 字体大小
            Rect            area;       /// block的区域
            Point           offset;     /// 字符起始位置
            CharMap         charmap;    /// 字符集
        };

        T3D_DECLARE_SMART_PTR(Block);

        // 存储所有相同字体外观和相同字体大小的block信息，一个元素是一个block
        typedef std::list<BlockPtr>             BlockList;
        typedef BlockList::iterator             BlockListItr;
        typedef BlockList::const_iterator       BlockListConstItr;
        typedef BlockList::value_type           BlockListValue;

        // 存储所有相同字体外观不同大小的block信息，相同字体大小会通过一个list串接起来
        typedef std::map<size_t, BlockList>     BlockMap;
        typedef BlockMap::iterator              BlockMapItr;
        typedef BlockMap::const_iterator        BlockMapConstItr;
        typedef BlockMap::value_type            BlockMapValue;

        struct Face : public Object
        {
            Face(const String &name, MaterialPtr material);
            ~Face();

            String          name;       /// 字体外观名称
            MaterialPtr     material;   /// 对应字体外观的材质
            Point           offset;     /// 新增block位置
            BlockMap        blockmap;   /// blockmap
            BlockList       free;       /// 空闲block链表
        };

        T3D_DECLARE_SMART_PTR(Face);

        // 相同的字体外观，用一个链表串起来，一个元素是一个字体外观对应的blockmap
        typedef std::list<FacePtr>              FaceList;
        typedef FaceList::iterator              FaceListItr;
        typedef FaceList::const_iterator        FaceListConstItr;
        typedef FaceList::value_type            FaceListValue;

        // 存储所有不同字体外观
        typedef std::map<String, FaceList>      FaceMap;
        typedef FaceMap::iterator               FaceMapItr;
        typedef FaceMap::const_iterator         FaceMapConstItr;
        typedef FaceMap::value_type             FaceMapValue;

        FreeTypeAtlas();

        /**
         * @brief 查找charmap
         * @param [in] code : 字符编码
         * @param [out] block : 返回的区块对象
         * @param [out] ch : 返回的字符信息对象
         * @return 查找到返回true，否则返回false
         */
        bool lookupCharMap(FontFreeTypePtr font, int32_t code, Font::CharPtr &ch);

        /**
         * @brief 插入字符到charmap
         * @param [in] code : 字符编码
         * @param [in] block : 区块对象
         * @param [out] ch : 返回的字符信息对象
         * @return 插入成功返回true，否则返回false
         */
        bool insertCharMap(FontFreeTypePtr font, int32_t code, Font::CharPtr &ch);

        /**
         * @brief 查找适合的区块
         * @param [out] block : 返回的区块对象
         * @return 查找到适合的返回true，否则返回false
         */
        bool lookupBlock(FontFreeTypePtr font, FacePtr &face, BlockPtr &block);

        /**
         * @brief 插入一个区块
         * @param [out] block : 返回的区块对象
         * @return 插入成功返回true，否则返回false
         */
        bool insertBlock(FontFreeTypePtr font, FacePtr face, BlockPtr &block);

        bool doStrategyAuto(FontFreeTypePtr font, FacePtr face, BlockPtr &block, Font::CharPtr &ch);

        bool doStrategyLRU(FontFreeTypePtr font, FacePtr face, BlockPtr &block, Font::CharPtr &ch);

        bool doStrategyAppend(FontFreeTypePtr font, FacePtr face, BlockPtr &block, Font::CharPtr &ch);

        bool loadMaterial(const String &name, FontFreeTypePtr font, MaterialPtr &material, size_t texWidth, size_t texHeight);

        bool createFace(MaterialPtr material, const String &fontName, FacePtr &face);

        bool createBlock(size_t fontSize, const Rect &area, const Point &offset, FacePtr face, BlockPtr &block);

        bool createTexture(const String &name, size_t texWidth, size_t texHeight, TexturePtr &texture);

        bool copyBitmapToTexture(FontFreeTypePtr font, BlockPtr block, Font::CharPtr ch);

        /**
         * @brief 扩展纹理，其实就是重新生成一个新纹理，然后把老纹理数据复制上去
         */
        bool extendTexture(const String &name, TexturePtr srcTexture, TexturePtr &newTexture);

    protected:
        FaceMap     mFaces;
        CharMap     mCharmap;           /// 存储所有字符，方便用于查找
    };
}


#endif  /*__T3D_FREE_TYPE_ATLAS_H__*/
