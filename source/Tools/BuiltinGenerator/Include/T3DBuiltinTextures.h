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


#pragma once


#include "T3DBuiltinGenPrerequisites.h"


namespace Tiny3D
{
    class BuiltinTextures : public Allocator, public Singleton<BuiltinTextures>
    {
    public:
        BuiltinTextures() = default;

        ~BuiltinTextures() = default;

        TResult generate(const String &rootPath);
        
    protected:
        TResult generateDefaultAlbedo(const String &outputPath);
        
        TResult generateTestTexture(const String &outputPath);

        /// 程序化生成一张天顶 → 地平线 → 地面的渐变 cubemap，作为默认天空盒贴图
        TResult generateDefaultSkybox(const String &outputPath);

        /// 扫描 textures/cubemaps/<name>/{px,nx,py,ny,pz,nz}.png，逐个导出 <name>.ttex
        TResult generateCubemaps(const String &searchPath, const String &outputPath);

        TResult generateCubemap(const String &facePath, const String &name, const String &outputPath);
        
        TResult generateTexture(const String &path, const String &outputPath);

        /// 把纹理和它的 meta 文件一起写到 outputPath 下
        TResult saveTextureWithMeta(const String &outputPath, const String &filename, Texture *texture);
    };

    #define T3D_BUILTIN_TEXTURES   (BuiltinTextures::getInstance())
}


