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


#ifndef __T3D_RENDER_TEXTURE_H__
#define __T3D_RENDER_TEXTURE_H__


#include "Resource/T3DTexture.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderTexture : public Texture2D
    {
    public:
        static RenderTexturePtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality);

        TEXTURE_TYPE getTextureType() const override;
        
    protected:
        RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality);

        ~RenderTexture() override;

        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;

        TResult onLoad() override;
    };
}




#endif  /*__T3D_RENDER_TEXTURE_H__*/
