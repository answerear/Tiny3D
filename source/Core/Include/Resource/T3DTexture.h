

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Misc/T3DCommon.h"
#include "Math/T3DRect.h"
#include "T3DResource.h"


namespace Tiny3D
{
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

        virtual bool copyToTexture(const TexturePtr &texture, Rect *src = nullptr, Rect *dst = nullptr) const = 0;

    protected:
        Texture(const String &name, int32_t numMipMaps, int32_t texWidth = -1, int32_t texHeight = -1, 
            TexUsage texUsage = E_TU_DEFAULT, TexType texType = E_TEX_TYPE_2D, 
            PixelFormat format = E_PF_A8R8G8B8);

        virtual bool loadImage(const Image &src) = 0;

        TexType     mTexType;
        TexUsage    mTexUsage;
        int32_t     mNumMipMaps;

        int32_t     mTexWidth;
        int32_t     mTexHeight;

        int32_t     mImgWidth;
        int32_t     mImgHeight;

        PixelFormat mFormat;

        bool        mHasAlpha;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
