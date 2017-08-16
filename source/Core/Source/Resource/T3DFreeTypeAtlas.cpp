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

            // 计算这里面需要的block数量
            int32_t blockCols = config.maxTexWidth / config.blockWidth;
            int32_t blockRows = config.maxTexHeight / config.blockHeight;
            int32_t blockCount = blockCols * blockRows;
            Size blockSize(config.blockWidth, config.blockHeight);
            int32_t col = 0, row = 0;

            int32_t cols = config.initTexWidth / config.blockWidth;
            int32_t rows = config.initTexHeight / config.blockHeight;

            // 加载初始材质和纹理
            std::stringstream ss;
            ss << name << "_#" << index;
            material = MaterialManager::getInstance().loadMaterial(ss.str(), Material::E_MT_MANUAL);

            TexturePtr texture = material->getTexture(0);

            if (texture == nullptr)
            {
                // 没有纹理，代表首次创建材质，而不是已有材质里面获取回来
                if (!createTexture(config.initTexWidth, config.initTexHeight, texture))
                {
                    break;
                }

                material->setTexture(0, texture);
            }

            // 创建所有的block，把备用的放到备用map里
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
                    // 初始纹理大小区域范围内，存放在可用链表里
                    available.push_back(block);
                }
                else
                {
                    // 不在初始纹理区域里，只能放到后备cache里，等需要扩展纹理的时候再动态调整到可用链表里
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

                // 从face的空闲链表里面获取一个空闲的block出来，放到blockmap中
                block = available.front();
                available.pop_front();
                block->size = fontSize;
                block->offset.x = block->offset.y = 0;
                r.first->second.push_back(block);
            }
            else
            {
                // 从face的空闲链表里面获取一个空闲的block出来，放到blockmap中
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
                // 先按照宽度是现在的两倍增长
                newTexWidth = texWidth * 2;
                newTexHeight = texHeight;
            }
            else if (texWidth < config.maxTexWidth && texWidth * 2 >= config.maxTexWidth)
            {
                // 剩余空间无法按照两倍增长，直接扩展到宽度最大
                newTexWidth = config.maxTexWidth;
                newTexHeight = texHeight;
            }
            else if (texWidth >= config.maxTexWidth
                && texHeight * 2 <= config.maxTexHeight)
            {
                // 已经无法按照宽度增长，只能按照高度两倍增长
                newTexWidth = config.maxTexWidth;
                newTexHeight = texHeight * 2;
            }
            else if (texWidth >= config.maxTexWidth
                && texHeight < config.maxTexHeight && texHeight * 2 >= config.maxTexHeight)
            {
                // 剩余空间无法按照两倍高度增长，直接扩展到高度最大
                newTexWidth = config.maxTexWidth;
                newTexHeight = config.maxTexHeight;
            }

            // 把缓存不可用的block放到可用的block链表里
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

            // 新创建一个纹理
            TexturePtr newTexture;
            if (!createTexture(newTexWidth, newTexHeight, newTexture))
            {
                break;
            }

            // 把旧纹理的所有数据复制到新纹理相同区域
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
                // 装载字形到字形槽
                size_t code = u16text[i];

                // 从区块里面查找是否有现成字符
                if (!lookupCharMap(font, code, ch))
                {
                    // 没找到缓存，新插入一个字符到材质纹理中
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

            // 查找一个适合的block给存放这种字号的文本
            FacePtr face;
            BlockPtr block;
            if (!lookupBlock(font, face, block))
            {
                // 没有找到一个合适的区块，只能插入一个新的区块
                if (!insertBlock(font, face, block))
                {
                    T3D_LOG_ERROR("Insert block failed !");
                }
            }

            // 先通过字体对象用 freetype 库加载位图
            Size charSize;
            if (!font->loadBitmap(code, charSize))
            {
                break;
            }

            if (block != nullptr)
            {
                // 判断是否需要换行
                if (block->offset.x + charSize.width > block->area.width())
                {
                    // 没办法了，只能换行
                    block->offset.x = 0;
                    block->offset.y += charSize.height;
                }

                // 判断是否够空间放入新的字符
                if (block->offset.y + charSize.height > block->area.height())
                {
                    // block空间不足放这个字符，只能新建一个block
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
                // 咦~~~ 居然有空闲区域可以插入新的block，那不客气了，直接拿来用
                // 由于上面已经通过 freetype 库加载了位图了，所以这里不用加载了
                uv.x = block->area.left + block->offset.x;
                uv.y = block->area.top + block->offset.y;

                dstRect.left = uv.x;
                dstRect.top = uv.y;
                dstRect.right = dstRect.left + charSize.width - 1;
                dstRect.bottom = dstRect.top + charSize.height - 1;

                // 把字符信息缓存起来
                ch = new Font::Char();
                ch->release();
                ch->mArea = dstRect;

                block->offset.x += charSize.width;
            }
            else
            {
                // 杯具了！ 插入block失败了，纹理没空间了，按照配置的不同策略去申请新的空间
                if (FontConfig::E_STRATEGY_LRU == config.strategy)
                {
                    // 直接使用LRU淘汰没用的字符，如果淘汰后还不够，只能返回错误
                    if (!doStrategyLRU(font, face, block, charSize, ch))
                    {
                        break;
                    }
                }
                else if (FontConfig::E_STRATEGY_APPEND == config.strategy)
                {
                    // 这种策略就直接新建材质和纹理来存放新字符
                    if (!doStrategyAppend(font, face, block, charSize, ch))
                    {
                        break;
                    }
                }
                else
                {
                    // 自动策略是先使用LRU淘汰没用的字符，实在没有淘汰的时候，再新建新纹理来存放新字符
                    if (!doStrategyAuto(font, face, block, charSize, ch))
                    {
                        break;
                    }
                }

                dstRect = ch->mArea;
            }

            T3D_ASSERT(block != nullptr);

            // 渲染到纹理指定区域
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

        // 先在所有字体外观里面查找对应字体外观信息
        auto itr = mFaces.find(font->getName());
        if (itr != mFaces.end())
        {
            // 找到有对应的字体外观信息
            FaceList &faceList = itr->second;
            
            // 逐个遍历相同字体外观查找是否有相同字体大小区块信息
            for (auto i = faceList.begin(); i != faceList.end(); ++i)
            {
                FacePtr temp = *i;

                // 查找相同字体外观和相同字号，看是否有对应的区块
                auto ii = temp->blockmap.find(font->getFontSize());
                if (ii != temp->blockmap.end())
                {
                    BlockList &blockList = ii->second;

                    // 这里肯定返回最后一个，因为只有最后一个才有可能是存在空闲区域的block
                    block = blockList.back();

                    

                    face = temp;
                    found = true;
                    break;
                }
                else
                {
                    // 如果没有相同字号的block，就去字体外观链上最后一个，因为只有最后一个才有可能存在空闲区域
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
                // 直接创建一个字体外观对象
                const String &fontName = font->getName();
                if (!createFace(fontName, face))
                {
                    break;
                }
            }

            T3D_ASSERT(face != nullptr);

            if (face->isBlockAvailable())
            {
                // 还有空闲block，直接创建一个block
                if (!face->createBlock(font->getFontSize(), block))
                {
                    break;
                }
            }
            else if (face->isTextureAvailable())
            {
                // 直接扩展纹理
                if (!face->extendTexture())
                {
                    break;
                }

                // 继续创建一个block
                if (!face->createBlock(font->getFontSize(), block))
                {
                    break;
                }
            }
            else
            {
                // 没有空闲block，直接告诉外面失败了，让外面根据预设策略处理
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
            // 本策略是先淘汰老的没用的字符，如果都没得淘汰，就新生成多一个字体外观和相关纹理
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
            // 本策略只是用LRU淘汰老的，如果没有可以淘汰的，直接返回false
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

                        // 因为字体相同，所以字体高度肯定相等
                        T3D_ASSERT(charSize.height == temp->mArea.height());

                        if (temp->referCount() == 2 && charSize.width <= temp->mArea.width())
                        {
                            // 没有外部引用，并且现成区域能存放新增的字符
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
                // 已经存在同样字体外观对象了，获取回来，插入链表里
                FaceList &faceList = itr->second;
                size_t faceIdx = faceList.size();
                face = Face::create(fontName, faceIdx);
                faceList.push_back(face);
            }
            else
            {
                // 没有字体外观对象，直接插入一个链表
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
