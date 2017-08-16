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
        // �洢�����ַ���Ӧ����Ϣ
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

            Object          *face;      /// ����������������ۣ�Ϊ�˱���ѭ�����õ�ʱ�޷��ͷţ�����ֱ����ָ�룬��������ָ��
            size_t          size;       /// �����С
            Rect            area;       /// block������
            Point           offset;     /// �ַ���ʼλ��
            CharMap         charmap;    /// �ַ���
        };

        T3D_DECLARE_SMART_PTR(Block);

        // �洢������ͬ������ۺ���ͬ�����С��block��Ϣ��һ��Ԫ����һ��block
        typedef std::list<BlockPtr>             BlockList;
        typedef BlockList::iterator             BlockListItr;
        typedef BlockList::const_iterator       BlockListConstItr;
        typedef BlockList::value_type           BlockListValue;

        // �洢������ͬ������۲�ͬ��С��block��Ϣ����ͬ�����С��ͨ��һ��list��������
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
             * @brief �Ƿ��п���block��ʹ��
             */
            bool isBlockAvailable() const;

            /**
             * @brief �����Ƿ��пռ����
             */
            bool isTextureAvailable() const;

            /**
             * @brief ��������
             */
            bool createTexture(size_t texWidth, size_t texHeight, TexturePtr &texture);

            /**
             * @brief ����block
             */
            bool createBlock(size_t fontSize, BlockPtr &block);

            /**
             * @brief ����Ԥ�������չ����
             */
            bool extendTexture();

            String          name;          /// �����������
            MaterialPtr     material;      /// ��Ӧ������۵Ĳ���
            BlockMap        blockmap;      /// blockmap
            BlockList       available;     /// ���п���block����
            Blocks          unavailable;   /// ���в�����block���������б����Ϊkey��ţ�������չ����ʱ���blocks�ŵ�free����
        };

        // ��ͬ��������ۣ���һ������������һ��Ԫ����һ��������۶�Ӧ��blockmap
        typedef std::list<FacePtr>              FaceList;
        typedef FaceList::iterator              FaceListItr;
        typedef FaceList::const_iterator        FaceListConstItr;
        typedef FaceList::value_type            FaceListValue;

        // �洢���в�ͬ�������
        typedef std::map<String, FaceList>      FaceMap;
        typedef FaceMap::iterator               FaceMapItr;
        typedef FaceMap::const_iterator         FaceMapConstItr;
        typedef FaceMap::value_type             FaceMapValue;

        FreeTypeAtlas();

        /**
         * @brief ����charmap
         * @param [in] code : �ַ�����
         * @param [out] block : ���ص��������
         * @param [out] ch : ���ص��ַ���Ϣ����
         * @return ���ҵ�����true�����򷵻�false
         */
        bool lookupCharMap(FontFreeTypePtr font, int32_t code, Font::CharPtr &ch);

        /**
         * @brief �����ַ���charmap
         * @param [in] code : �ַ�����
         * @param [in] block : �������
         * @param [out] ch : ���ص��ַ���Ϣ����
         * @return ����ɹ�����true�����򷵻�false
         */
        bool insertCharMap(FontFreeTypePtr font, int32_t code, Font::CharPtr &ch);

        /**
         * @brief �����ʺϵ�����
         * @param [out] block : ���ص��������
         * @return ���ҵ��ʺϵķ���true�����򷵻�false
         */
        bool lookupBlock(FontFreeTypePtr font, FacePtr &face, BlockPtr &block);

        /**
         * @brief ����һ������
         * @param [out] block : ���ص��������
         * @return ����ɹ�����true�����򷵻�false
         */
        bool insertBlock(FontFreeTypePtr font, FacePtr &face, BlockPtr &block);

        bool doStrategyAuto(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch);

        bool doStrategyLRU(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch);

        bool doStrategyAppend(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch);

        bool createFace(const String &fontName, FacePtr &face);

    protected:
        FaceMap     mFaces;
        CharMap     mCharmap;           /// �洢�����ַ����������ڲ���
    };
}


#endif  /*__T3D_FREE_TYPE_ATLAS_H__*/
