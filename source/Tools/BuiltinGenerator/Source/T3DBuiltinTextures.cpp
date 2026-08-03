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


#include "T3DBuiltinTextures.h"
#include "T3DBuiltinGuidUtil.h"

#include <algorithm>
#include <cmath>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    /// 6 面源图所在的目录名，相对 textures/
    static const char *CUBEMAP_DIR = "cubemaps";

    /// 面文件名（不含扩展名），顺序必须是 +X, -X, +Y, -Y, +Z, -Z
    static const char *CUBEMAP_FACE_NAMES[Cubemap::FACE_COUNT] =
    {
        "px", "nx", "py", "ny", "pz", "nz"
    };

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generate(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            String outputPath = rootPath + Dir::getNativeSeparator() + "textures";

            ret = generateDefaultAlbedo(outputPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate default albedo in %s ! ERROR [%d]", outputPath.c_str(), ret);
            }
            
            ret = generateTestTexture(outputPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate test texture in %s ! ERROR [%d]", outputPath.c_str(), ret);
            }

            ret = generateDefaultSkybox(outputPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate default skybox in %s ! ERROR [%d]", outputPath.c_str(), ret);
            }

            String searchPath = outputPath;
            ret = generateCubemaps(searchPath + Dir::getNativeSeparator() + CUBEMAP_DIR, outputPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate cubemaps in %s ! ERROR [%d]", outputPath.c_str(), ret);
            }

            ret = generateTexture(searchPath, outputPath);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generateDefaultAlbedo(const String &outputPath)
    {
        const uint32_t w = 4;
        const uint32_t h = 4;
        const uint32_t bytesPerPixel = 4;
        const uint32_t dataSize = w * h * bytesPerPixel;
        uint8_t *data = T3D_POD_NEW_ARRAY(uint8_t, dataSize);
        uint32_t i = 0;
        while (i < dataSize)
        {
            data[i++] = 255;
            data[i++] = 255;
            data[i++] = 255;
            data[i++] = 255;
        }
        
        Buffer texData;
        texData.setData(data, dataSize);
        T3D_POD_SAFE_DELETE_ARRAY(data);
        
        String name = "white";
        String filename = name + "." + Resource::EXT_TEXTURE;
        
        // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
        UUID existingUUID = BuiltinGuidUtil::readExistingMetaUUID(outputPath, filename + ".meta");
        TexturePtr texture = T3D_TEXTURE_MGR.createTexture2D(name, w, h, PixelFormat::E_PF_B8G8R8X8, texData, 1, 1, 0, existingUUID);

        SamplerDesc samplerDesc;
        texture->setSamplerDesc(samplerDesc);

        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(outputPath, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        TResult ret = T3D_TEXTURE_MGR.saveTexture(archive, filename, texture);
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Failed to generate default albedo texture %s ! ERROR [%d]", filename.c_str(), ret);
        }

        // Texture meta file
        MetaTexturePtr meta = MetaTexture::create(texture->getUUID());
        filename = filename + ".meta";
        ret = archive->write(filename,
            [](DataStream &stream, const String &filename, void *userData)
            {
                MetaTexture *meta = static_cast<MetaTexture *>(userData);
                return T3D_SERIALIZER_MGR.serialize(stream, meta);
            },
            meta.get());
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Failed to generate default albedo texture meta %s ! ERROR [%d]", filename.c_str(), ret);
        }
        
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generateTestTexture(const String &outputPath)
    {
        const uint32_t width = 64;
        const uint32_t height = 64;
        uint32_t pitch = Image::calcPitch(width, 32);
        const uint32_t dataSize = pitch * height;
        uint8_t *pixels = T3D_POD_NEW_ARRAY(uint8_t, dataSize);
        
        for (uint32_t y = 0; y < height; ++y)
        {
            uint8_t *lines = pixels + pitch * y;
            uint32_t i = 0;
            for (uint32_t x = 0; x < width; ++x)
            {
                if (x < 16 && y < 16)
                {
                    // top, blue
                    // B
                    lines[i++] = 196;
                    // G
                    lines[i++] = 114;
                    // R
                    lines[i++] = 68;
                }
                else if (x < 16 && y >= 16 && y < 32)
                {
                    // front, orange
                    // B
                    lines[i++] = 49;
                    // G
                    lines[i++] = 125;
                    // R
                    lines[i++] = 237;
                }
                else if (x >= 16 && x < 32 && y >= 16 && y < 32)
                {
                    // right, green
                    // B
                    lines[i++] = 71;
                    // G
                    lines[i++] = 173;
                    // R
                    lines[i++] = 112;
                }
                else if (x >= 32 && x < 48 && y >= 16 && y < 32)
                {
                    // back, yellow
                    // B
                    lines[i++] = 0;
                    // G
                    lines[i++] = 192;
                    // R
                    lines[i++] = 255;
                }
                else if ( x >= 48 && x < 64 && y >= 16 && y <32)
                {
                    // left, red
                    // B
                    lines[i++] = 0;
                    // G
                    lines[i++] = 0;
                    // R
                    lines[i++] = 255;
                }
                else if ( x < 16 && y >= 32 && y < 48)
                {
                    // bottom, purple
                    // B
                    lines[i++] = 160;
                    // G
                    lines[i++] = 48;
                    // R
                    lines[i++] = 112;
                }
                else
                {
                    // B
                    lines[i++] = 0;
                    // G
                    lines[i++] = 0;
                    // R
                    lines[i++] = 0;
                }
                
                // A
                lines[i++] = 255;
            }
        }
        
        Buffer texData;
        texData.Data = pixels;
        texData.DataSize = dataSize;

        // Testing texture
        String name = "Test";
        String filename = name + "." + Resource::EXT_TEXTURE;
        // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
        UUID existingUUID = BuiltinGuidUtil::readExistingMetaUUID(outputPath, filename + ".meta");
        Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D(name, width, height, PixelFormat::E_PF_B8G8R8X8, texData, 1, 1, 0, existingUUID);

        SamplerDesc desc;
        texture->setSamplerDesc(desc);
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(outputPath, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        TResult ret = T3D_TEXTURE_MGR.saveTexture(archive, filename, texture);
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Failed to generate testing texture %s ! ERROR [%d]", filename.c_str(), ret);
        }

        // Testing texture meta file
        MetaTexturePtr meta = MetaTexture::create(texture->getUUID());
        filename = filename + ".meta";
        ret = archive->write(filename,
            [](DataStream &stream, const String &filename, void *userData)
            {
                MetaTexture *meta = static_cast<MetaTexture *>(userData);
                return T3D_SERIALIZER_MGR.serialize(stream, meta);
            },
            meta.get());
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Failed to generate testing texture meta %s ! ERROR [%d]", filename.c_str(), ret);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::saveTextureWithMeta(const String &outputPath, const String &filename, Texture *texture)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(outputPath, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        T3D_ASSERT(archive != nullptr);

        TResult ret = T3D_TEXTURE_MGR.saveTexture(archive, filename, texture);
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Failed to save texture %s ! ERROR [%d]", filename.c_str(), ret);
            return ret;
        }

        MetaTexturePtr meta = MetaTexture::create(texture->getUUID());
        const String metaName = filename + ".meta";
        ret = archive->write(metaName,
            [](DataStream &stream, const String &filename, void *userData)
            {
                MetaTexture *meta = static_cast<MetaTexture *>(userData);
                return T3D_SERIALIZER_MGR.serialize(stream, meta);
            },
            meta.get());
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Failed to save texture meta %s ! ERROR [%d]", metaName.c_str(), ret);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generateDefaultSkybox(const String &outputPath)
    {
        const uint32_t size = 64;
        const uint32_t pitch = Image::calcPitch(size, 32);
        const size_t faceSize = (size_t)pitch * size;

        // 天顶 → 地平线 → 地面的三段渐变，RGB
        const float zenith[3]  = { 0.28f, 0.48f, 0.86f };
        const float horizon[3] = { 0.78f, 0.85f, 0.92f };
        const float ground[3]  = { 0.32f, 0.30f, 0.27f };

        Buffer texData;
        texData.DataSize = faceSize * Cubemap::FACE_COUNT;
        texData.Data = T3D_POD_NEW_ARRAY(uint8_t, texData.DataSize);

        for (uint32_t face = 0; face < Cubemap::FACE_COUNT; ++face)
        {
            uint8_t *facePixels = texData.Data + faceSize * face;

            for (uint32_t y = 0; y < size; ++y)
            {
                uint8_t *line = facePixels + pitch * y;
                uint32_t i = 0;

                for (uint32_t x = 0; x < size; ++x)
                {
                    // 面内纹素坐标映射到 [-1, 1]
                    const float u = (x + 0.5f) / size * 2.0f - 1.0f;
                    const float v = (y + 0.5f) / size * 2.0f - 1.0f;

                    // 标准 cube face 朝向，顺序 +X, -X, +Y, -Y, +Z, -Z
                    float dir[3];
                    switch (face)
                    {
                    case 0: dir[0] =  1.0f; dir[1] =   -v; dir[2] =   -u; break;
                    case 1: dir[0] = -1.0f; dir[1] =   -v; dir[2] =    u; break;
                    case 2: dir[0] =     u; dir[1] = 1.0f; dir[2] =    v; break;
                    case 3: dir[0] =     u; dir[1] =-1.0f; dir[2] =   -v; break;
                    case 4: dir[0] =     u; dir[1] =   -v; dir[2] = 1.0f; break;
                    default:dir[0] =    -u; dir[1] =   -v; dir[2] =-1.0f; break;
                    }

                    const float len = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
                    const float ny = dir[1] / len;

                    // 靠近地平线时用更陡的曲线，天空看起来更自然
                    const float *target = (ny >= 0.0f ? zenith : ground);
                    const float t = std::pow(std::abs(ny), ny >= 0.0f ? 0.45f : 0.60f);

                    float color[3];
                    for (uint32_t c = 0; c < 3; ++c)
                    {
                        const float value = horizon[c] + (target[c] - horizon[c]) * t;
                        color[c] = std::min(std::max(value, 0.0f), 1.0f);
                    }

                    // B8G8R8X8
                    line[i++] = (uint8_t)(color[2] * 255.0f);
                    line[i++] = (uint8_t)(color[1] * 255.0f);
                    line[i++] = (uint8_t)(color[0] * 255.0f);
                    line[i++] = 255;
                }
            }
        }

        const String name = "skybox_default";
        const String filename = name + "." + Resource::EXT_TEXTURE;

        // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
        UUID existingUUID = BuiltinGuidUtil::readExistingMetaUUID(outputPath, filename + ".meta");
        CubemapPtr texture = T3D_TEXTURE_MGR.createCubemap(name, size, size, PixelFormat::E_PF_B8G8R8X8, texData, 1, 1, 0, existingUUID);
        if (texture == nullptr)
        {
            texData.release();
            BGEN_LOG_ERROR("Failed to create default skybox cubemap !");
            return T3D_ERR_RES_LOAD_FAILED;
        }

        SamplerDesc samplerDesc;
        samplerDesc.AddressU = TextureAddressMode::kClamp;
        samplerDesc.AddressV = TextureAddressMode::kClamp;
        samplerDesc.AddressW = TextureAddressMode::kClamp;
        texture->setSamplerDesc(samplerDesc);

        return saveTextureWithMeta(outputPath, filename, texture);
    }

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generateCubemaps(const String &searchPath, const String &outputPath)
    {
        String searchFile = searchPath + Dir::getNativeSeparator() + "*.*";
        Dir dir;
        bool working = dir.findFile(searchFile);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // 每个子目录就是一张 cubemap，目录名即资源名
                const String facePath = dir.getFilePath();
                const size_t pos = facePath.find_last_of("/\\");
                const String name = (pos == String::npos ? facePath : facePath.substr(pos + 1));

                generateCubemap(facePath, name, outputPath);
            }

            working = dir.findNextFile();
        }

        dir.close();

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generateCubemap(const String &facePath, const String &name, const String &outputPath)
    {
        BGEN_LOG_INFO("Begin generating cubemap (%s) ...", facePath.c_str());

        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(facePath, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
        if (archive == nullptr)
        {
            BGEN_LOG_ERROR("Failed to open cubemap face folder (%s) !", facePath.c_str());
            return T3D_ERR_RES_LOAD_FAILED;
        }

        ImagePtr images[Cubemap::FACE_COUNT];
        Image *faces[Cubemap::FACE_COUNT] = { nullptr };

        for (uint32_t i = 0; i < Cubemap::FACE_COUNT; ++i)
        {
            const String faceFile = String(CUBEMAP_FACE_NAMES[i]) + ".png";
            images[i] = T3D_IMAGE_MGR.loadImage(archive, faceFile);
            if (images[i] == nullptr)
            {
                BGEN_LOG_ERROR("Missing cubemap face (%s) in %s !", faceFile.c_str(), facePath.c_str());
                return T3D_ERR_RES_LOAD_FAILED;
            }
            faces[i] = images[i];
        }

        const String filename = name + "." + Resource::EXT_TEXTURE;

        // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
        UUID existingUUID = BuiltinGuidUtil::readExistingMetaUUID(outputPath, filename + ".meta");
        CubemapPtr texture = T3D_TEXTURE_MGR.createCubemapFromImages(name, faces, 1, existingUUID);
        if (texture == nullptr)
        {
            BGEN_LOG_ERROR("Failed to create cubemap (%s) !", name.c_str());
            return T3D_ERR_RES_LOAD_FAILED;
        }

        SamplerDesc samplerDesc;
        samplerDesc.AddressU = TextureAddressMode::kClamp;
        samplerDesc.AddressV = TextureAddressMode::kClamp;
        samplerDesc.AddressW = TextureAddressMode::kClamp;
        texture->setSamplerDesc(samplerDesc);

        TResult ret = saveTextureWithMeta(outputPath, filename, texture);
        if (T3D_SUCCEEDED(ret))
        {
            BGEN_LOG_INFO("Completed generating cubemap (%s) !", facePath.c_str());
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinTextures::generateTexture(const String &searchPath, const String &outputPath)
    {
        TResult ret;

        String searchFile = searchPath + Dir::getNativeSeparator() + "*.*";
        Dir dir;
        bool working = dir.findFile(searchFile);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // cubemaps/ 下的散图是 cubemap 的面，已经由 generateCubemaps 处理，
                // 不能再当成独立的 2D 纹理导出一遍
                const String subPath = dir.getFilePath();
                const size_t pos = subPath.find_last_of("/\\");
                const String dirName = (pos == String::npos ? subPath : subPath.substr(pos + 1));

                if (dirName != CUBEMAP_DIR)
                {
                    generateTexture(subPath, outputPath);
                }
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                String path, title, ext;
                Dir::parsePath(filePath, path, title, ext);

                if (ext != "ttex" && ext != "meta")
                {
                    BGEN_LOG_INFO("Begin generating texture (%s) ...", filePath.c_str());

                    ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
                    T3D_ASSERT(archive != nullptr);
                    String name = title + "." + ext;
                    ImagePtr image = T3D_IMAGE_MGR.loadImage(archive, name);
                    T3D_ASSERT(image != nullptr);
                    name = title + "." + Resource::EXT_TEXTURE;

                    // 复用已有资源的 guid，避免重生成导致引用失效：创建时即传入旧 guid
                    UUID existingUUID = BuiltinGuidUtil::readExistingMetaUUID(outputPath, name + ".meta");
                    TexturePtr texture = T3D_TEXTURE_MGR.createTexture2D(name, image, 1, 1, 0, existingUUID);
                    T3D_ASSERT(texture != nullptr);

                    archive = T3D_ARCHIVE_MGR.loadArchive(outputPath, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
                    T3D_ASSERT(archive != nullptr);
                    SamplerDesc samplerDesc;
                    texture->setSamplerDesc(samplerDesc);

                    // Generate texture file
                    ret = T3D_TEXTURE_MGR.saveTexture(archive, name, texture);
                    if (T3D_FAILED(ret))
                    {
                        T3D_LOG_ERROR("Failed to generate texture (%s) !", outputPath.c_str());
                    }
                    else
                    {
                        BGEN_LOG_INFO("Completed generating texture (%s) !", filePath.c_str());
                    }

                    // Generate texture meta file
                    MetaTexturePtr meta = MetaTexture::create(texture->getUUID());
                    name = name + ".meta";
                    ret = archive->write(name,
                        [](DataStream &stream, const String &filename, void *userData)
                        {
                            MetaTexture *meta = static_cast<MetaTexture *>(userData);
                            return T3D_SERIALIZER_MGR.serialize(stream, meta);
                        },
                        meta.get());
                    if (T3D_FAILED(ret))
                    {
                        BGEN_LOG_ERROR("Failed to generate texture meta (%s) ! ERROR [%d]", outputPath.c_str(), ret);
                    }
                }
            }

            working = dir.findNextFile();
        }

        dir.close();

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
