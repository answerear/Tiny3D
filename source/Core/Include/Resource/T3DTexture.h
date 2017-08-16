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
     * @brief ������Դ
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
            E_TU_DEFAULT = 0,   /// ��ͼƬ���������������
            E_TU_RENDERTARGET,  /// ��Ⱦ������
            E_TU_BLANK,         /// �հ�����
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
         * @brief ��Դ�������������ݹ���
         * @param [in] texture : Ŀ������
         * @param [in] dstRect : ָ��Ŀ����������Ĭ��nullptrΪ���Ƶ�����������
         * @param [in] srcRect : ָ��Դ��������Ĭ��nullptrΪ����������
         * @return ���óɹ�����true�����򷵻�false
         * @note ���Դ���������Ŀ����������һ�£�����Զ������Ŵ�����������ᵼ��������һ��������½�
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
