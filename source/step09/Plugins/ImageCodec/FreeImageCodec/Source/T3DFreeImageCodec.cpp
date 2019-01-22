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
        TResult ret = T3D_ERR_OK;

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
        TResult ret = T3D_ERR_OK;
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
        TResult ret = T3D_ERR_OK;

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
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, 
                    "Convert from raw data failed !");
                break;
            }

            stream = FreeImage_OpenMemory(0, 0);

            FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)type;

            if (!FreeImage_SaveToMemory(fif, dib, stream))
            {
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, 
                    "Encode image data to target format failed !");
                break;
            }

            uint8_t *temp = nullptr;

            if (!FreeImage_AcquireMemory(stream, &temp, (DWORD *)&size))
            {
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, 
                    "Retreive encoding data from memory failed !");
                break;
            }

            data = new uint8_t[size];
            memcpy(data, temp, size);
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

    TResult FreeImageCodec::decode(uint8_t *data, size_t size, Image &image, 
        FileType type)
    {
        TResult ret = T3D_ERR_OK;

        return ret;
    }
}

