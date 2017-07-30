/*******************************************************************************
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
 ******************************************************************************/

#ifndef __T3D_IMAGE_CODEC_H__
#define __T3D_IMAGE_CODEC_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    class ImageCodec : public Singleton<ImageCodec>
    {
    public:
        ImageCodec();
        virtual ~ImageCodec();

        bool startup();
        bool shutdown();

        bool encode(const String &name, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG);
        bool encode(DataStream &stream, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG);
        bool encode(uint8_t *data, size_t size, const Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_PNG);

        bool decode(const String &name, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_UNKNOWN);
        bool decode(DataStream &stream, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_UNKNOWN);
        bool decode(uint8_t *data, size_t size, Image &image, ImageCodecBase::FileType eType = ImageCodecBase::E_FT_UNKNOWN);

    protected:
        ImageCodecBasePtr getImageCodec(uint8_t *data, size_t size, ImageCodecBase::FileType &eFileType) const;
        ImageCodecBasePtr getImageCodec(ImageCodecBase::FileType eFileType) const;

        typedef std::vector<ImageCodecBasePtr>      ImageCodecList;
        typedef ImageCodecList::iterator            ImageCodecListItr;
        typedef ImageCodecList::const_iterator      ImageCodecListConstItr;
        typedef ImageCodecList::value_type          ImageCodecListValue;

        typedef std::map<ImageCodecBase::FileType, ImageCodecBasePtr>       ImageCodecMap;
        typedef ImageCodecMap::iterator                                     ImageCodecMapItr;
        typedef ImageCodecMap::const_iterator                               ImageCodecMapConstItr;
        typedef ImageCodecMap::value_type                                   ImageCodecMapValue;
        
        ImageCodecList  mCodecList;
        ImageCodecMap   mCodecMap;

        static const size_t MAX_NUMBER_OF_CODEC;
    };

    #define T3D_IMAGE_CODEC     (ImageCodec::getInstance())
}




#endif  /*__T3D_IMAGE_CODEC_H__*/
