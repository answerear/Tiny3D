/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Resource/T3DResource.h"
#include "Kernel/T3DPixelFormat.h"
#include "Render/T3DHardwarePixelBuffer.h"


namespace Tiny3D
{
    /**
     * @brief 纹理类型
     */
    enum TextureType
    {
        E_TEX_TYPE_1D = 1,      /**< 一维纹理 */
        E_TEX_TYPE_2D,          /**< 二维纹理 */
        E_TEX_TYPE_3D,          /**< 三位纹理 */
        E_TEX_TYPE_CUBE_MAP,    /**< CubeMap 纹理 */
        E_TEX_TYPE_2D_ARRAY,
        E_TEX_TYPE_2D_RECT
    };

    /**
     * @brief 纹理资源
     */
    class T3D_ENGINE_API Texture : public Resource
    {
    public:
        /**
         * @brief 纹理用途
         */
        enum TexUsage
        {
            E_TU_DEFAULT = 0,       /**< 默认纹理，从图片加载数据填充纹理 */
            E_TU_RENDERTARGET,      /**< 渲染到纹理 */
            E_TU_BLANK,             /**< 空白纹理 */
        };

        /** 
         * @brief 创建 Texture 对象
         * @param [in] name : 纹理资源名称
         * @param [in] mpimaps : 纹理的 mipmap 层级数量
         * @param [in] texWidth : 纹理宽度，默认从文件读取
         * @param [in] texHeight : 纹理高度，默认从文件读取
         * @param [in] texUsage : 纹理用途，默认是从图片加载直接用于渲染的
         * @param [in] texType : 纹理类型，默认是2D纹理
         * @param [in] format : 纹理像素格式，默认是A8R8G8B8
         * @return 返回一个 Texture 对象
         * @see enum TexUsage
         * @see enum TexType
         */
        static TexturePtr create(const String &name, size_t mipmaps, 
            size_t texWidth = 0, size_t texHeight = 0, 
            TexUsage texUsage = E_TU_DEFAULT, TextureType texType = E_TEX_TYPE_2D,
            PixelFormat format = PixelFormat::E_PF_A8R8G8B8);

        /** 
         * @brief 析构函数 
         */
        virtual ~Texture();

        /** 重写 Resource::getType() */
        virtual Type getType() const override;

        /** 重写 Resource::clone() */
        virtual ResourcePtr clone() const override;

        /**
         * @brief 获取纹理类型
         */
        TextureType getTexType() const { return mTexType; }

        /**
         * @brief 获取纹理层级
         */
        size_t getMipMaps() const { return mMipmaps; }

        /**
         * @brief 获取纹理宽度
         */
        size_t getTexWidth() const { return mTexWidth; }

        /**
         * @brief 获取纹理高度
         */
        size_t getTexHeight() const { return mTexHeight; }

        /**
         * @brief 获取原图宽度
         */
        size_t getOriginalWidth() const { return mImgWidth; }

        /**
         * @brief 获取原图高度
         */
        size_t getOriginalHeight() const { return mImgHeight; }

        /**
         * @brief 获取纹理像素格式
         */
        PixelFormat getPixelFormat() const { return mFormat; }

        /**
         * @brief 获取是否带Alpha通道
         */
        bool hasAlpha() const { return mHasAlpha; }

        /**
         * @brief 获取像素缓冲对象
         */
        HardwarePixelBufferPtr getPixelBuffer() const { return mPBO; }

        /**
         * @brief 保存到文件
         * @param [in] path : 文件名和路径
         * @param [in] fileType : 文件类型
         */
        TResult saveToFile(const String &path, uint32_t fileType) const;

        /**
         * @brief 从源纹理复制纹理数据过来
         * @param [in] texture : 目标纹理
         * @param [in] dstRect : 指定目标纹理区域，默认nullptr为复制到整个纹理上
         * @param [in] srcRect : 指定源纹理区域，默认nullptr为整个纹理复制
         * @return 调用成功返回 T3D_OK
         * @remarks 如果源纹理区域和目标纹理区域不一致，则会自动做缩放处理，
         *      但是这个会导致性能有一定程序的下降
         */
        TResult copyTo(TexturePtr texture, Rect *dstRect = nullptr, 
            Rect *srcRect = nullptr);

    protected:
        /** 构造函数 */
        Texture(const String &name, size_t mipmaps, size_t texWidth, 
            size_t texHeight, TexUsage texUsage, TextureType texType, 
            PixelFormat format);

        /** 重写 Resource::load() */
        virtual TResult load() override;

        /** 重写 Resource::unlaod() */
        virtual TResult unload() override;

    protected:
        TextureType                 mTexType;       /**< 纹理类型 */
        TexUsage                mTexUsage;      /**< 纹理用途 */

        size_t                  mMipmaps;       /**< 纹理LOD层数 */

        size_t                  mTexWidth;      /**< 纹理宽度 */
        size_t                  mTexHeight;     /**< 纹理高度 */
        size_t                  mImgWidth;      /**< 图像宽度 */
        size_t                  mImgHeight;     /**< 图像高度 */

        PixelFormat             mFormat;        /**< 像素格式 */

        bool                    mHasAlpha;      /**< 是否有 Alpha 通道 */

        HardwarePixelBufferPtr  mPBO;           /**< 像素数据对象 */
    };
}


#endif  /*__T3D_TEXTURE_H__*/
