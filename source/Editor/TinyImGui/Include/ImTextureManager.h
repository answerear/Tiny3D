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


#include "ImPrerequisites.h"


namespace Tiny3D
{
    class TINYIMGUI_API ImTextureManager : public Singleton<ImTextureManager>
    {
    public:
        ~ImTextureManager() override;

        /**
         * 加载纹理
         * @param name : 图像文件相对路径和名称
         * @return : 调用成功返回 imgui 使用的纹理对象
         */
        ImTextureID loadTexture(const String &name);

        /**
         * 卸载纹理，纹理用完后需要卸载
         * @param texID : imgui 使用的纹理对象
         * @return : 调用成功返回 IM_OK
         */
        TResult unloadTexture(ImTextureID texID);

        /**
         * 获取纹理大小
         * @param texID : 纹理 ID 
         * @return 返回纹理大小
         */
        ImVec2 getTextureSize(ImTextureID texID);

        /**
         * 卸载所有纹理
         */
        void unloadAllTextures();

    protected:
        struct TextureData
        {
            TextureData() = default;

            TextureData(Texture2D *tex)
                : texture(tex)
            {
            }
            
            Texture2DPtr    texture {nullptr};
            uint32_t        refCount {0};
        };
        
        using Textures = TUnorderedMap<String, TextureData*>;
        using TextureToNames = TUnorderedMap<ImTextureID, String>;

        /// 所有加载的纹理缓存，方便复用
        Textures        mTextures {};
        /// 用于卸载时候，根据 imgui 纹理对象，查找到对应的纹理名称
        TextureToNames  mTexToNames {};
    };

    #define IM_TEXTURE_MGR      (ImTextureManager::getInstance()) 
}
