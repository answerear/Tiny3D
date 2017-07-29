/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#include "Misc/T3DImage.h"
#include "Misc/T3DColor4.h"
#include "ImageCodec/T3DImageCodec.h"
#include "Resource/T3DArchiveManager.h"


namespace Tiny3D
{
    const char * const Image::FILETYPE_BMP = "bmp";
    const char * const Image::FILETYPE_PNG = "png";
    const char * const Image::FILETYPE_TGA = "tga";
    const char * const Image::FILETYPE_JPG = "jpg";
    const char * const Image::FILETYPE_DDS = "dds";

    Image::Image()
        : mWidth(0)
        , mHeight(0)
        , mBPP(0)
        , mPitch(0)
        , mFormat(E_PF_A8R8G8B8)
        , mHasAlpha(false)
        , mIsPreMulti(false)
        , mData(nullptr)
        , mDataSize(0)
    {

    }

    Image::Image(int32_t width, int32_t height)
    {
        
    }

    Image::~Image()
    {
        T3D_SAFE_DELETE_ARRAY(mData);
    }

    bool Image::load(const String &name)
    {
        return T3D_IMAGE_CODEC.decode(name, *this);
    }

    bool Image::save(const String &name, const String &fileType /* = FILETYPE_PNG */) const
    {
        ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG;
        
        return T3D_IMAGE_CODEC.encode(name, *this, eType);
    }

    bool Image::flip()
    {
        return true;
    }

    bool Image::mirror()
    {
        return true;
    }

    bool Image::fill(const Color4 &rkColor)
    {
        return true;
    }

    bool Image::copyToScaling(void *dstData, int32_t dstWidth, int32_t dstHeight, PixelFormat dstFormat, int32_t dstPitch, bool needFlip /* = false */) const
    {
        bool ret = false;

        if (dstFormat == mFormat && dstWidth == mWidth && dstHeight == mHeight)
        {
            if (needFlip)
            {
                uint8_t *dst = (uint8_t *)dstData;
                uint8_t *src = mData;
                int32_t bpp = (mFormat == E_PF_A8R8G8B8 ? 32 : 24) / 8;
                const uint32_t bwidth = dstWidth * bpp;
                const uint32_t rest = dstPitch - bwidth;
                dst = dst + dstPitch * (dstHeight - 1);
                for (int32_t y = 0; y < dstHeight; ++y)
                {
                    // copy scanline
                    memcpy(dst, src, bwidth);
                    // clear pitch
                    memset(dst+bwidth, 0, rest);
                    dst -= dstPitch;
                    src += mPitch;
                }
                ret = true;
            }
            else
            {
                if (dstPitch == mPitch)
                {
                    memcpy(dstData, mData, mDataSize);
                    ret = true;
                }
                else
                {
                    uint8_t *dst = (uint8_t *)dstData;
                    uint8_t *src = mData;
                    int32_t bpp = (mFormat == E_PF_A8R8G8B8 ? 32 : 24) / 8;
                    const uint32_t bwidth = dstWidth * bpp;
                    const uint32_t rest = dstPitch - bwidth;
                    for (int32_t y = 0; y < dstHeight; ++y)
                    {
                        // copy scanline
                        memcpy(dst, src, bwidth);
                        // clear pitch
                        memset(dst+bwidth, 0, rest);
                        dst += dstPitch;
                        src += mPitch;
                    }
                    ret = true;
                }
            }
        }
        else
        {
            int32_t dstBytesPerPixel = 3;
            if (dstFormat == E_PF_A8R8G8B8 || dstFormat == E_PF_B8G8R8A8 || dstFormat == E_PF_B8G8R8X8 || dstFormat == E_PF_X8R8G8B8)
                dstBytesPerPixel = 4;
            int32_t srcBytesPerPiexl = mBPP / 8;
            const float sourceXStep = (float)mWidth / (float)dstWidth;
            const float sourceYStep = (float)mHeight / (float)dstHeight;
            int32_t yval=0, syval=0;
            float sy = 0.0f;
            uint8_t *dst = (uint8_t *)dstData;
            if (needFlip)
            {
                yval = dstPitch * (dstHeight - 1);
                for (int32_t y = 0; y < dstHeight; ++y)
                {
                    float sx = 0.0f;
                    for (int32_t x = 0; x < dstWidth; ++x)
                    {
                        Color4::convert(mData+syval+int32_t(sx)*srcBytesPerPiexl, mFormat, dst+yval+x*dstBytesPerPixel, dstFormat);
                        sx += sourceXStep;
                    }
                    sy += sourceYStep;
                    syval = ((int32_t)sy) * mPitch;
                    yval -= dstPitch;
                }
            }
            else
            {
                for (int32_t y = 0; y < dstHeight; ++y)
                {
                    float sx = 0.0f;
                    for (int32_t x = 0; x < dstWidth; ++x)
                    {
                        Color4::convert(mData+syval+int32_t(sx)*srcBytesPerPiexl, mFormat, dst+yval+x*dstBytesPerPixel, dstFormat);
                        sx += sourceXStep;
                    }
                    sy += sourceYStep;
                    syval = ((int32_t)sy) * mPitch;
                    yval += dstPitch;
                }
            }

            ret = true;
        }

        return ret;
    }

    int32_t Image::compare(const Image &other, bool compareAlpha /* = true */) const
    {
        return 0;
    }

    void Image::copy(const Image &other)
    {

    }
}
