

#include "Resource/T3DFreeTypeAtlas.h"
#include "Resource/T3DFontManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(FreeTypeAtlas);

    FreeTypeAtlas::FreeTypeAtlas()
    {

    }

    FreeTypeAtlas::~FreeTypeAtlas()
    {

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
            // 查找一个适合的block给存放这种字号的文本
            BlockPtr block;
            if (!lookupBlock(font, block))
            {
                // 没有找到一个合适的区块，只能插入一个新的区块
                if (!insertBlock(font, block))
                {
                    T3D_LOG_ERROR("Insert block failed !");
                    break;
                }
            }

            // 先判断是否够空间放入新的字符
            FT_Face ftFace = font->getFontFace();
            Rect srcRect;
            srcRect.left = (ftFace->glyph->metrics.horiBearingX >> 6);
            srcRect.right = (srcRect.left + ftFace->glyph->metrics.width >> 6);
            srcRect.top = (ftFace->glyph->metrics.horiBearingY >> 6);
            srcRect.bottom = (srcRect.top + ftFace->glyph->metrics.height >> 6);

            Size srcSize;
            srcSize.width = ftFace->glyph->bitmap.width;
            srcSize.height = ftFace->glyph->bitmap.rows;

            block->charmap.insert(CharMapValue(code, ch));
            ch->mBlock = block;

            mCharmap.insert(CharMapValue(code, ch));
            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::lookupBlock(FontFreeTypePtr font, BlockPtr &block)
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
                FacePtr face = *i;

                // 查找相同的字体外观和相同的字体，看是否有对应的区块
                auto ii = face->blockmap.find(font->getFontSize());
                if (ii != face->blockmap.end())
                {
                    BlockList &blockList = ii->second;

                    // 这里肯定返回最后一个，因为只有最后一个才有可能是存在空闲区域的block
                    block = blockList.back();
                    found = true;
                }
            }
        }

        return found;
    }

    bool FreeTypeAtlas::insertBlock(FontFreeTypePtr font, BlockPtr &block)
    {
        bool ret = false;

        const FontConfig &config = FontManager::getInstance().getConfig();

        switch (config.strategy)
        {
        case FontConfig::E_STRATEGY_APPEND:
            {
                ret = insertBlockWithAppend(font, block);
            }
            break;
        case FontConfig::E_STRATEGY_AUTO:
        case FontConfig::E_STRATEGY_LRU:
            default:
            {
                ret = insertBlockWithLRU(font, block);
            }
            break;
        }

        return ret;
    }

    bool FreeTypeAtlas::insertBlockWithAppend(FontFreeTypePtr font, BlockPtr &block)
    {
        bool ret = false;

        do 
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            Point uv;
            Point offset;
            Size blockSize(config.blockWidth, config.blockHeight);

            if (!mFaces.empty())
            {
                // 先加载一个新的材质和纹理
                const String &fontName = font->getName();
                size_t faceIdx = 0;
                std::stringstream ss;
                ss << fontName << "_#" << faceIdx;
                String name = ss.str();

                MaterialPtr material;
                if (!loadMaterial(name, font, material))
                {
                    break;
                }

                // 插入一个新face
                FacePtr face;
                if (!createFace(material, fontName, face))
                {
                    break;
                }

                // 插入一个新block
                BlockPtr block;
                if (!createBlock(font->getFontSize(), Rect(uv, blockSize), offset, face, block))
                {
                    break;
                }
            }
            else
            {
                
            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::insertBlockWithLRU(FontFreeTypePtr font, BlockPtr &block)
    {
        bool ret = false;

        do 
        {
            const FontConfig &config = FontManager::getInstance().getConfig();

            Point uv;
            Point offset;
            Size blockSize(config.blockWidth, config.blockHeight);

            if (mFaces.empty())
            {
                // 先加载一个新的材质和纹理
                const String &fontName = font->getName();
                size_t faceIdx = 0;
                std::stringstream ss;
                ss << fontName << "_#" << faceIdx;
                String name = ss.str();

                MaterialPtr material;
                if (!loadMaterial(name, font, material))
                {
                    break;
                }

                // 插入一个新face
                FacePtr face;
                if (!createFace(material, fontName, face))
                {
                    break;
                }

                // 插入一个新block
                BlockPtr block;
                if (!createBlock(font->getFontSize(), Rect(uv, blockSize), offset, face, block))
                {
                    break;
                }
            }
            else
            {

            }

            ret = true;
        } while (0);

        return ret;
    }

    bool FreeTypeAtlas::loadMaterial(const String &name, FontFreeTypePtr font, MaterialPtr &material)
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
                material->setAmbientColor(Color4::WHITE);
                material->setDiffuseColor(Color4::WHITE);
                material->setSpecularColor(Color4::WHITE);
                material->setEmissiveColor(Color4::WHITE);

                texture = TextureManager::getInstance().loadTexture(name, config.initTexWidth, config.initTexHeight, 1, E_PF_A8R8G8B8, Texture::E_TU_BLANK);
                if (texture == nullptr)
                {
                    T3D_LOG_ERROR("Load font %s texture failed !", name.c_str());
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

            face = new Face();
            face->release();
            face->name = fontName;
            face->material = material;
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
            block = new Block();
            block->release();
            block->size = fontSize;
            block->area = area;
            block->offset = offset;
            block->face = face;

            auto r = face->blockmap.insert(BlockMapValue(fontSize, BlockList()));
            if (!r.second)
            {
                T3D_LOG_ERROR("Insert font block [%s_%u] failed !", face->name.c_str(), fontSize);
                break;
            }

            r.first->second.push_back(block);
        } while (0);

        return ret;
    }
}
