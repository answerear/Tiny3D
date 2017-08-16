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

        bool updateContent(FontFreeTypePtr font, const String &text, MaterialPtr &material, Font::CharSet &set, Size &size);

    protected:
        // 存储所有字符对应的信息
        typedef std::map<int32_t, Font::CharPtr>    CharMap;
        typedef CharMap::iterator                   CharMapItr;
        typedef CharMap::const_iterator             CharMapConstItr;
        typedef CharMap::value_type                 CharMapValue;

        struct Block : public Object
        {
            Block()
                : face(nullptr)
                , size(0)
            {}

            Object          *face;      /// 区块所属的字体外观，为了避免循环引用到时无法释放，这里直接用指针，不用智能指针
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

        typedef std::map<int32_t, BlockPtr>     Blocks;
        typedef Blocks::iterator                BlocksItr;
        typedef Blocks::const_iterator          BlocksConstItr;
        typedef Blocks::value_type              BlocksValue;

        struct Face;
        T3D_DECLARE_SMART_PTR(Face);

        struct Face : public Object
        {
            static FacePtr create(const String &name, size_t index);

            Face(const String &name);
            ~Face();

            bool init(size_t index);

            /**
             * @brief 是否还有空闲block可使用
             */
            bool isBlockAvailable() const;

            /**
             * @brief 纹理是否还有空间可用
             */
            bool isTextureAvailable() const;

            /**
             * @brief 创建纹理
             */
            bool createTexture(size_t texWidth, size_t texHeight, TexturePtr &texture);

            /**
             * @brief 创建block
             */
            bool createBlock(size_t fontSize, BlockPtr &block);

            /**
             * @brief 按照预设策略扩展纹理
             */
            bool extendTexture();

            String          name;          /// 字体外观名称
            MaterialPtr     material;      /// 对应字体外观的材质
            BlockMap        blockmap;      /// blockmap
            BlockList       available;     /// 空闲可用block链表
            Blocks          unavailable;   /// 空闲不可用block，按照行列编号作为key存放，方便扩展纹理时候从blocks放到free里面
        };

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
        bool insertBlock(FontFreeTypePtr font, FacePtr &face, BlockPtr &block);

        bool doStrategyAuto(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch);

        bool doStrategyLRU(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch);

        bool doStrategyAppend(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch);

        bool createFace(const String &fontName, FacePtr &face);

    protected:
        FaceMap     mFaces;
        CharMap     mCharmap;           /// 存储所有字符，方便用于查找
    };
}


#endif  /*__T3D_FREE_TYPE_ATLAS_H__*/
