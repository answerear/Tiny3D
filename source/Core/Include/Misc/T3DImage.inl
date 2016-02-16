

namespace Tiny3D
{
    inline Image::Image(const Image &other)
    {
        copy(other);
    }

    inline Image &Image::operator =(const Image &other)
    {
        copy(other);
        return *this;
    }

    inline bool Image::operator ==(const Image &other) const
    {
        return (compare(other) == 0);
    }

    inline bool Image::operator !=(const Image &other) const
    {
        return (compare(other) != 0);
    }

    inline uint8_t *Image::getData()
    {
        return mData;
    }

    inline const uint8_t *Image::getData() const
    {
        return mData;
    }

    inline size_t Image::getSize() const
    {
        return mHeight * mPitch;
    }

    inline int32_t Image::getWidth() const
    {
        return mWidth;
    }

    inline int32_t Image::getHeight() const
    {
        return mHeight;
    }

    inline int32_t Image::getPitch() const
    {
        return mPitch;
    }

    inline int32_t Image::getBPP() const
    {
        return mBPP;
    }

    inline PixelFormat Image::getFormat() const
    {
        return mFormat;
    }

    inline bool Image::hasAlpha() const
    {
        return mHasAlpha;
    }

    inline bool Image::isPremultipliedAlpha() const
    {
        return mIsPreMulti;
    }
}
