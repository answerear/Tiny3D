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

#ifndef __T3D_D3D9_TEXTURE_H__
#define __T3D_D3D9_TEXTURE_H__


#include "T3DD3D9Prerequisites.h"


namespace Tiny3D
{
    class T3D_D3D9RENDERER_API D3D9Texture : public Texture
    {
    public:
        static TexturePtr create(const String &name, int32_t width, int32_t height,
            int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType);

        virtual ~D3D9Texture();

        LPDIRECT3DTEXTURE9 getD3DTexture()  { return mD3DTexture; }

    protected:
        D3D9Texture(const String &name, int32_t width, int32_t height,
            int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType);

        virtual bool load() override;
        virtual void unload() override;
        virtual bool loadImage(const Image &src) override;
        virtual bool copyToTexture(const TexturePtr &texture, Rect *src /* = nullptr */, Rect *dst /* = nullptr */) const override;
        virtual ResourcePtr clone() const override;

        virtual size_t writeData(uint8_t *data, size_t size, Rect *dst /* = nullptr */, Rect *src /* = nullptr */) override;
        virtual size_t readData(uint8_t *data, size_t size, Rect *dst /* = nullptr */, Rect *src /* = nullptr */) override;

        bool createTexture();
        bool createRenderTexture();

        LPDIRECT3DTEXTURE9  mD3DTexture;
    };
}


#endif  /*__T3D_D3D9_TEXTURE_H__*/
