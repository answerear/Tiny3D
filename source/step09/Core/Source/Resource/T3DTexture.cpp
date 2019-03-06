﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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
 ******************************************************************************/


#include "Resource/T3DTexture.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "ImageCodec/T3DImage.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TexturePtr Texture::create(const String &name, size_t mipmaps,
        size_t texWidth /* = 0 */, size_t texHeight /* = 0 */,
        TexUsage texUsage /* = E_TU_DEFAULT */,
        TexType texType /* = E_TEX_TYPE_2D */,
        PixelFormat format /* = E_PF_A8R8G8B8 */)
    {
        TexturePtr texture = new Texture(name, mipmaps, texWidth, texHeight, 
            texUsage, texType, format);
        texture->release();
        return texture;
    }

    //--------------------------------------------------------------------------

    Texture::Texture(const String &name, size_t mipmaps, size_t texWidth, 
        size_t texHeight, TexUsage texUsage, TexType texType, 
        PixelFormat format)
        : Resource(name)
        , mTexType(texType)
        , mTexUsage(texUsage)
        , mMipmaps(mipmaps)
        , mTexWidth(texWidth)
        , mTexHeight(texHeight)
        , mImgWidth(texWidth)
        , mImgHeight(texHeight)
        , mFormat(format)
        , mHasAlpha(false)
        , mPBO(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    Texture::~Texture()
    {
        mPBO = nullptr;
    }

    //--------------------------------------------------------------------------

    Resource::Type Texture::getType() const
    {
        return E_RT_TEXTURE;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Texture::clone() const
    {
        TexturePtr texture = Texture::create(mName, mMipmaps, mTexWidth,
            mTexHeight, mTexUsage, mTexType, mFormat);

        if (texture != nullptr)
        {
            TResult ret = texture->load();
            if (ret != T3D_OK)
            {
                texture = nullptr;
            }
        }

        return texture;
    }

    //--------------------------------------------------------------------------

    TResult Texture::load()
    {
        TResult ret = T3D_OK;

        do 
        {
            if (E_TU_DEFAULT == mTexUsage)
            {
                ArchivePtr archive;
                MemoryDataStream stream;


                // 加载纹理数据
                

                Image image;
                ret = image.load(mName);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load image %s failed !",
                        mName.c_str());
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
                mPBO = T3D_HARDWARE_BUFFER_MGR.createPixelBuffer(mTexWidth, 
                    mTexHeight, mFormat, HardwareBuffer::E_HBU_DYNAMIC, 
                    false, false);

                if (mPBO == nullptr)
                {
                    ret = T3D_ERR_INVALID_POINTER;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Create pixel buffer \
                        failed !");
                    break;
                }

                // 复制纹理数据到硬件缓冲区
                ret = mPBO->readImage(image);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Read image data failed !");
                    break;
                }
            }
            else
            {
                // 创建硬件缓冲区
                mPBO = T3D_HARDWARE_BUFFER_MGR.createPixelBuffer(mTexWidth, 
                    mTexHeight, mFormat, HardwareBuffer::E_HBU_DYNAMIC, 
                    false, false);

                if (mPBO == nullptr)
                {
                    ret = T3D_ERR_INVALID_POINTER;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Create pixel buffer \
                        failed !");
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture::unload()
    {
        mPBO = nullptr;
        return Resource::unload();
    }

    //--------------------------------------------------------------------------

    TResult Texture::saveToFile(const String &path, uint32_t fileType) const
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mPBO == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Not initialize !");
                break;
            }

            Image image;
            ret = mPBO->writeImage(image);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Write to image failed !");
                break;
            }

            ret = image.save(path, fileType);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save to file failed !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture::copyTo(TexturePtr texture, Rect *dstRect /* = nullptr */,
        Rect *srcRect /* = nullptr */)
    {
        if (!texture->isLoaded())
        {
            texture->load();
        }

        return mPBO->copyTo(texture->getPixelBuffer(), dstRect, srcRect);
    }
}

