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


#include "Resource/T3DImageManager.h"
#include "Resource/T3DImage.h"
#include "Kernel/T3DAgent.h"
#include "ImageCodec/T3DImageCodec.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImageManagerPtr ImageManager::create()
    {
        return new ImageManager();
    }

    //--------------------------------------------------------------------------
    
    ImagePtr ImageManager::createImage(const String &name, uint32_t width, uint32_t height, PixelFormat format)
    {
        return smart_pointer_cast<Image>(createResource(name, 3, width, height, format));
    }

    //--------------------------------------------------------------------------

    ImagePtr ImageManager::loadImage(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Image>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    TResult ImageManager::saveImage(Archive *archive, Image *image)
    {
        return save(archive, image); 
    }

    //--------------------------------------------------------------------------

    ResourcePtr ImageManager::newResource(const String &name, int32_t argc, va_list args)
    {
        T3D_ASSERT(argc == 3, "The number of arguments when create Image does not match !");
        
        ImagePtr image = nullptr;
        
        uint32_t width = va_arg(args, uint32_t);
        uint32_t height = va_arg(args, uint32_t);
        PixelFormat format = va_arg(args, PixelFormat);
        
        return Image::create(name, width, height, format);
    }

    //--------------------------------------------------------------------------

    ResourcePtr ImageManager::loadResource(const String &name, DataStream &stream, int32_t argc, va_list args)
    {
        ImagePtr image;

        do
        {
            image = Image::create(name);
            if (image == nullptr)
            {
                break;
            }
            
            TResult ret = T3D_IMAGE_CODEC.decode(stream, *image);
            if (T3D_FAILED(ret))
            {
                image = nullptr;
                break;
            }
        } while (false);
        
        return image;
    }
    
    //--------------------------------------------------------------------------

    TResult ImageManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kImage, "Save resource must be shader !");
        Image *image = static_cast<Image*>(res);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

