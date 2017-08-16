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

#include "Resource/T3DFreeTypeAtlas.h"
#include "Resource/T3DFontManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(FreeTypeAtlas);

    FreeTypeAtlas::FacePtr FreeTypeAtlas::Face::create(const String &name, size_t index)
    {
        FreeTypeAtlas::FacePtr face = new Face(name);
        if (face != nullptr && face->init(index))
        {
            face->release();
        }
        else
        {
            T3D_SAFE_DELETE(face);
        }
        return face;
    }

    FreeTypeAtlas::Face::Face(const String &n)
        : name(n)
    {
    }

    FreeTypeAtlas::Face::~Face()
    {
    }

    bool FreeTypeAtlas::Face::init(size_t index)
    {
        bool ret = false;

        do 
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            // ������������Ҫ��block����
            int32_t blockCols = config.maxTexWidth / config.blockWidth;
            int32_t blockRows = config.maxTexHeight / config.blockHeight;
            int32_t blockCount = blockCols * blockRows;
            Size blockSize(config.blockWidth, config.blockHeight);
            int32_t col = 0, row = 0;

            int32_t cols = config.initTexWidth / config.blockWidth;
            int32_t rows = config.initTexHeight / config.blockHeight;

            // ���س�ʼ���ʺ�����
            std::stringstream ss;
            ss << name << "_#" << index;
            material = MaterialManager::getInstance().loadMaterial(ss.str(), Material::E_MT_MANUAL);

            TexturePtr texture = material->getTexture(0);

            if (texture == nullptr)
            {
                // û�����������״δ������ʣ����������в��������ȡ����
                if (!createTexture(config.initTexWidth, config.initTexHeight, texture))
                {
                    break;
                }

                material->setTexture(0, texture);
            }

            // �������е�block���ѱ��õķŵ�����map��
            for (int32_t i = 0; i < blockCount; i++)
            {
                BlockPtr block = new Block();
                block->release();
                block->face = this;
                block->area.left = i % blockCols * config.blockWidth;
                block->area.right = block->area.left + config.blockWidth - 1;
                block->area.top = i / blockCols * config.blockHeight;
                block->area.bottom = block->area.top + config.blockHeight - 1;

                int32_t blockID = row * blockCols + col;

                if (col < cols && row < rows)
                {
                    // ��ʼ�����С����Χ�ڣ�����ڿ���������
                    available.push_back(block);
                }
                else
                {
                    // ���ڳ�ʼ���������ֻ�ܷŵ���cache�����Ҫ��չ�����ʱ���ٶ�̬����������������
                    unavailable.insert(BlocksValue(blockID, block));
                }

                if (col + 1 >= blockCols)
                {
                    col = 0;
                    row += 1;
                }
                else
                {
                    col += 1;
                }
            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::Face::isBlockAvailable() const
    {
        return (!available.empty());
    }

    bool FreeTypeAtlas::Face::isTextureAvailable() const
    {
        return (!unavailable.empty());
    }

    bool FreeTypeAtlas::Face::createTexture(size_t texWidth, size_t texHeight, TexturePtr &texture)
    {
        bool ret = false;

        do 
        {
            std::stringstream ss;
            ss << name << "_tex_" << texWidth << "x" << texHeight;
            texture = TextureManager::getInstance().loadTexture(ss.str(), texWidth, texHeight, 1, E_PF_A8R8G8B8, Texture::E_TU_BLANK);
            if (texture == nullptr)
            {
                T3D_LOG_ERROR("Load font %s texture failed !", ss.str().c_str());
                break;
            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::Face::createBlock(size_t fontSize, BlockPtr &block)
    {
        bool ret = false;

        do 
        {
            auto itr = blockmap.find(fontSize);
            if (itr == blockmap.end())
            {
                auto r = blockmap.insert(BlockMapValue(fontSize, BlockList()));
                if (!r.second)
                {
                    T3D_LOG_ERROR("Insert font block [%s_%u] failed !", name.c_str(), fontSize);
                    break;
                }

                // ��face�Ŀ������������ȡһ�����е�block�������ŵ�blockmap��
                block = available.front();
                available.pop_front();
                block->size = fontSize;
                block->offset.x = block->offset.y = 0;
                r.first->second.push_back(block);
            }
            else
            {
                // ��face�Ŀ������������ȡһ�����е�block�������ŵ�blockmap��
                block = available.front();
                available.pop_front();
                block->size = fontSize;
                block->offset.x = block->offset.y = 0;
                itr->second.push_back(block);
            }

            ret = true;
        } while (0);
        
        return ret;
    }

    bool FreeTypeAtlas::Face::extendTexture()
    {
        bool ret = false;

        do 
        {
            TexturePtr srcTexture = material->getTexture(0);

            const FontConfig &config = FontManager::getInstance().getConfig();

            int32_t texWidth = srcTexture->getTexWidth();
            int32_t texHeight = srcTexture->getTexHeight();

            int32_t newTexWidth = 0;
            int32_t newTexHeight = 0;

            if (texWidth * 2 <= config.maxTexWidth)
            {
                // �Ȱ��տ�������ڵ���������
                newTexWidth = texWidth * 2;
                newTexHeight = texHeight;
            }
            else if (texWidth < config.maxTexWidth && texWidth * 2 >= config.maxTexWidth)
            {
                // ʣ��ռ��޷���������������ֱ����չ��������
                newTexWidth = config.maxTexWidth;
                newTexHeight = texHeight;
            }
            else if (texWidth >= config.maxTexWidth
                && texHeight * 2 <= config.maxTexHeight)
            {
                // �Ѿ��޷����տ��������ֻ�ܰ��ո߶���������
                newTexWidth = config.maxTexWidth;
                newTexHeight = texHeight * 2;
            }
            else if (texWidth >= config.maxTexWidth
                && texHeight < config.maxTexHeight && texHeight * 2 >= config.maxTexHeight)
            {
                // ʣ��ռ��޷����������߶�������ֱ����չ���߶����
                newTexWidth = config.maxTexWidth;
                newTexHeight = config.maxTexHeight;
            }

            // �ѻ��治���õ�block�ŵ����õ�block������
            int32_t blockCols = texWidth / config.blockWidth;
            int32_t blockRows = texHeight / config.blockHeight;

            int32_t newBlockCols = newTexWidth / config.blockWidth;
            int32_t newBlockRows = newTexHeight / config.blockHeight;

            int32_t maxBlockCols = config.maxTexWidth / config.blockWidth;
            int32_t maxBlockRows = config.maxTexHeight / config.blockHeight;

            bool found = true;
            int32_t x = 0, y = 0;
            for (y = 0; y < newBlockRows; y++)
            {
                for (x = 0; x < newBlockCols; x++)
                {
                    if (x >= blockCols || y >= blockRows)
                    {
                        int32_t blockID = y * maxBlockCols + x;
                        auto itr = unavailable.find(blockID);
                        if (itr != unavailable.end())
                        {
                            BlockPtr block = itr->second;
                            unavailable.erase(itr);
                            available.push_back(block);
                        }
                        else
                        {
                            found = false;
                            break;
                        }
                    }
                }

                if (!found)
                {
                    break;
                }
            }

            if (!found)
                break;

            // �´���һ������
            TexturePtr newTexture;
            if (!createTexture(newTexWidth, newTexHeight, newTexture))
            {
                break;
            }

            // �Ѿ�������������ݸ��Ƶ���������ͬ����
            Rect srcRect(Point(0, 0), Size(texWidth, texHeight));
            Rect dstRect = srcRect;
            if (!srcTexture->copyTo(newTexture, &dstRect, &srcRect))
            {
                T3D_LOG_ERROR("Copy source texture data to new texture failed !");
                break;
            }

            material->setTexture(0, newTexture);
            T3D_TEXTURE_MGR.unloadTexture(srcTexture);

            ret = true;
        } while (0);

        return ret;
    }

    FreeTypeAtlas::FreeTypeAtlas()
    {

    }

    FreeTypeAtlas::~FreeTypeAtlas()
    {

    }

    bool FreeTypeAtlas::hasCharacter(int32_t code, Font::CharPtr &ch) const
    {
        bool found = false;

        auto itr = mCharmap.find(code);
        if (itr != mCharmap.end())
        {
            ch = itr->second;
            found = true;
        }

        return found;
    }

    bool FreeTypeAtlas::updateContent(FontFreeTypePtr font, const String &text, MaterialPtr &material, Font::CharSet &set, Size &size)
    {
        bool ret = false;

        do 
        {
            UTF16String u16text;
            TextCodec::getInstance().UTF8ToUTF16(text, u16text);

            FT_Face ftFace = font->getFontFace();

            size.width = 0;
            size.height = 0;

            set.clear();
            Font::CharPtr ch;
            size_t i = 0;
            for (i = 0; i < u16text.length(); ++i)
            {
                // װ�����ε����β�
                size_t code = u16text[i];

                // ��������������Ƿ����ֳ��ַ�
                if (!lookupCharMap(font, code, ch))
                {
                    // û�ҵ����棬�²���һ���ַ�������������
                    if (!insertCharMap(font, code, ch))
                    {
                        continue;
                    }

                    set.push_back(ch);
                    size.width += ch->mArea.width();
                }
                else
                {
                    set.push_back(ch);
                    size.width += ch->mArea.width();
                }
            }

            if (ch != nullptr)
            {
                Block *block = (Block *)(ch->mBlock);
                Face *face = (Face *)(block->face);
                material = face->material;
                size.height = font->getFontHeight();
                ret = true;
            }
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::lookupCharMap(FontFreeTypePtr font, int32_t code, Font::CharPtr &ch)
    {
        bool found = false;

        auto itr = mCharmap.find(code);
        if (itr != mCharmap.end())
        {
            found = true;
            ch = itr->second;
        }

        return found;
    }

    bool FreeTypeAtlas::insertCharMap(FontFreeTypePtr font, int32_t code, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            // ����һ���ʺϵ�block����������ֺŵ��ı�
            FacePtr face;
            BlockPtr block;
            if (!lookupBlock(font, face, block))
            {
                // û���ҵ�һ�����ʵ����飬ֻ�ܲ���һ���µ�����
                if (!insertBlock(font, face, block))
                {
                    T3D_LOG_ERROR("Insert block failed !");
                }
            }

            // ��ͨ����������� freetype �����λͼ
            Size charSize;
            if (!font->loadBitmap(code, charSize))
            {
                break;
            }

            if (block != nullptr)
            {
                // �ж��Ƿ���Ҫ����
                if (block->offset.x + charSize.width > block->area.width())
                {
                    // û�취�ˣ�ֻ�ܻ���
                    block->offset.x = 0;
                    block->offset.y += charSize.height;
                }

                // �ж��Ƿ񹻿ռ�����µ��ַ�
                if (block->offset.y + charSize.height > block->area.height())
                {
                    // block�ռ䲻�������ַ���ֻ���½�һ��block
                    block = nullptr;

                    if (!insertBlock(font, face, block))
                    {
                        T3D_LOG_ERROR("Insert block failed !");
                    }
                }
            }

            Point uv;
            Rect dstRect;

            if (block != nullptr)
            {
                // ��~~~ ��Ȼ�п���������Բ����µ�block���ǲ������ˣ�ֱ��������
                // ���������Ѿ�ͨ�� freetype �������λͼ�ˣ��������ﲻ�ü�����
                uv.x = block->area.left + block->offset.x;
                uv.y = block->area.top + block->offset.y;

                dstRect.left = uv.x;
                dstRect.top = uv.y;
                dstRect.right = dstRect.left + charSize.width - 1;
                dstRect.bottom = dstRect.top + charSize.height - 1;

                // ���ַ���Ϣ��������
                ch = new Font::Char();
                ch->release();
                ch->mArea = dstRect;

                block->offset.x += charSize.width;
            }
            else
            {
                // �����ˣ� ����blockʧ���ˣ�����û�ռ��ˣ��������õĲ�ͬ����ȥ�����µĿռ�
                if (FontConfig::E_STRATEGY_LRU == config.strategy)
                {
                    // ֱ��ʹ��LRU��̭û�õ��ַ��������̭�󻹲�����ֻ�ܷ��ش���
                    if (!doStrategyLRU(font, face, block, charSize, ch))
                    {
                        break;
                    }
                }
                else if (FontConfig::E_STRATEGY_APPEND == config.strategy)
                {
                    // ���ֲ��Ծ�ֱ���½����ʺ�������������ַ�
                    if (!doStrategyAppend(font, face, block, charSize, ch))
                    {
                        break;
                    }
                }
                else
                {
                    // �Զ���������ʹ��LRU��̭û�õ��ַ���ʵ��û����̭��ʱ�����½���������������ַ�
                    if (!doStrategyAuto(font, face, block, charSize, ch))
                    {
                        break;
                    }
                }

                dstRect = ch->mArea;
            }

            T3D_ASSERT(block != nullptr);

            // ��Ⱦ������ָ������
            T3D_LOG_INFO("code [%d]", code);

            TexturePtr texture = face->material->getTexture(0);
            if (!font->renderAt(texture, dstRect))
            {
                T3D_LOG_ERROR("Render at texture failed !");
                break;
            }

            ch->mBlock = block;
            ch->mCode = code;
            block->charmap.insert(CharMapValue(code, ch));
            mCharmap.insert(CharMapValue(code, ch));

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::lookupBlock(FontFreeTypePtr font, FacePtr &face, BlockPtr &block)
    {
        bool found = false;

        // ���������������������Ҷ�Ӧ���������Ϣ
        auto itr = mFaces.find(font->getName());
        if (itr != mFaces.end())
        {
            // �ҵ��ж�Ӧ�����������Ϣ
            FaceList &faceList = itr->second;
            
            // ���������ͬ������۲����Ƿ�����ͬ�����С������Ϣ
            for (auto i = faceList.begin(); i != faceList.end(); ++i)
            {
                FacePtr temp = *i;

                // ������ͬ������ۺ���ͬ�ֺţ����Ƿ��ж�Ӧ������
                auto ii = temp->blockmap.find(font->getFontSize());
                if (ii != temp->blockmap.end())
                {
                    BlockList &blockList = ii->second;

                    // ����϶��������һ������Ϊֻ�����һ�����п����Ǵ��ڿ��������block
                    block = blockList.back();

                    

                    face = temp;
                    found = true;
                    break;
                }
                else
                {
                    // ���û����ͬ�ֺŵ�block����ȥ��������������һ������Ϊֻ�����һ�����п��ܴ��ڿ�������
                    face = faceList.back();
                }
            }
        }

        return found;
    }

    bool FreeTypeAtlas::insertBlock(FontFreeTypePtr font, FacePtr &face, BlockPtr &block)
    {
        bool ret = false;

        do
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            Point uv;
            Point offset;
            Size blockSize(config.blockWidth, config.blockHeight);

            if (mFaces.empty() || face == nullptr)
            {
                // ֱ�Ӵ���һ��������۶���
                const String &fontName = font->getName();
                if (!createFace(fontName, face))
                {
                    break;
                }
            }

            T3D_ASSERT(face != nullptr);

            if (face->isBlockAvailable())
            {
                // ���п���block��ֱ�Ӵ���һ��block
                if (!face->createBlock(font->getFontSize(), block))
                {
                    break;
                }
            }
            else if (face->isTextureAvailable())
            {
                // ֱ����չ����
                if (!face->extendTexture())
                {
                    break;
                }

                // ��������һ��block
                if (!face->createBlock(font->getFontSize(), block))
                {
                    break;
                }
            }
            else
            {
                // û�п���block��ֱ�Ӹ�������ʧ���ˣ����������Ԥ����Դ���
                break;
            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::doStrategyAuto(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
            // ������������̭�ϵ�û�õ��ַ��������û����̭���������ɶ�һ��������ۺ��������
            if (!doStrategyLRU(font, face, block, charSize, ch))
            {
                if (!doStrategyAppend(font, face, block, charSize, ch))
                {
                    break;
                }
            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::doStrategyLRU(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
            // ������ֻ����LRU��̭�ϵģ����û�п�����̭�ģ�ֱ�ӷ���false
            size_t fontSize = font->getFontSize();

            auto itr = face->blockmap.find(fontSize);

            if (itr != face->blockmap.end())
            {
                BlockList &blockList = itr->second;

                for (auto i = blockList.begin(); i != blockList.end(); ++i)
                {
                    BlockPtr tempBlock = *i;

                    for (auto ii = tempBlock->charmap.begin(); ii != tempBlock->charmap.end(); ++ii)
                    {
                        Font::CharPtr temp = ii->second;

                        // ��Ϊ������ͬ����������߶ȿ϶����
                        T3D_ASSERT(charSize.height == temp->mArea.height());

                        if (temp->referCount() == 2 && charSize.width <= temp->mArea.width())
                        {
                            // û���ⲿ���ã������ֳ������ܴ���������ַ�
                            ch = temp;
                            tempBlock->charmap.erase(temp->mCode);
                            mCharmap.erase(temp->mCode);
                            block = tempBlock;
                            ret = true;
                            break;
                        }
                    }

                    if (ret)
                    {
                        break;
                    }
                }

                ++itr;
            }
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::doStrategyAppend(FontFreeTypePtr font, FacePtr face, BlockPtr &block, const Size &charSize, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
            if (!insertBlock(font, face, block))
            {
                break;
            }

            T3D_ASSERT(face != nullptr && block != nullptr);

            ch = new Font::Char();
            ch->release();
            ch->mBlock = block;

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::createFace(const String &fontName, FacePtr &face)
    {
        bool ret = false;

        do 
        {
            auto itr = mFaces.find(fontName);

            if (itr != mFaces.end())
            {
                // �Ѿ�����ͬ��������۶����ˣ���ȡ����������������
                FaceList &faceList = itr->second;
                size_t faceIdx = faceList.size();
                face = Face::create(fontName, faceIdx);
                faceList.push_back(face);
            }
            else
            {
                // û��������۶���ֱ�Ӳ���һ������
                auto r = mFaces.insert(FaceMapValue(fontName, FaceList()));
                if (!r.second)
                {
                    T3D_LOG_ERROR("Insert font face [%s] failed !", fontName.c_str());
                    break;
                }

                size_t faceIdx = 0;
                face = Face::create(fontName, faceIdx);
                r.first->second.push_back(face);
            }

            ret = true;
        } while (0);

        return ret;
    }
}
