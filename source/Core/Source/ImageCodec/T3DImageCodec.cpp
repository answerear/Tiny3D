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

#include "ImageCodec/T3DImageCodec.h"
#include "Kernel/T3DArchiveManager.h"
#include "Kernel/T3DArchive.h"
#include "T3DErrorDef.h"
#include "Kernel/T3DAgent.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImageCodecPtr ImageCodec::create()
    {
        ImageCodecPtr codec = T3D_NEW ImageCodec();
        return codec;
    }

    //--------------------------------------------------------------------------

    ImageCodec::ImageCodec()
    {

    }

    //--------------------------------------------------------------------------
    
    ImageCodec::~ImageCodec()
    {

    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::addImageCodec(ImageCodecBase::FileType type, ImageCodecBase *codec)
    {
        TResult ret = T3D_OK;

        mCodecMap.emplace(type, codec);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ImageCodec::removeImageCodec(ImageCodecBase::FileType type)
    {
        TResult ret = T3D_OK;

        mCodecMap.erase(type);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::encode(const String &name, const Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::FileType::PNG */)
    {
        TResult ret = T3D_OK;

        FileDataStream fs;

        do 
        {
            if (!fs.open(name.c_str(), FileDataStream::E_MODE_WRITE_ONLY))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Open file %s failed when encoding image !",
                    name.c_str());
                break;
            }

            ret = encode(fs, image, type);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        if (fs.isOpened())
        {
            fs.close();
        }

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ImageCodec::encode(DataStream &stream, const Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::PNG */)
    {
        TResult ret = T3D_OK;

        do 
        {
            uint8_t *data = nullptr;
            size_t size = 0;

            ret = encode(data, size, image, type);
            if (T3D_FAILED(ret))
            {
                break;
            }

            if (stream.write(data, size) != size)
            {
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Write file content failed !");
            }
            
            T3D_SAFE_DELETE_ARRAY(data);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ImageCodec::encode(uint8_t *&data, size_t &size, const Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::FileType::PNG */)
    {
        TResult ret = T3D_OK;

        do 
        {
            ImageCodecBasePtr codec = getImageCodec(type);
            if (codec == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC, 
                    "Could not find the destinate image codec !");
                break;
            }

            ret = codec->encode(data, size, image, type);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::decode(const String &name, Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::FileType::UNKNOWN */)
    {
        TResult ret = T3D_OK;

        // do 
        // {
        //     ArchivePtr archive = T3D_AGENT.getAssetsArchive(name);
        //     if (archive == nullptr)
        //     {
        //         ret = T3D_ERR_IMG_NOT_FOUND;
        //         T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
        //             "Could not find the archive for file %s !", name.c_str());
        //         break;
        //     }
        //     
        //     String path = T3D_AGENT.getMainAssetsPath(name);
        //     MemoryDataStream stream;
        //     ret = archive->read(path, stream);
        //     if (T3D_FAILED(ret))
        //     {
        //         T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
        //             "Read image content failed from file %s ! ", name.c_str());
        //         break;
        //     }
        //
        //     ret = decode(stream, image, type);
        // } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ImageCodec::decode(DataStream &stream, Image &image, 
        ImageCodecBase::FileType type /* = ImageCodecBase::FileType::UNKNOWN */)
    {
        TResult ret = T3D_OK;

        do 
        {
            uint8_t *data = nullptr;
            size_t size = stream.read(data);
            if (size == 0)
            {
                ret = T3D_ERR_FILE_DATA_MISSING;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC, 
                    "Read image content failed !");
                break;
            }

            ret = decode(data, size, image, type);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ImageCodec::decode(uint8_t *data, size_t size, Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::FileType::UNKNOWN */)
    {
        TResult ret = T3D_OK;

        do 
        {
            ImageCodecBasePtr codec = getImageCodec(data, size, type);
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->decode(data, size, image, type);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::flip(Image &image)
    {
        TResult ret = T3D_OK;

        do 
        {
            ImageCodecBasePtr codec = getImageCodec(
                (ImageCodecBase::FileType)image.getFileFormat());
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->flip(image);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::mirror(Image &image)
    {
        TResult ret = T3D_OK;

        do 
        {
            ImageCodecBasePtr codec = getImageCodec(
                (ImageCodecBase::FileType)image.getFileFormat());
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->mirror(image);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::fill(Image &image, const Color4 &color)
    {
        TResult ret = T3D_OK;

        do 
        {
            ImageCodecBasePtr codec = getImageCodec(
                (ImageCodecBase::FileType)image.getFileFormat());
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->fill(image, color);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::copy(const Image &srcImage, const Rect *srcRect, 
        Image &dstImage, const Rect *dstRect, uint32_t filter)
    {
        TResult ret = T3D_OK;

        do 
        {
            ImageCodecBasePtr codec = getImageCodec(
                (ImageCodecBase::FileType)srcImage.getFileFormat());
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->copy(srcImage, srcRect, dstImage, dstRect, filter);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::convert(Image &image, PixelFormat format)
    {
        TResult ret = T3D_OK;

        do
        {
            ImageCodecBasePtr codec = getImageCodec(
                (ImageCodecBase::FileType)image.getFileFormat());
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->convert(image, format);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::convert(const Image &srcImage, Image &dstImage, PixelFormat format)
    {
        TResult ret = T3D_OK;

        do
        {
            ImageCodecBasePtr codec = getImageCodec(
                (ImageCodecBase::FileType)srcImage.getFileFormat());
            if (codec == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_IMAGE_CODEC,
                    "Could not find the image codec !");
                break;
            }

            ret = codec->convert(srcImage, dstImage, format);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ImageCodecBasePtr ImageCodec::getImageCodec(uint8_t *data, size_t size, ImageCodecBase::FileType &type) const
    {
        ImageCodecBasePtr codec;

        if (type != ImageCodecBase::FileType::UNKNOWN)
        {
            auto itr = mCodecMap.find(type);
            if (itr != mCodecMap.end())
            {
                codec = itr->second;
            }
        }
        else
        {
            auto itr = mCodecMap.begin();
            while (itr != mCodecMap.end())
            {
                if (itr->second->isSupportedType(data, size, type))
                {
                    codec = itr->second;
                    break;
                }
                ++itr;
            }
        }

        return codec;
    }

    //--------------------------------------------------------------------------
    
    ImageCodecBasePtr ImageCodec::getImageCodec(ImageCodecBase::FileType type) const
    {
        ImageCodecBasePtr codec;
        auto itr = mCodecMap.find(type);

        if (itr != mCodecMap.end())
        {
            codec = itr->second;
        }

        return codec;
    }
}

