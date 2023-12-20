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

#ifndef __T3D_IMAGE_MANAGER_H__
#define __T3D_IMAGE_MANAGER_H__


#include "Kernel/T3DConstant.h"
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ImageManager
        : public Singleton<ImageManager>
        , public ResourceManager
    {
    public:
        static ImageManagerPtr create();

        ImagePtr createImage(const String &name, uint32_t width, uint32_t height, PixelFormat format);
        
        ImagePtr loadImage(Archive *archive, const String &name);

        TResult saveImage(Archive *archive, Image *image);

    protected:
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        ResourcePtr loadResource(const String &name, DataStream &stream, int32_t argc, va_list args) override;
        
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_IMAGE_MGR    (ImageManager::getInstance()) 
}


#endif    /*__T3D_IMAGE_MANAGER_H__*/
