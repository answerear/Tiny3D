

#ifndef __T3D_IMAGE_H__
#define __T3D_IMAGE_H__



#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Image : public Object
    {
    public:
        explicit Image();
        Image(int32_t width, int32_t height, EPixelFormat format);
        Image(const Image &other);
        virtual ~Image();

        Image &operator =(const Image &other);

        bool operator ==(const Image &other) const;
        bool operator !=(const Image &other) const;

        bool load(const String &name);
        bool save(const String &name) const;

        bool flip();
        bool mirror();

        bool fill(const Color4 &rkColor);

        int32_t compare(const Image &other, bool compareAlpha = true) const;

        uint8_t *getData();
        const uint8_t *getData() const;

        size_t getSize() const;

        int32_t getWidth() const;
        int32_t getHeight() const;
        int32_t getPitch() const;
        int32_t getBPP() const;

        bool hasAlpha() const;

    protected:
        void copy(const Image &other);

    private:
        int32_t     mWidth;
        int32_t     mHeight;
        int32_t     mBPP;
        int32_t     mPitch;

        EPixelFormat    mFormat;

        bool        mHasAlpha;

        uint8_t     *mData;
    };
}


#include "T3DImage.inl"


#endif  /*__T3D_IMAGE_H__*/

