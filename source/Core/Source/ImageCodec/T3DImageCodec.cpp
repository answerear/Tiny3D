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

#include "T3DImageCodec.h"
#include "T3DImageCodecIMG.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(ImageCodec);
    
    const size_t ImageCodec::MAX_NUMBER_OF_CODEC = 1;

    ImageCodec::ImageCodec()
        : mCodecList(MAX_NUMBER_OF_CODEC)
    {
        size_t i = 0;
        mCodecList[i++] = ImageCodecIMG::create();
    }

    ImageCodec::~ImageCodec()
    {

    }

    bool ImageCodec::startup()
    {
        bool ret = true;

        for (auto i = mCodecList.begin(); i != mCodecList.end(); ++i)
        {
            ImageCodecBasePtr codec = *i;
            ret = ret && codec->startup();

            if (ret)
            {
                if (codec->getFileType() == ImageCodecBase::E_FT_IMG)
                {
                    ImageCodecIMGPtr codecIMG = smart_pointer_cast<ImageCodecIMG>(codec);
                    ImageCodecIMG::FileTypeList fileTypeList = codecIMG->getSupportedFileTypeList();

                    for (auto itr = fileTypeList.begin(); itr != fileTypeList.end(); ++itr)
                    {
                        mCodecMap.insert(ImageCodecMapValue(*itr, codec));
                    }
                }
                else
                {
                    mCodecMap.insert(ImageCodecMapValue(codec->getFileType(), codec));
                }
            }
        }

        return ret;
    }

    bool ImageCodec::shutdown()
    {
        bool ret = true;
        size_t i = 0;
        for (i = 0; i < mCodecList.size(); ++i)
        {
            ret = ret && mCodecList[i]->shutdown();
        }

        return ret;
    }

    bool ImageCodec::encode(const String &name, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_PNG */)
    {
        bool ret = false;

        return ret;
    }

    bool ImageCodec::encode(DataStream &stream, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_PNG */)
    {
        bool ret =false;

        return ret;
    }

    bool ImageCodec::encode(uint8_t *data, size_t size, const Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_PNG */)
    {
        bool ret = false;
        const ImageCodecBasePtr &codec = getImageCodec(eType);

        if (codec != nullptr)
        {
            ret = codec->encode(data, size, image);
        }

        return ret;
    }

    bool ImageCodec::decode(const String &name, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        bool ret = false;
        ArchivePtr archive;

        if (T3D_ARCHIVE_MGR.getArchive(name, archive))
        {
            MemoryDataStream stream;
            if (archive->read(name, stream))
            {
                ret = decode(stream, image, eType);
            }
        }

        return ret;
    }

    bool ImageCodec::decode(DataStream &stream, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        bool ret = false;

        uint8_t *data = nullptr;
        size_t size = stream.read(data);
        ret = decode(data, size, image, eType);

        return ret;
    }

    bool ImageCodec::decode(uint8_t *data, size_t size, Image &image, ImageCodecBase::FileType eType /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        bool ret = false;

        ImageCodecBasePtr codec = getImageCodec(data, size, eType);

        if (codec != ImageCodecBasePtr::NULL_PTR)
        {
            ret = codec->decode(data, size, image, eType);
        }

        return ret;
    }

    ImageCodecBasePtr ImageCodec::getImageCodec(uint8_t *data, size_t size, ImageCodecBase::FileType &eFileType) const
    {
        ImageCodecBasePtr codec;

        if (eFileType != ImageCodecBase::E_FT_UNKNOWN)
        {
            auto itr = mCodecMap.find(eFileType);
            if (itr != mCodecMap.end())
            {
                codec = itr->second;
            }
        }

        if (codec == nullptr)
        {
            for (auto i = mCodecList.begin(); i != mCodecList.end(); ++i)
            {
                if ((*i)->isSupportedType(data, size, eFileType))
                {
                    codec = *i;
                    break;
                }
            }
        }

        return codec;
    }

    ImageCodecBasePtr ImageCodec::getImageCodec(ImageCodecBase::FileType eFileType) const
    {
        ImageCodecBasePtr codec;
        auto itr = mCodecMap.find(eFileType);
        if (itr != mCodecMap.end())
        {
            codec = itr->second;
        }
        return codec;
    }
}
