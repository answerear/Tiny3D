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

#include "Misc/T3DImage.h"
#include "Misc/T3DColor4.h"
#include "ImageCodec/T3DImageCodec.h"
#include "Resource/T3DArchiveManager.h"
#include "Misc/T3DString.h"
#include <FreeImage.h>


namespace Tiny3D
{
    const char * const Image::FILETYPE_BMP = "bmp";
    const char * const Image::FILETYPE_PNG = "png";
    const char * const Image::FILETYPE_TGA = "tga";
    const char * const Image::FILETYPE_JPG = "jpg";
    const char * const Image::FILETYPE_DDS = "dds";

    int32_t Image::calcPitch(int32_t width, int32_t bpp)
    {
        return (width * (bpp / 8) + 3) & ~3;
    }

    int32_t Image::getBPP(PixelFormat format)
    {
        int32_t bpp = 0;

        switch (format)
        {
        case E_PF_PALETTE8:
            bpp = 8;
            break;
        case E_PF_R5G6B5:
        case E_PF_A1R5G5B5:
        case E_PF_A4R4G4B4:
            bpp = 16;
            break;
        case E_PF_R8G8B8:
        case E_PF_B8G8R8:
            bpp = 24;
            break;
        case E_PF_A8R8G8B8:
        case E_PF_B8G8R8A8:
        case E_PF_X8R8G8B8:
        case E_PF_B8G8R8X8:
            bpp = 32;
            break;
        }

        return bpp;
    }

    Image::Image()
        : mWidth(0)
        , mHeight(0)
        , mBPP(0)
        , mPitch(0)
        , mFormat(E_PF_A8R8G8B8)
        , mHasAlpha(false)
        , mIsPreMulti(false)
        , mIsDataExternal(false)
        , mIsEmpty(true)
        , mData(nullptr)
        , mDataSize(0)
    {

    }

    Image::Image(int32_t width, int32_t height, int32_t bpp, PixelFormat format)
        : mWidth(width)
        , mHeight(height)
        , mBPP(bpp)
        , mPitch(0)
        , mFormat(format)
        , mHasAlpha(false)
        , mIsPreMulti(false)
        , mIsDataExternal(false)
        , mIsEmpty(false)
        , mData(nullptr)
        , mDataSize(0)
    {
        mPitch = calcPitch();
        mDataSize = mHeight * mPitch;
        mData = new uint8_t[mDataSize];
        memset(mData, 0, mDataSize);
    }

    Image::~Image()
    {
        if (!mIsDataExternal)
        {
            T3D_SAFE_DELETE_ARRAY(mData);
        }
    }

    bool Image::destroy()
    {
        if (!mIsEmpty)
        {
            if (!mIsDataExternal)
            {
                T3D_SAFE_DELETE_ARRAY(mData);
            }

            mIsEmpty = true;
        }

        mWidth = mHeight = mBPP = mPitch = 0;

        return true;
    }

    bool Image::load(const String &path)
    {
        destroy();

        bool ret = T3D_IMAGE_CODEC.decode(path, *this);

        if (ret)
        {
            mIsEmpty = false;
            mIsDataExternal = false;
        }

        return ret;
    }

    bool Image::load(DataStream &stream)
    {
        destroy();

        bool ret = T3D_IMAGE_CODEC.decode(stream, *this);

        if (ret)
        {
            mIsEmpty = false;
            mIsDataExternal = false;
        }

        return ret;
    }

    bool Image::load(uint8_t *data, int32_t width, int32_t height, int32_t bpp, int32_t pitch, 
        PixelFormat format, bool copySource /* = false */)
    {
        destroy();

        bool ret = false;

        do 
        {
            mWidth = width;
            mHeight = height;
            mBPP = bpp;
            mPitch = calcPitch();

            if (pitch != mPitch)
                break;

            mFormat = format;
            mDataSize = mPitch * mHeight;

            if (copySource)
            {
                mData = new uint8_t[mDataSize];
                memcpy(mData, data, mDataSize);
                mIsDataExternal = false;
            }
            else
            {
                mData = data;
                mIsDataExternal = true;
            }

            mIsEmpty = false;
            ret = true;
        } while (0);

        return ret;
    }

    bool Image::save(const String &path, const String &fileType /* = FILETYPE_PNG */) const
    {
        ImageCodecBase::FileType eType = (ImageCodecBase::FileType)getFileType(fileType);
        return T3D_IMAGE_CODEC.encode(path, *this, eType);
    }

    bool Image::save(DataStream &stream, const String &fileType /* = FILETYPE_PNG */) const
    {
        ImageCodecBase::FileType eType = (ImageCodecBase::FileType)getFileType(fileType);
        return T3D_IMAGE_CODEC.encode(stream, *this, eType);
    }

    bool Image::flip()
    {
        bool ret = false;

        FIBITMAP *dib = nullptr;

        do 
        {
            uint32_t redMask, greenMask, blueMask, alphaMask;
            getColorMask(redMask, greenMask, blueMask, alphaMask);

            dib = FreeImage_ConvertFromRawBitsEx(FALSE, mData, FIT_BITMAP, mWidth, mHeight, mPitch, mBPP, redMask, greenMask, blueMask, FALSE);
            
            if (dib == nullptr)
            {
                T3D_LOG_ERROR("Convert from raw bits failed !");
                break;
            }

            if (!FreeImage_FlipVertical(dib))
            {
                T3D_LOG_ERROR("Flip image failed !");
                break;
            }

            FreeImage_Unload(dib);
            dib = nullptr;

            ret = true;
        } while (0);

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        return ret;
    }

    bool Image::mirror()
    {
        bool ret = false;

        FIBITMAP *dib = nullptr;

        do
        {
            uint32_t redMask, greenMask, blueMask, alphaMask;
            getColorMask(redMask, greenMask, blueMask, alphaMask);

            dib = FreeImage_ConvertFromRawBitsEx(FALSE, mData, FIT_BITMAP, mWidth, mHeight, mPitch, mBPP, redMask, greenMask, blueMask, FALSE);

            if (dib == nullptr)
            {
                T3D_LOG_ERROR("Convert from raw bits failed !");
                break;
            }

            if (!FreeImage_FlipHorizontal(dib))
            {
                T3D_LOG_ERROR("Mirror image failed !");
                break;
            }

            FreeImage_Unload(dib);
            dib = nullptr;

            ret = true;
        } while (0);

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        return ret;
    }

    bool Image::fill(const Color4 &rkColor)
    {
        bool ret = false;

        FIBITMAP *dib = nullptr;

        do
        {
            uint32_t redMask, greenMask, blueMask, alphaMask;
            getColorMask(redMask, greenMask, blueMask, alphaMask);

            dib = FreeImage_ConvertFromRawBitsEx(FALSE, mData, FIT_BITMAP, mWidth, mHeight, mPitch, mBPP, redMask, greenMask, blueMask, FALSE);

            if (dib == nullptr)
            {
                T3D_LOG_ERROR("Convert from raw bits failed !");
                break;
            }

            uint32_t color = rkColor.A8R8G8B8();
            if (!FreeImage_FillBackground(dib, (const void *)&color))
            {
                T3D_LOG_ERROR("Fill image failed !");
                break;
            }

            FreeImage_Unload(dib);
            dib = nullptr;

            ret = true;
        } while (0);

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        return ret;
    }

    bool Image::copy(const Image &image, Rect *srcRect /* = nullptr */, Rect *dstRect /* = nullptr */, Filter filter /* = E_FILTER_BILINEAR */)
    {
        bool ret = false;

        FIBITMAP *dib = nullptr;
        FIBITMAP *newdib = nullptr;

        do
        {
            if (image.isEmpty())
            {
                // ��ͼ�񣬸���ͼ�񴴽����Ҹ�������
                mData = new uint8_t[image.mDataSize];
                mDataSize = image.mDataSize;
                memcpy(mData, image.mData, image.mDataSize);
                mWidth = image.mWidth;
                mHeight = image.mHeight;
                mBPP = image.mBPP;
                mFormat = image.mFormat;
                mPitch = image.mPitch;
                mHasAlpha = image.mHasAlpha;
                mIsPreMulti = image.mIsPreMulti;
                mIsDataExternal = false;
                mIsEmpty = false;
            }
            else
            {
                // �ǿ�ͼ������е��鷳�ˣ�ֻ��ӲͷƤ��

                // ������ظ�ʽ��һ�£�ֱ�ӷ��أ����ȵ���convert�ӿ���ת����һ�¸�ʽ��ͼ��
                if (image.getFormat() != mFormat)
                {
                    T3D_LOG_ERROR("Source image and destination image are different !");
                    break;
                }

                Rect rtDst;
                if (dstRect == nullptr)
                {
                    rtDst = Rect(Point(0, 0), Size(mWidth, mHeight));
                }
                else
                {
                    rtDst = *dstRect;
                }

                Rect rtSrc;
                if (srcRect == nullptr)
                {
                    rtSrc = Rect(Point(0, 0), Size(image.getWidth(), image.getHeight()));
                }
                else
                {
                    rtSrc = *srcRect;
                }

                // ����������Ч��
                if (rtSrc.left < 0 || rtSrc.right >= mWidth 
                    || rtSrc.top < 0 || rtSrc.bottom >= mHeight)
                {
                    // ����Դͼ��Χ��
                    T3D_LOG_ERROR("Source image rectangle is out of bound !");
                    break;
                }

                if (rtDst.left < 0 || rtDst.right >= image.getWidth()
                    || rtDst.top < 0 || rtDst.bottom >= image.getHeight())
                {
                    // ����Ŀ��ͼ��Χ��
                    T3D_LOG_ERROR("Destination image rectangle is out of bound !");
                    break;
                }

                if (rtDst == rtSrc)
                {
                    // Դ��Ŀ��������ȫ��ͬ��ֱ�ӵ��
                    T3D_ASSERT(image.mDataSize == mDataSize);
                    memcpy(mData, image.mData, mDataSize);
                }
                else if(rtSrc.width() == rtDst.width() && rtSrc.height() == rtDst.height())
                {
                    // Դ��Ŀ�������Сһ�£����������Լ�������
                    uint8_t *srcData = image.mData + rtSrc.top * image.mPitch + rtSrc.left * image.getBytesPerPixel();
                    uint8_t *dstData = mData + rtDst.top * mPitch + rtDst.left * getBytesPerPixel();
                    uint8_t *src = srcData;
                    uint8_t *dst = dstData;

                    int32_t pitch = rtSrc.width() * getBytesPerPixel();

                    int32_t y = 0;
                    for (y = 0; y < rtSrc.height(); ++y)
                    {
                        memcpy(dst, src, pitch);
                        dst += image.mPitch;
                        src += mPitch;
                    }
                }
                else
                {
                    // û�취���Լ�ͼ�����㷨�ã�Ҳ����ȥ�о�������FreeImage��æ��
                    uint32_t redMask, greenMask, blueMask, alphaMask;
                    image.getColorMask(redMask, greenMask, blueMask, alphaMask);

                    // ��Դ���ݶ���FreeImage���棬��FreeImage������
                    dib = FreeImage_ConvertFromRawBitsEx(FALSE, mData, FIT_BITMAP, 
                        image.getWidth(), image.getWidth(), image.getPitch(), image.getBPP(), 
                        redMask, greenMask, blueMask, FALSE);

                    if (dib == nullptr)
                    {
                        T3D_LOG_ERROR("Convert from raw bits failed !");
                        break;
                    }

                    // ����
                    newdib = FreeImage_Rescale(dib, rtDst.width(), rtDst.height(), (FREE_IMAGE_FILTER)filter);
                    if (newdib == nullptr)
                    {
                        T3D_LOG_ERROR("Scale image failed !");
                        break;
                    }

                    // ��ת�õ����ݸ��Ƶ�Ŀ��ͼ����
                    int32_t srcBPP = FreeImage_GetBPP(newdib);
                    T3D_ASSERT(srcBPP == image.getBPP());

                    int32_t srcPitch = FreeImage_GetPitch(newdib);
                    uint8_t *srcData = FreeImage_GetBits(newdib);

                    int32_t dstPitch = mPitch;
                    uint8_t *dstData = mData + rtDst.top * dstPitch + rtDst.left * getBytesPerPixel();

                    uint8_t *src = srcData;
                    uint8_t *dst = dstData;
                    int32_t y = 0;

                    for (y = 0; y < rtDst.height(); ++y)
                    {
                        memcpy(dst, src, srcPitch);
                        dst += dstPitch;
                        src += srcPitch;
                    }

                    // �ͷ�FreeImage����
                    FreeImage_Unload(newdib);
                    newdib = nullptr;

                    FreeImage_Unload(dib);
                    dib = nullptr;
                }
            }

            ret = true;
        } while (0);

        if (newdib != nullptr)
        {
            FreeImage_Unload(newdib);
            newdib = nullptr;
        }

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        return ret;
    }

    bool Image::convert(PixelFormat format)
    {
        bool ret = false;

        do 
        {
            if (!isEmpty())
            {

            }
        } while (0);

        return ret;
    }

    bool Image::convert(Image &image, PixelFormat format) const
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    bool Image::compare(const Image &other, bool compareAlpha /* = true */) const
    {
        return false;
    }

    uint32_t Image::getFileType(const String &fileExt) const
    {
        ImageCodecBase::FileType fileType = ImageCodecBase::E_FT_UNKNOWN;
        String ext = fileExt;
        StringUtil::toLowerCase(ext);

        if (ext == "png")
        {
            fileType = ImageCodecBase::E_FT_PNG;
        }
        else if (ext == "bmp")
        {
            fileType = ImageCodecBase::E_FT_BMP;
        }
        else if (ext == "jpg" || ext == "jpeg")
        {
            fileType = ImageCodecBase::E_FT_JPEG;
        }
        else if (ext == "dds")
        {
            fileType = ImageCodecBase::E_FT_DDS;
        }
        else if (ext == "tga")
        {
            fileType = ImageCodecBase::E_FT_TARGA;
        }

        return fileType;
    }

    int32_t Image::calcPitch() const
    {
        return Image::calcPitch(mWidth, mBPP);//(mWidth * (mBPP / 8) + 3) & ~3;
    }

    void Image::getColorMask(uint32_t &redMask, uint32_t &greenMask, uint32_t &blueMask, uint32_t &alphaMask) const
    {
        switch (mFormat)
        {
        case E_PF_A1R5G5B5:
            {
                redMask = Color4::RGB555_RED_MASK;
                greenMask = Color4::RGB555_GREEN_MASK;
                blueMask = Color4::RGB555_BLUE_MASK;
                alphaMask = Color4::RGB555_ALPHA_MASK;
            }
            break;
        case E_PF_R5G6B5:
            {
                redMask = Color4::RGB565_RED_MASK;
                greenMask = Color4::RGB565_GREEN_MASK;
                blueMask = Color4::RGB565_BLUE_MASK;
                alphaMask = 0;
            }
            break;
        case E_PF_A4R4G4B4:
            {
                redMask = Color4::RGB444_RED_MASK;
                greenMask = Color4::RGB444_GREEN_MASK;
                blueMask = Color4::RGB444_BLUE_MASK;
                alphaMask = Color4::RGB444_ALPHA_MASK;
            }
            break;
        case E_PF_R8G8B8:
            break;
        case E_PF_B8G8R8:
            break;
        case E_PF_A8R8G8B8:
            break;
        case E_PF_B8G8R8A8:
            break;
        case E_PF_X8R8G8B8:
            break;
        case E_PF_B8G8R8X8:
            break;
        default:
            break;
        }
    }

    int32_t Image::getBytesPerPixel() const
    {
        return (mBPP >> 3);
    }
}
