

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Misc/T3DCommon.h"
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
            E_TU_DEFAULT = 0,
            E_TU_RENDERTARGET,
        };

        virtual ~Texture();

        virtual Type getType() const override;

        TexType getTextureType() const  { return mTexType; }
        TexUsage getTextureUsage() const    { return mTexUsage; }

        int32_t getNumMipMaps() const   { return mNumMipMaps; }

        int32_t getWidth() const        { return mWidth; }
        int32_t getHeight() const       { return mHeight; }

        PixelFormat getFormat() const   { return mFormat; }

        bool hasAlpha() const           { return mHasAlpha; }

    protected:
        Texture(const String &name, TexType texType, TexUsage texUsage);

        virtual bool loadImage(const Image &src) = 0;

        TexType     mTexType;
        TexUsage    mTexUsage;
        int32_t     mNumMipMaps;

        int32_t     mWidth;
        int32_t     mHeight;

        PixelFormat mFormat;

        bool        mHasAlpha;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
