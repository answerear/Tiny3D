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

#include "ImageCodec/T3DImageCodecIMG.h"
#include <FreeImage.h>
#include <sstream>
#include "Misc/T3DString.h"
#include "Misc/T3DColor4.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
    const size_t ImageCodecIMG::MAX_SUPPORTED_FILE_TYPE = 37;

    void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
    {
        if (fif != FIF_UNKNOWN)
        {
            T3D_LOG_ERROR("%s Format", FreeImage_GetFormatFromFIF(fif));
        }

        T3D_LOG_ERROR(message);
    }

    ImageCodecIMGPtr ImageCodecIMG::create()
    {
        ImageCodecIMGPtr codec = new ImageCodecIMG();
        codec->release();
        return codec;
    }

    ImageCodecIMG::ImageCodecIMG()
    {

    }

    ImageCodecIMG::~ImageCodecIMG()
    {

    }

    bool ImageCodecIMG::startup()
    {
        FreeImage_Initialise(FALSE);

        T3D_LOG_INFO("FreeImage Version : %s", FreeImage_GetVersion());
        T3D_LOG_INFO("%s", FreeImage_GetCopyrightMessage());

        std::stringstream ss;
        ss << "Supported formats : ";
        bool first = true;
        int i = 0;
        for (i = 0; i < FreeImage_GetFIFCount(); ++i)
        {
            String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
            if (!first)
            {
                ss << ",";
            }
            first = false;
            ss << exts;

            StringVector extensions = StringUtil::split(exts, ",");
        }

        T3D_LOG_INFO("%s", ss.str().c_str());

        FreeImage_SetOutputMessage(FreeImageErrorHandler);

        mFileTypeList.resize(MAX_SUPPORTED_FILE_TYPE);
        for (i = 0; i < mFileTypeList.size(); ++i)
        {
            mFileTypeList[i] = (ImageCodecBase::FileType)i;
        }

        return true;
    }

    bool ImageCodecIMG::shutdown()
    {
        FreeImage_DeInitialise();

        return true;
    }

    bool ImageCodecIMG::isSupportedType(uint8_t *data, size_t size, FileType &eFileType) const
    {
        bool ret = false;

        FIMEMORY* fiMem = FreeImage_OpenMemory(data, size);

        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMem, size);
        FreeImage_CloseMemory(fiMem);

        eFileType = (FileType)fif;

        if (fif != FIF_UNKNOWN)
        {
            ret = true;
        }

        return ret;
    }

    ImageCodecBase::FileType ImageCodecIMG::getFileType() const
    {
        return E_FT_IMG;
    }

    bool ImageCodecIMG::encode(uint8_t *&data, size_t &size, const Image &image, FileType eFileType)
    {
        bool ret = false;

        FIBITMAP *dib = nullptr;
        FIMEMORY *stream = nullptr;

        do 
        {
            uint32_t redMask, greenMask, blueMask, alphaMask;
            image.getColorMask(redMask, greenMask, blueMask, alphaMask);

            dib = FreeImage_ConvertFromRawBitsEx(FALSE, (BYTE*)image.getData(), FIT_BITMAP,
                image.getWidth(), image.getHeight(), image.getPitch(), image.getBPP(), 
                redMask, greenMask, blueMask, TRUE);

            stream = FreeImage_OpenMemory(0, 0);

            FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)eFileType;

            if (!FreeImage_SaveToMemory(fif, dib, stream))
            {
                T3D_LOG_ERROR("Encode image data to target format failed !");
                break;
            }

            uint8_t *temp = nullptr;
            if (!FreeImage_AcquireMemory(stream, &temp, (DWORD *)&size))
            {
                T3D_LOG_ERROR("Retreive encoding data from memory failed !");
                break;
            }

            data = new uint8_t[size];
            memcpy(data, temp, size);

            ret = true;
        } while (0);


        if (stream != nullptr)
        {
            FreeImage_CloseMemory(stream);
            stream = nullptr;
        }

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        return ret;
    }

    bool ImageCodecIMG::decode(uint8_t *data, size_t size, Image &image, FileType eFileType)
    {
        bool ret = false;
        FIMEMORY *stream = FreeImage_OpenMemory(data, size);

        FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)eFileType;
        FIBITMAP *dib = FreeImage_LoadFromMemory(fif, stream);

        if (dib == nullptr)
        {
            FreeImage_CloseMemory(stream);
            T3D_LOG_ERROR("Error decoding image !");
            return false;
        }

        uint32_t width = FreeImage_GetWidth(dib);
        uint32_t height = FreeImage_GetHeight(dib);
        int32_t bpp = FreeImage_GetBPP(dib);

        FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
        FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(dib);
        PixelFormat eFormat = E_PF_A8R8G8B8;

        switch (imageType)
        {
        case FIT_BITMAP:
            {
                if (colorType == FIC_MINISWHITE || colorType == FIC_MINISBLACK)
                {
                    FIBITMAP *newBitmap = FreeImage_ConvertToGreyscale(dib);
                    FreeImage_Unload(dib);
                    dib = newBitmap;
                    bpp = FreeImage_GetBPP(dib);
                }
                else if (bpp < 8 || colorType == FIC_PALETTE || colorType == FIC_CMYK)
                {
                    FIBITMAP *newBitmap = nullptr;
                    if (FreeImage_IsTransparent(dib))
                    {
                        // �Ѵ�͸��ͨ����ת��32λ
                        newBitmap = FreeImage_ConvertTo32Bits(dib);
                    }
                    else
                    {
                        // ����͸��ͨ����ֱ��ת��24λ
                        newBitmap = FreeImage_ConvertTo24Bits(dib);
                    }

                    FreeImage_Unload(dib);
                    dib = newBitmap;
                    bpp = FreeImage_GetBPP(dib);
                }

                ret = true;
            }
            break;
        default:
            break;
        }

        if (ret)
        {
            bool hasAlpha = false;
            bool isPreMulti = false;

            int32_t srcPitch = FreeImage_GetPitch(dib);
            uint8_t *src = FreeImage_GetBits(dib);

            int32_t dstPitch = srcPitch;
            size_t imageSize = dstPitch * height;
            uint8_t *dst = new uint8_t[imageSize];

            switch (bpp)
            {
            case 8:
                {
                    eFormat = E_PF_PALETTE8;
                }
                break;
            case 16:
                {
                    if (FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK)
                    {
                        eFormat = E_PF_R5G6B5;
                    }
                    else
                    {
                        eFormat = E_PF_A1R5G5B5;
                    }
                }
                break;
            case 24:
                {
                    eFormat = E_PF_R8G8B8;
                }
                break;
            case 32:
                {
                    eFormat = E_PF_A8R8G8B8;
                    hasAlpha = true;
                }
                break;
            }

            int32_t y = 0;
            uint8_t *pDst = dst;

            if (eFileType == E_FT_DDS)
            {
                // DDSҪ��תһ�Σ���ΪDDSԭ�������Ͻ�
                for (y = 0; y < height; ++y)
                {
                    uint8_t *pSrc = src + (height - y - 1) * srcPitch;
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                    Color4::convert_B8G8R8A8toA8R8G8B8(pSrc, pDst, width);
#elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
                    memcpy(pDst, pSrc, dstPitch);
#endif
                    pDst += dstPitch;
                }
            }
            else
            {
                for (y = 0; y < height; ++y)
                {
                    uint8_t *pSrc = src + y * srcPitch;
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                    Color4::convert_B8G8R8A8toA8R8G8B8(pSrc, pDst, width);
#elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
                    memcpy(pDst, pSrc, dstPitch);
#endif
                    pDst += dstPitch;
                }
            }

            setImageDimension(image, width, height, dstPitch);
            setImageInfo(image, bpp, hasAlpha, isPreMulti, eFormat);
            setImageData(image, dst, imageSize);
        }

        FreeImage_Unload(dib);
        FreeImage_CloseMemory(stream);

        return ret;
    }
}

