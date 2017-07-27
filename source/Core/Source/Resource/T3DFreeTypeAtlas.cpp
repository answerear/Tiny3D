

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

    FreeTypeAtlas::Face::Face(const String &n, MaterialPtr m)
        : name(n)
        , material(m)
    {
        const FontConfig &config = FontManager::getInstance().getConfig();

        // 计算这里面需要的block数量
        int32_t blockCols = config.maxTexWidth / config.blockWidth;
        int32_t blockRows = config.maxTexHeight / config.blockHeight;
        int32_t blockCount = blockCols * blockRows;
        Size blockSize(config.blockWidth, config.blockHeight);

        // 创建所有的block，放到空闲链表里
        for (int32_t i = 0; i < blockCount; i++)
        {
            BlockPtr block = new Block();
            block->release();
            block->face = this;
            block->area.left = i % blockCols * config.blockWidth;
            block->area.right = block->area.left + config.blockWidth - 1;
            block->area.top = i / blockCols * config.blockHeight;
            block->area.bottom = block->area.top + config.blockHeight - 1;
            free.push_back(block);
        }
    }

    FreeTypeAtlas::Face::~Face()
    {
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

    bool FreeTypeAtlas::updateContent(FontFreeTypePtr font, const String &text, MaterialPtr &material, Font::CharSet &set)
    {
        bool ret = false;

        do 
        {
            UTF16String u16text;
            TextCodec::getInstance().UTF8ToUTF16(text, u16text);

            FT_Face ftFace = font->getFontFace();

            set.clear();
            Font::CharPtr ch;
            size_t i = 0;
            for (i = 0; i < u16text.length(); ++i)
            {
                // 装载字形到字形槽
                size_t code = u16text[i];
                if (FT_Load_Char(ftFace, code, FT_LOAD_RENDER))
                {
                    continue;
                }

                // 从区块里面查找是否有现成字符
                if (!lookupCharMap(font, code, ch))
                {
                    // 没找到缓存，新插入一个字符到材质纹理中
                    if (!insertCharMap(font, code, ch))
                    {
                        continue;
                    }

                    set.push_back(ch);
                }
            }

            if (ch != nullptr)
            {
                BlockPtr block = smart_pointer_cast<Block>(ch->mBlock);
                FacePtr face = smart_pointer_cast<Face>(block->face);
                material = face->material;
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

            if (block != nullptr)
            {
                // 获取字体高度
                int32_t fontWidth = font->getFontWidth();
                int32_t fontHeight = font->getFontHeight();

                // 先判断是否够空间放入新的字符
                if (block->offset.x + fontWidth > block->area.width()
                    && block->offset.y + fontHeight > block->area.height())
                {
                    // block空间不足放这个字符，只能新建一个block
                    block = nullptr;

                    if (!insertBlock(font, face, block))
                    {
                        T3D_LOG_ERROR("Insert block failed !");
                    }
                }
            }

            if (block == nullptr)
            {
                // 插入block失败了，纹理没空间了，按照配置的不同策略去申请新的空间
                if (FontConfig::E_STRATEGY_LRU == config.strategy)
                {
                    // 直接使用LRU淘汰没用的字符，如果淘汰后还不够，只能返回错误
                    if (!doStrategyLRU(font, face, block, ch))
                    {
                        break;
                    }
                }
                else if (FontConfig::E_STRATEGY_APPEND == config.strategy)
                {
                    // 这种策略就直接新建材质和纹理来存放新字符
                    if (!doStrategyAppend(font, face, block, ch))
                    {
                        break;
                    }
                }
                else
                {
                    // 自动策略是先使用LRU淘汰没用的字符，实在没有淘汰的时候，再新建新纹理来存放新字符
                    if (!doStrategyAuto(font, face, block, ch))
                    {
                        break;
                    }
                }
            }
            else
            {
                // 咦，居然有空闲区域可以插入新的block，那不好意思了，只能拿来用

                // 先通过字体对象用 freetype 库加载位图
                Size charSize;

                if (!font->loadBitmap(code, charSize))
                {
                    T3D_LOG_ERROR("Load font bitmap failed !");
                    break;
                }

                // 判断是否需要换行
                if (block->offset.x + charSize.width > block->area.width())
                {
                    // 没办法了，只能换行
                    block->offset.x = 0;
                    block->offset.y += charSize.height;
                }

                TexturePtr texture = face->material->getTexture(0);
                Point uv(block->area.left + block->offset.x, block->area.top + block->offset.y);
                Rect dstRect(uv, charSize);
                if (!font->renderAt(texture, dstRect))
                {
                    T3D_LOG_ERROR("Render at texture failed !");
                    break;
                }

                ch = new Font::Char();
                ch->release();
                ch->mCode = code;
                ch->mBlock = block;
                ch->mArea = dstRect;

                block->charmap.insert(CharMapValue(code, ch));
                ch->mBlock = block;

                mCharmap.insert(CharMapValue(code, ch));
            }

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
                    found = true;
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

    bool FreeTypeAtlas::insertBlock(FontFreeTypePtr font, FacePtr face, BlockPtr &block)
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
                // 先加载一个新的材质和纹理
                const String &fontName = font->getName();
                size_t faceIdx = 0;
                std::stringstream ss;
                ss << fontName << "_#" << faceIdx;
                String name = ss.str();

                MaterialPtr material;
                if (!loadMaterial(name, font, material, config.incTexWidth, config.incTexHeight))
                {
                    break;
                }

                // 插入一个新face
                FacePtr newFace;
                if (!createFace(material, fontName, newFace))
                {
                    break;
                }

                face = newFace;
            }

            T3D_ASSERT(face != nullptr);

            TexturePtr texture = face->material->getTexture(0);
            int32_t texWidth = texture->getTexWidth();
            int32_t texHeight = texture->getTexHeight();

            if (!face->free.empty())
            {
                // 还有空闲block

                size_t blocksUsed = face->blockmap.size();
                size_t blocksFree = face->free.size();

                {
                    // 直接扩展纹理
                    TexturePtr newTexture;
                    if (!extendTexture(face->material->getName(), texture, newTexture))
                    {
                        break;
                    }

                    // 在新纹理扩展区域
                    face->offset.x = texWidth;
                    face->offset.y = 0;

                    blockSize.width = config.blockWidth;
                    blockSize.height = config.blockHeight;
                    offset.setZero();
                }
                        
                if (!createBlock(font->getFontSize(), Rect(uv, blockSize), offset, face, block))
                {
                    break;
                }

                // 跳到下一个block开始位置，为将来新开辟block空间做准备
                face->offset.x += config.blockWidth;
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

    bool FreeTypeAtlas::doStrategyAuto(FontFreeTypePtr font, FacePtr face, BlockPtr &block, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::doStrategyLRU(FontFreeTypePtr font, FacePtr face, BlockPtr &block, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::doStrategyAppend(FontFreeTypePtr font, FacePtr face, BlockPtr &block, Font::CharPtr &ch)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::loadMaterial(const String &name, FontFreeTypePtr font, MaterialPtr &material, size_t texWidth, size_t texHeight)
    {
        bool ret = false;

        do
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            material = MaterialManager::getInstance().loadMaterial(name, Material::E_MT_MANUAL);
            if (material == nullptr)
            {
                T3D_LOG_ERROR("Load font %s material failed !", name.c_str());
                break;
            }

            TexturePtr texture = material->getTexture(0);

            if (texture == nullptr)
            {
                // 没有纹理，代表首次创建材质，而不是已有材质里面获取回来
                if (!createTexture(name, texWidth, texHeight, texture))
                {
                    break;
                }

                material->setTexture(0, texture);
            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::createFace(MaterialPtr material, const String &fontName, FacePtr &face)
    {
        bool ret = false;

        do 
        {
            auto r = mFaces.insert(FaceMapValue(fontName, FaceList()));
            if (!r.second)
            {
                T3D_LOG_ERROR("Insert font face [%s] failed !", fontName.c_str());
                break;
            }

            face = new Face(fontName, material);
            face->release();

            r.first->second.push_back(face);
            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::createBlock(size_t fontSize, const Rect &area, const Point &offset, FacePtr face, BlockPtr &block)
    {
        bool ret = false;

        do 
        {
            auto r = face->blockmap.insert(BlockMapValue(fontSize, BlockList()));
            if (!r.second)
            {
                T3D_LOG_ERROR("Insert font block [%s_%u] failed !", face->name.c_str(), fontSize);
                break;
            }

            // 从face的空闲链表里面获取一个空闲的block出来，放到blockmap中
            block = face->free.front();
            face->free.pop_front();
            block->size = fontSize;
            block->offset.x = block->offset.y;
            r.first->second.push_back(block);

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::createTexture(const String &name, size_t texWidth, size_t texHeight, TexturePtr &texture)
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

    bool FreeTypeAtlas::copyBitmapToTexture(FontFreeTypePtr font, BlockPtr block, Font::CharPtr ch)
    {
        bool ret = false;

        do 
        {
            FT_Face ftFace = font->getFontFace();
            
            // 源位图区域
            Rect srcRect(Point(0, 0), Size(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows));

            uint8_t *srcData = ftFace->glyph->bitmap.buffer;
            int32_t srcPitch = ftFace->glyph->bitmap.pitch;
            int32_t srcWidth = ftFace->glyph->bitmap.width;
            int32_t srcHeight = ftFace->glyph->bitmap.rows;

            // 这里的bitmap_left有可能存在负值
            int32_t srcLeft = (ftFace->glyph->bitmap_left >= 0 ? ftFace->glyph->bitmap_left : 0);
            int32_t srcTop = ftFace->glyph->bitmap_top;

            // 基线
            int32_t baseline = (ftFace->size->metrics.ascender >> 6);
            // 字体高度
            int32_t fontHeight = (ftFace->size->metrics.height >> 6);

            // 目标纹理的起始位置
            int32_t dstLeft = block->offset.x + srcLeft;
            int32_t dstTop = block->offset.y + baseline - srcTop;

            // 水平步进
            int32_t advanceX = ftFace->glyph->metrics.horiAdvance;
            // 字体实际宽度，取步进值和位图宽度最大值
            int32_t fontWidth = std::max(advanceX, srcWidth);

            if (dstLeft + fontWidth > block->area.right)
            {
                // 需要换行了，重新计算block中字符区域的位置
                block->offset.x = block->area.left;
                block->offset.y += fontHeight;
                dstLeft = block->offset.x + srcLeft;
            }

            // 目标纹理绘制区域
            Rect dstRect(Point(dstLeft, dstTop), Size(srcWidth, srcHeight));
            T3D_ASSERT(dstRect.bottom <= block->area.bottom);

            // 字符区域，字符区域跟目标纹理绘制区域是不相同的，因为目标纹理区域是固定高度的，而绘制内容是实际绘制高度
            ch->mArea.left = block->offset.x;
            ch->mArea.right = ch->mArea.left + fontWidth - 1;
            ch->mArea.top = block->offset.y;
            ch->mArea.bottom = ch->mArea.top + fontHeight - 1;

            // 获取纹理缓存，写纹理
            FacePtr face = smart_pointer_cast<Face>(block->face);
            TexturePtr texture = face->material->getTexture(0);

            // 这里需要获取纹理数据来修改，后续再添加，这里需要修改纹理读写数据结构
            T3D_ASSERT(0);
            uint8_t *dstData = nullptr;
            int32_t dstPitch = 0;
            int32_t dstWidth = 0;
            int32_t dstHeight = 0;
            
            uint8_t *dstLine = dstData + dstTop * dstPitch + 4 * dstLeft;
            uint8_t *srcLine = srcData + (srcHeight - 1);

            // 纹理UV坐标是X向右为正，Y向下为正，而freetype里面的bitmap坐标是X向右为正，Y向上位正
            for (size_t y = 0; y < srcHeight; y++)
            {
                uint32_t *dst = (uint32_t *)dstLine;
                uint8_t *src = srcLine;
                for (size_t x = 0; x < srcWidth; x++)
                {
                    // 因为字体是灰度图，把字体存在alpha通道里备份，方便后续直接修改文本颜色，不用再解析字库
                    Color4 color(*src, *src, *src, *src);
                    *dst = color.A8R8G8B8();
                    dst++;
                    src++;
                }

                dstLine += dstPitch;
                srcLine -= srcPitch;
            }
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::extendTexture(const String &name, TexturePtr srcTexture, TexturePtr &newTexture)
    {
        bool ret = false;

        do 
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            int32_t texWidth = srcTexture->getTexWidth();
            int32_t texHeight = srcTexture->getTexHeight();

            size_t incTexWidth = (config.incTexWidth == 0 ? texWidth : config.incTexWidth);
            size_t incTexHeight = (config.incTexHeight == 0 ? texHeight : config.incTexHeight);

            int32_t newTexWidth = 0;
            int32_t newTexHeight = 0;

            if (texWidth + incTexWidth < config.maxTexWidth)
            {
                newTexWidth = texWidth + incTexWidth;
            }
            else
            {
                newTexWidth = config.maxTexWidth;
            }

            if (texHeight + incTexHeight < config.maxTexHeight)
            {
                newTexHeight = texHeight + incTexHeight;
            }
            else
            {
                newTexHeight = config.maxTexHeight;
            }

            // 新创建一个纹理
            if (!createTexture(name, newTexWidth, newTexHeight, newTexture))
            {
                break;
            }

            // 把旧纹理的所有数据复制到新纹理相同区域
            Rect srcRect(Point(0, 0), Size(texWidth, texHeight));
            Rect dstRect = srcRect;
            if (!srcTexture->copyToTexture(newTexture, &srcRect, &dstRect))
            {
                T3D_LOG_ERROR("Copy source texture data to new texture failed !");
                break;
            }

            ret = true;
        } while (0);

        return ret;
    }
}
