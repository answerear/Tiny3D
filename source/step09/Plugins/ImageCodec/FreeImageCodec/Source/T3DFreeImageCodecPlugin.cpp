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


#include "T3DFreeImageCodecPlugin.h"
#include "T3DFreeImageCodec.h"


namespace Tiny3D
{
    FreeImageCodecPlugin::FreeImageCodecPlugin()
        : mName("FreeImageCodec")
        , mImageCodec(nullptr)
    {

    }

    FreeImageCodecPlugin::~FreeImageCodecPlugin()
    {

    }

    const String &FreeImageCodecPlugin::getName() const
    {
        return mName;
    }

    TResult FreeImageCodecPlugin::install()
    {
        TResult ret = T3D_OK;
        
        mImageCodec = FreeImageCodec::create();
        
        const FreeImageCodec::FileTypeList &filetypes 
            = mImageCodec->getSupportFileTypeList();

        size_t i = 0;

        for (i = 0; i < filetypes.size(); ++i)
        {
            ImageCodecBase::FileType type = filetypes[i];
            ret = Engine::getInstance().addImageCodec(type, mImageCodec);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC, 
                    "Install plugin failed !");
                break;
            }
        }

        return ret;
    }

    TResult FreeImageCodecPlugin::startup()
    {
        TResult ret = T3D_OK;
        return ret;
    }

    TResult FreeImageCodecPlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult FreeImageCodecPlugin::uninstall()
    {
        TResult ret = T3D_OK;
        
        const FreeImageCodec::FileTypeList &filetypes
            = mImageCodec->getSupportFileTypeList();

        size_t i = 0;

        for (i = 0; i < filetypes.size(); ++i)
        {
            ImageCodecBase::FileType type = filetypes[i];
            ret = Engine::getInstance().removeImageCodec(type);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_FREEIMAGE_CODEC,
                    "uninstall plugin failed !");
                break;
            }
        }

        mImageCodec = nullptr;
        return ret;
    }
}

