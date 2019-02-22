/*******************************************************************************
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


#include "T3DFreeImageCodec.h"
#include "T3DFreeImageCodecError.h"
#include <FreeImage.h>
#include <sstream>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
    {
        if (fif != FIF_UNKNOWN)
        {
            T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, "Error [%s] : %s", 
                FreeImage_GetFormatFromFIF(fif), message);
        }
        else
        {
            T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, "Error [UNKNOWN] : %s",
                message);
        }
    }

    //--------------------------------------------------------------------------

    const size_t FreeImageCodec::MAX_SUPPORTED_FILE_TYPE = 37;

    //--------------------------------------------------------------------------

    FreeImageCodecPtr FreeImageCodec::create()
    {
        FreeImageCodecPtr codec = new FreeImageCodec();
        codec->release();
        return codec;
    }

    //--------------------------------------------------------------------------

    TResult FreeImageCodec::startup()
    {
        TResult ret = T3D_OK;

        do 
        {
            FreeImage_Initialise(FALSE);
            FreeImage_SetOutputMessage(FreeImageErrorHandler);

            T3D_LOG_INFO(LOG_TAG_FREEIMAGE_CODEC, "FreeImage Version : %s", 
                FreeImage_GetVersion());
            T3D_LOG_INFO(LOG_TAG_FREEIMAGE_CODEC, "%s", 
                FreeImage_GetCopyrightMessage());

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
            }

            T3D_LOG_INFO(LOG_TAG_FREEIMAGE_CODEC, "%s", ss.str().c_str());

            mFileTypeList.resize(MAX_SUPPORTED_FILE_TYPE);
            for (i = 0; i < mFileTypeList.size(); ++i)
            {
                mFileTypeList[i] = (ImageCodecBase::FileType)i;
            }
        } while (0);

        return ret;
    }

    TResult FreeImageCodec::shutdown()
    {
        TResult ret = T3D_OK;
        FreeImage_DeInitialise();
        return ret;
    }

    bool FreeImageCodec::isSupportedType(uint8_t *data, size_t size, 
        FileType &type) const
    {
        bool ret = true;

        do 
        {
            FIMEMORY* fiMem = FreeImage_OpenMemory(data, size);
            if (fiMem == nullptr)
            {
                ret = false;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, "Open memory failed !");
                break;
            }

            FREE_IMAGE_FORMAT fif 
                = FreeImage_GetFileTypeFromMemory(fiMem, size);
            FreeImage_CloseMemory(fiMem);

            type = (FileType)fif;
            if (fif == FIF_UNKNOWN)
            {
                ret = false;
                break;
            }

        } while (0);

        return ret;
    }

    ImageCodecBase::FileType FreeImageCodec::getFileType() const
    {
        return E_FT_IMG;
    }

    TResult FreeImageCodec::encode(uint8_t *&data, size_t &size, 
        const Image &image, FileType type)
    {
        TResult ret = T3D_OK;

        FIBITMAP *dib = nullptr;
        FIMEMORY *stream = nullptr;

        do 
        {
            uint32_t redMask, greenMask, blueMask, alphaMask;
            image.getColorMask(redMask, greenMask, blueMask, alphaMask);

            dib = FreeImage_ConvertFromRawBitsEx(FALSE, (BYTE*)image.getData(), 
                FIT_BITMAP, image.getWidth(), image.getHeight(), 
                image.getPitch(), image.getBPP(), redMask, greenMask, blueMask, 
                TRUE);
            if (dib == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC,
                    "Convert from raw data failed !");
                break;
            }

            stream = FreeImage_OpenMemory(0, 0);
            if (stream == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, "Open memory failed !");
                break;
            }

            FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)type;

            if (!FreeImage_SaveToMemory(fif, dib, stream))
            {
                ret = T3D_ERR_CODEC_ENCODE_TO_MEMORY;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, 
                    "Encode image data to target format failed !");
                break;
            }

            uint8_t *temp = nullptr;

            if (!FreeImage_AcquireMemory(stream, &temp, (DWORD *)&size))
            {
                ret = T3D_ERR_CODEC_ACQUIRE_MEMORY;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, 
                    "Acquire memory for encoding data failed !");
                break;
            }

            data = new uint8_t[size];
            memcpy(data, temp, size);
        } while (0);

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        if (stream != nullptr)
        {
            FreeImage_CloseMemory(stream);
            stream = nullptr;
        }

        return ret;
    }

    TResult FreeImageCodec::decode(uint8_t *data, size_t size, Image &image, 
        FileType type)
    {
        TResult ret = T3D_OK;

        FIMEMORY *stream = nullptr;
        FIBITMAP *dib = nullptr;

        do 
        {
            stream = FreeImage_OpenMemory(data, size);
            if (stream == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, "Open memory failed !");
                break;
            }

            FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)type;
            dib = FreeImage_LoadFromMemory(fif, stream);
            if (dib == nullptr)
            {
                ret = T3D_ERR_CODEC_DECODE_FROM_MEMORY;
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC,
                    "Decode from memory failed !");
                break;
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
                    if (colorType == FIC_MINISWHITE 
                        || colorType == FIC_MINISBLACK)
                    {
                        FIBITMAP *newBitmap = FreeImage_ConvertToGreyscale(dib);
                        if (newBitmap == nullptr)
                        {
                            ret = T3D_ERR_CODEC_CONVERT_TO_GREY;
                            T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC,
                                "Convert to grey scale failed !");
                            break;
                        }

                        FreeImage_Unload(dib);
                        dib = newBitmap;
                        bpp = FreeImage_GetBPP(dib);
                    }
                    else if (bpp < 8 
                        || colorType == FIC_PALETTE || colorType == FIC_CMYK)
                    {
                        FIBITMAP *newBitmap = nullptr;
                        if (FreeImage_IsTransparent(dib))
                        {
                            // 把带透明通道，转成32位
                            newBitmap = FreeImage_ConvertTo32Bits(dib);
                            if (newBitmap == nullptr)
                            {
                                ret = T3D_ERR_CODEC_CONVERT_TO_32BIT;
                                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC,
                                    "Convert to 32 bits failed !");
                                break;
                            }
                        }
                        else
                        {
                            // 不带透明通道，直接转成24位
                            newBitmap = FreeImage_ConvertTo24Bits(dib);
                            if (newBitmap == nullptr)
                            {
                                ret = T3D_ERR_CODEC_CONVERT_TO_24BIT;
                                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC,
                                    "Convert to 24 bits failed !");
                                break;
                            }
                        }

                        FreeImage_Unload(dib);
                        dib = newBitmap;
                        bpp = FreeImage_GetBPP(dib);
                    }
                }
                break;
            default:
                break;
            }

            if (ret == T3D_OK)
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

                if (type == E_FT_DDS)
                {
                    // DDS要翻转一次，因为DDS原点在左上角
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
                        uint8_t *pSrc = src + (height - y - 1) * srcPitch;
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
        } while (0);

        if (dib != nullptr)
        {
            FreeImage_Unload(dib);
            dib = nullptr;
        }

        if (stream != nullptr)
        {
            FreeImage_CloseMemory(stream);
            stream = nullptr;
        }

        return ret;
    }
}

