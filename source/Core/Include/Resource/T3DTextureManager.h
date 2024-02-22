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

#ifndef __T3D_TEXTURE_MANAGER_H__
#define __T3D_TEXTURE_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TextureManager
        : public Singleton<TextureManager>
        , public ResourceManager
    {
    public:
        static TextureManagerPtr create();

        RenderTexturePtr createRenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);

        Texture1DPtr createTexture1D(const String &name, uint32_t width, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1);

        Texture2DPtr createTexture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);

        Texture2DArrayPtr createTexture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);
        
        Texture3DPtr createTexture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1);

        CubemapPtr createCubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0);

        CubemapArrayPtr createCubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);

        // Texture1DPtr loadTexture1D(Archive *archive, const String &name);
        //
        // Texture2DPtr loadTexture2D(Archive *archive, const String &name);
        //
        // Texture2DArrayPtr loadTexture2DArray(Archive *archive, const String &name);
        //
        // Texture3DPtr loadTexture3D(Archive *archive, const String &name);
        //
        // CubemapPtr loadCubemap(Archive *archive, const String &name);
        //
        // CubemapArrayPtr loadCubemapArray(Archive *archive, const String &name);

        TexturePtr loadTexture(Archive *archive, const String &name);

        TResult saveTexture(Archive *archive, Texture *texture);

    protected:

        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        ResourcePtr loadResource(const String &name, DataStream &stream, int32_t argc, va_list args) override;
        
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_TEXTURE_MGR    (TextureManager::getInstance()) 
}


#endif    /*__T3D_TEXTURE_MANAGER_H__*/
