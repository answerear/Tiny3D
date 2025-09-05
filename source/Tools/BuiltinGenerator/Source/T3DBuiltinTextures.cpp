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


namespace Tiny3D
{
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

            String searchPath = outputPath;
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

        String name = "white";
        String filename = name + "." + Resource::EXT_TEXTURE;
        
        TexturePtr texture = T3D_TEXTURE_MGR.createTexture2D(name, w, h, PixelFormat::E_PF_B8G8R8X8, texData);

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
        Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D(name, width, height, PixelFormat::E_PF_B8G8R8X8, texData);
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
                // directory
                generateTexture(dir.getFilePath(), outputPath);
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
                    TexturePtr texture = T3D_TEXTURE_MGR.createTexture2D(name, image);
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
