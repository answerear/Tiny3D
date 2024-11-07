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


#include "ImTextureManager.h"
#include "ImErrors.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImTextureManager::~ImTextureManager()
    {
        
    }

    //--------------------------------------------------------------------------

    ImTextureID ImTextureManager::loadTexture(const String &name)
    {
        ImTextureID texID = nullptr;

        do
        {
            auto itr = mTextures.find(name);
            if (itr != mTextures.end())
            {
                texID = itr->second->texture->getPixelBuffer()->getRHIResource()->getNativeObject();
                itr->second->refCount++;
                break;
            }
            
            ArchivePtr archive = T3D_ARCHIVE_MGR.getArchive(ARCHIVE_TYPE_FS, Dir::getAppPath(), Archive::AccessMode::kRead);
            T3D_ASSERT(archive != nullptr);
            ImagePtr image = T3D_IMAGE_MGR.loadImage(archive, name);
            T3D_ASSERT(image != nullptr);
            Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D(name, image);
            texID = texture->getPixelBuffer()->getRHIResource()->getNativeObject();
            TextureData *texData = new TextureData(texture);
            texData->refCount++;
            mTextures.emplace(name, texData);
            mTexToNames.emplace(texID, name);
        } while (false);
        
        return texID;
    }

    //--------------------------------------------------------------------------

    TResult ImTextureManager::unloadTexture(ImTextureID texID)
    {
        TResult ret = IM_OK;

        do
        {
            auto itr = mTexToNames.find(texID);
            if (itr == mTexToNames.end())
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            auto it = mTextures.find(itr->second);
            if (it == mTextures.end())
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            it->second->refCount--;

            if (it->second->refCount == 0)
            {
                // 没有引用，释放纹理，删除纹理数据对象，最后从对象池中删掉
                T3D_TEXTURE_MGR.unload(it->second->texture);
                T3D_SAFE_DELETE(it->second);
                mTextures.erase(it);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ImVec2 ImTextureManager::getTextureSize(ImTextureID texID)
    {
        ImVec2 size(0, 0);

        do
        {
            auto itr = mTexToNames.find(texID);
            if (itr == mTexToNames.end())
            {
                break;
            }

            auto it = mTextures.find(itr->second);
            if (it == mTextures.end())
            {
                break;
            }

            size.x = (float)it->second->texture->getWidth();
            size.y = (float)it->second->texture->getHeight();
        } while (false);

        return size;
    }

    //--------------------------------------------------------------------------

    void ImTextureManager::unloadAllTextures()
    {
        for (auto itr = mTextures.begin(); itr != mTextures.end(); ++itr)
        {
            T3D_TEXTURE_MGR.unload(itr->second->texture);
            T3D_SAFE_DELETE(itr->second);
        }

        mTextures.clear();
        mTexToNames.clear();
    }

    //--------------------------------------------------------------------------
}
