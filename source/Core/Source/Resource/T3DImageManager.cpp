/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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
        return save(image, archive); 
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

