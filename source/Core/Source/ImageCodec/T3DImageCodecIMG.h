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

#ifndef __T3D_IMAGE_CODEC_IMG_H__
#define __T3D_IMAGE_CODEC_IMG_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    class ImageCodecIMG;

    T3D_DECLARE_SMART_PTR(ImageCodecIMG);

    class ImageCodecIMG : public ImageCodecBase
    {
    public:
        typedef std::vector<ImageCodecBase::FileType>   FileTypeList;
        typedef FileTypeList::iterator                  FileTypeListItr;
        typedef FileTypeList::const_iterator            FileTypeListConstItr;

        static ImageCodecIMGPtr create();

        virtual ~ImageCodecIMG();

        const FileTypeList &getSupportedFileTypeList() const { return mFileTypeList; }

        virtual bool startup() override;
        virtual bool shutdown() override;

        virtual bool isSupportedType(uint8_t *data, size_t size, FileType &eFileType) const override;
        virtual FileType getFileType() const override;

        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) override;

        virtual bool decode(uint8_t *data, size_t size, Image &image, FileType eFileType) override;

    protected:
        ImageCodecIMG();

        FileTypeList        mFileTypeList;

        static const size_t MAX_SUPPORTED_FILE_TYPE;
    };
}


#endif  /*__T3D_IMAGE_CODEC_IMG_H__*/
