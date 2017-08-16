/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Misc/T3DCommon.h"
#include "T3DRect.h"
#include "T3DResource.h"


namespace Tiny3D
{
    /**
     * @brief 纹理资源
     */
    class T3D_ENGINE_API Texture : public Resource
    {
    public:
        enum TexType
        {
            E_TEX_TYPE_1D = 1,
            E_TEX_TYPE_2D,
            E_TEX_TYPE_3D,
            E_TEX_TYPE_CUBE_MAP,
        };

        enum TexUsage
        {
            E_TU_DEFAULT = 0,   /// 从图片加载数据填充纹理
            E_TU_RENDERTARGET,  /// 渲染到纹理
            E_TU_BLANK,         /// 空白纹理
        };

        static TexturePtr create(const String &name, int32_t numMipMaps, int32_t texWidth = -1, 
            int32_t texHeight = -1, TexUsage texUsage = E_TU_DEFAULT, TexType texType = E_TEX_TYPE_2D,
            PixelFormat format = E_PF_A8R8G8B8);

        virtual ~Texture();

        virtual Type getType() const override;

        TexType getTextureType() const  { return mTexType; }
        TexUsage getTextureUsage() const    { return mTexUsage; }

        int32_t getNumMipMaps() const   { return mNumMipMaps; }

        int32_t getTexWidth() const        { return mTexWidth; }
        int32_t getTexHeight() const       { return mTexHeight; }

        int32_t getOriginalWidth() const    { return mImgWidth; }
        int32_t getOriginalHeight() const   { return mImgHeight; }

        PixelFormat getFormat() const   { return mFormat; }

        bool hasAlpha() const           { return mHasAlpha; }

        HardwarePixelBufferPtr getPixelBuffer() const { return mPixelBuffer; }

        bool saveToFile(const String &path, const String &fileType);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        /**
         * @brief 从源纹理复制纹理数据过来
         * @param [in] texture : 目标纹理
         * @param [in] dstRect : 指定目标纹理区域，默认nullptr为复制到整个纹理上
         * @param [in] srcRect : 指定源纹理区域，默认nullptr为整个纹理复制
         * @return 调用成功返回true，否则返回false
         * @note 如果源纹理区域和目标纹理区域不一致，则会自动做缩放处理，但是这个会导致性能有一定程序的下降
         */
        bool copyTo(TexturePtr texture, Rect *dstRect = nullptr, Rect *srcRect = nullptr);

    protected:
        Texture(const String &name, int32_t numMipMaps, int32_t texWidth = -1, int32_t texHeight = -1, 
            TexUsage texUsage = E_TU_DEFAULT, TexType texType = E_TEX_TYPE_2D, 
            PixelFormat format = E_PF_A8R8G8B8);

        TexType     mTexType;
        TexUsage    mTexUsage;
        int32_t     mNumMipMaps;

        int32_t     mTexWidth;
        int32_t     mTexHeight;

        int32_t     mImgWidth;
        int32_t     mImgHeight;

        PixelFormat mFormat;

        bool        mHasAlpha;

        HardwarePixelBufferPtr  mPixelBuffer;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
