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

#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwarePixelBuffer.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
    TexturePtr Texture::create(const String &name, int32_t numMipMaps, int32_t texWidth /* = -1 */, 
        int32_t texHeight /* = -1 */, TexUsage texUsage /* = E_TU_DEFAULT */, 
        TexType texType /* = E_TEX_TYPE_2D */, PixelFormat format /* = E_PF_A8R8G8B8 */)
    {
        TexturePtr texture = new Texture(name, numMipMaps, texWidth, texHeight, texUsage, texType, format);
        if (texture != nullptr)
        {
            texture->release();
        }
        return texture;
    }

    Texture::Texture(const String &name, int32_t numMipMaps, 
        int32_t texWidth /* = -1 */, int32_t texHeight /* = -1 */, 
        TexUsage texUsage /* = E_TU_DEFAULT */, TexType texType /* = E_TEX_TYPE_1D */, 
        PixelFormat format /* = E_PF_A8R8G8B8 */)
        : Resource(name)
        , mTexType(texType)
        , mTexUsage(texUsage)
        , mNumMipMaps(numMipMaps)
        , mTexWidth(texWidth)
        , mTexHeight(texHeight)
        , mImgWidth(texWidth)
        , mImgHeight(texHeight)
        , mFormat(format)
        , mHasAlpha(false)
    {

    }

    Texture::~Texture()
    {
        mPixelBuffer = nullptr;
    }

    Resource::Type Texture::getType() const
    {
        return E_TYPE_TEXTURE;
    }

    bool Texture::load()
    {
        bool ret = false;

        do 
        {
            if (E_TU_DEFAULT == mTexUsage)
            {
                ArchivePtr archive;
                MemoryDataStream stream;

                // 加载纹理数据
                if (!ArchiveManager::getInstance().getArchive(mName, archive))
                {
                    T3D_LOG_ERROR("Get archive named %s failed !", mName.c_str());
                    break;
                }

                if (!archive->read(mName, stream))
                {
                    T3D_LOG_ERROR("Read data from stream failed !");
                    break;
                }

                Image image;
                if (!image.load(stream))
                {
                    T3D_LOG_ERROR("Load image failed !");
                    break;
                }

                if (mTexWidth == -1)
                {
                    mTexWidth = image.getWidth();
                }

                if (mTexHeight == -1)
                {
                    mTexHeight = image.getHeight();
                }

                // 创建硬件缓冲区
                mPixelBuffer = HardwareBufferManager::getInstance().createPixelBuffer(mTexWidth, mTexHeight, mFormat, HardwareBuffer::E_HBU_DYNAMIC, false);

                if (mPixelBuffer == nullptr)
                {
                    T3D_LOG_ERROR("Create pixel buffer failed !");
                    break;
                }

                // 复制纹理数据到硬件缓冲区
                if (!mPixelBuffer->readImage(image))
                {
                    T3D_LOG_ERROR("Read image data failed !");
                    break;
                }
            }
            else
            {
                // 创建硬件缓冲区
                mPixelBuffer = HardwareBufferManager::getInstance().createPixelBuffer(mTexWidth, mTexHeight, mFormat, HardwareBuffer::E_HBU_DYNAMIC, false);

                if (mPixelBuffer == nullptr)
                {
                    T3D_LOG_ERROR("Create pixel buffer failed !");
                    break;
                }
            }

            ret = true;
        } while (0);

        return ret;
    }

    void Texture::unload()
    {
        mPixelBuffer = nullptr;
    }

    ResourcePtr Texture::clone() const
    {
        TexturePtr newTexture = Texture::create(mName, mNumMipMaps, mTexWidth, mTexHeight, mTexUsage, mTexType, mFormat);

        if (newTexture != nullptr)
        {
            newTexture->load();
        }

        return newTexture;
    }

    bool Texture::copyTo(TexturePtr texture, Rect *dstRect /* = nullptr */, Rect *srcRect /* = nullptr */)
    {
        if (!texture->isLoaded())
        {
            texture->load();
        }

        return mPixelBuffer->copyTo(texture->getPixelBuffer(), dstRect, srcRect);
    }

    bool Texture::saveToFile(const String &path, const String &fileType)
    {
        bool ret = false;

        do 
        {
            if (mPixelBuffer == nullptr)
            {
                T3D_LOG_ERROR("Empty texture data !");
                break;
            }

            Image image;
            if (!mPixelBuffer->writeImage(image))
            {
                break;
            }

            if (!image.save(path, fileType))
            {
                break;
            }

            ret = true;
        } while (0);

        return ret;
    }
}
