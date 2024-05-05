/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Resource/T3DImage.h"
#include "ImageCodec/T3DImageCodec.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    uint32_t Image::getBPP(PixelFormat format)
    {
        uint32_t bpp = 0;

        switch (format)
        {
        case PixelFormat::E_PF_PALETTE8:
            bpp = 8;
            break;
        case PixelFormat::E_PF_B5G6R5:
        case PixelFormat::E_PF_B5G5R5A1:
        case PixelFormat::E_PF_B4R4G4A4:
            bpp = 16;
            break;
        case PixelFormat::E_PF_R8G8B8:
        case PixelFormat::E_PF_B8G8R8:
            bpp = 24;
            break;
        case PixelFormat::E_PF_R8G8B8A8:
        case PixelFormat::E_PF_B8G8R8A8:
        case PixelFormat::E_PF_R8G8B8X8:
        case PixelFormat::E_PF_B8G8R8X8:
            bpp = 32;
            break;
        default:
            bpp = 0;
            break;
        }

        return bpp;
    }

    //--------------------------------------------------------------------------

    ImagePtr Image::create(const String &name)
    {
        return new Image(name);
    }
    
    //--------------------------------------------------------------------------

    ImagePtr Image::create(const String &name, uint32_t width, uint32_t height, PixelFormat format)
    {
        return new Image(name, width, height,format);
    }
    
    //--------------------------------------------------------------------------

    Image::Image(const String &name)
        : Resource(name)
    {

    }

    //--------------------------------------------------------------------------
    
    Image::Image(const String &name, uint32_t width, uint32_t height, PixelFormat format)
        : Resource(name)
        , mWidth(width)
        , mHeight(height)
        , mPixelFormat(format)
    {
        mBPP = getBPP(format);
        mPitch = calcPitch();
        mDataSize = mHeight * mPitch;
        mData = new uint8_t[mDataSize];
        memset(mData, 0, mDataSize);
    }

    //--------------------------------------------------------------------------
    
    Image::~Image()
    {
        T3D_SAFE_DELETE_ARRAY(mData);
    }

    //--------------------------------------------------------------------------

    Resource::Type Image::getType() const
    {
        return Type::kImage;
    }

    //--------------------------------------------------------------------------

    TResult Image::destroy()
    {
        if (!mIsEmpty)
        {
            T3D_SAFE_DELETE_ARRAY(mData);
            mIsEmpty = true;
        }

        mWidth = mHeight = mBPP = mPitch = 0;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    // TResult Image::load(const String &path)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         ret = destroy();
    //         if (T3D_FAILED(ret))
    //         {
    //             break;
    //         }
    //
    //         ret = T3D_IMAGE_CODEC.decode(path, *this);
    //         if (T3D_FAILED(ret))
    //         {
    //             break;
    //         }
    //
    //         mIsEmpty = false;
    //         mIsDataExternal = false;
    //     }while (0);
    //
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult Image::load(DataStream &stream)
    // {
    //     TResult ret = T3D_OK;
    //     
    //     do 
    //     {
    //         ret = destroy();
    //         if (T3D_FAILED(ret))
    //         {
    //             break;
    //         }
    //
    //         ret = T3D_IMAGE_CODEC.decode(stream, *this);
    //         if (T3D_FAILED(ret))
    //         {
    //             break;
    //         }
    //
    //         mIsEmpty = false;
    //         mIsDataExternal = false;
    //     } while (0);
    //
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult Image::load(uint8_t *data, size_t width, size_t height,
    //     size_t bpp, size_t pitch, PixelFormat format,
    //     bool copySource /* = false */)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do 
    //     {
    //         ret = destroy();
    //         if (T3D_FAILED(ret))
    //         {
    //             break;
    //         }
    //
    //         mWidth = width;
    //         mHeight = height;
    //         mBPP = bpp;
    //         mPitch = calcPitch();
    //
    //         if (pitch != mPitch)
    //             break;
    //
    //         mFormat = format;
    //         mDataSize = mPitch * mHeight;
    //         mSourceType = FILETYPE_RAW;
    //
    //         if (copySource)
    //         {
    //             mData = new uint8_t[mDataSize];
    //             memcpy(mData, data, mDataSize);
    //             mIsDataExternal = false;
    //         }
    //         else
    //         {
    //             mData = data;
    //             mIsDataExternal = true;
    //         }
    //
    //         mIsEmpty = false;
    //     } while (0);
    //
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult Image::save(const String &path, 
    //     uint32_t ft /* = FILETYPE_PNG */) const
    // {
    //     ImageCodecBase::FileType eType = (ImageCodecBase::FileType)ft;
    //     return T3D_IMAGE_CODEC.encode(path, *this, eType);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult Image::save(DataStream &stream, 
    //     uint32_t ft /* = FILETYPE_PNG */) const
    // {
    //     ImageCodecBase::FileType eType = (ImageCodecBase::FileType)ft;
    //     return T3D_IMAGE_CODEC.encode(stream, *this, eType);
    // }

    //--------------------------------------------------------------------------
    
    TResult Image::flip()
    {
        return T3D_IMAGE_CODEC.flip(*this);
    }

    //--------------------------------------------------------------------------
    
    TResult Image::mirror()
    {
        return T3D_IMAGE_CODEC.mirror(*this);
    }

    //--------------------------------------------------------------------------
    
    TResult Image::fill(const Color4 &rkColor)
    {
        return T3D_IMAGE_CODEC.fill(*this, rkColor);
    }

    //--------------------------------------------------------------------------
    
    TResult Image::copy(const Image &image, 
        const Rect *srcRect /* = nullptr */, 
        const Rect *dstRect /* = nullptr */, 
        Filter filter /* = E_FILTER_BILINEAR */)
    {
        TResult ret = T3D_OK;

        if (isEmpty())
        {
            // 空图像，给空图像创建并且复制数据
            mData = new uint8_t[image.mDataSize];
            mDataSize = image.mDataSize;
            memcpy(mData, image.mData, image.mDataSize);
            mWidth = image.mWidth;
            mHeight = image.mHeight;
            mBPP = image.mBPP;
            mPixelFormat = image.mPixelFormat;
            mPitch = image.mPitch;
            mHasAlpha = image.mHasAlpha;
            mIsPreMulti = image.mIsPreMulti;
            mIsEmpty = false;
        }
        else
        {
            ret = T3D_IMAGE_CODEC.copy(image, srcRect, *this, dstRect, (uint32_t)filter);
        }

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult Image::convert(PixelFormat format)
    {
        TResult ret = T3D_OK;

        do
        {
            if (!isEmpty())
            {

            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult Image::convert(Image &image, PixelFormat format) const
    {
        TResult ret = T3D_OK;

        do
        {
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    bool Image::compare(const Image &other, bool compareAlpha /* = true */) const
    {
        return false;
    }

    //--------------------------------------------------------------------------
    
    void Image::getColorMask(uint32_t &redMask, uint32_t &greenMask, uint32_t &blueMask, uint32_t &alphaMask) const
    {
        switch (mPixelFormat)
        {
        case PixelFormat::E_PF_B5G5R5A1:
            {
                redMask = Color4::RGB555_RED_MASK;
                greenMask = Color4::RGB555_GREEN_MASK;
                blueMask = Color4::RGB555_BLUE_MASK;
                alphaMask = Color4::RGB555_ALPHA_MASK;
            }
            break;
        case PixelFormat::E_PF_B5G6R5:
            {
                redMask = Color4::RGB565_RED_MASK;
                greenMask = Color4::RGB565_GREEN_MASK;
                blueMask = Color4::RGB565_BLUE_MASK;
                alphaMask = 0;
            }
            break;
        case PixelFormat::E_PF_B4R4G4A4:
            {
                redMask = Color4::RGB444_RED_MASK;
                greenMask = Color4::RGB444_GREEN_MASK;
                blueMask = Color4::RGB444_BLUE_MASK;
                alphaMask = Color4::RGB444_ALPHA_MASK;
            }
            break;
        case PixelFormat::E_PF_R8G8B8:
            break;
        case PixelFormat::E_PF_B8G8R8:
            break;
        case PixelFormat::E_PF_R8G8B8A8:
            break;
        case PixelFormat::E_PF_B8G8R8A8:
            break;
        case PixelFormat::E_PF_R8G8B8X8:
            break;
        case PixelFormat::E_PF_B8G8R8X8:
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    ResourcePtr Image::clone() const
    {
        ImagePtr image = Image::create(getName());
        image->cloneProperties(this);
        return image;
    }

    //--------------------------------------------------------------------------

    void Image::cloneProperties(const Resource *const src)
    {
        
    }

    //--------------------------------------------------------------------------

    TResult Image::onCreate()
    {
        return Resource::onCreate();
    }

    //--------------------------------------------------------------------------

    TResult Image::onLoad(Archive *archive)
    {
        return Resource::onLoad(archive);
    }

    //--------------------------------------------------------------------------

    TResult Image::onUnload()
    {
        return Resource::onUnload();
    }

    //--------------------------------------------------------------------------
}
