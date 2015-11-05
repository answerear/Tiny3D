

#include "Misc/T3DImage.h"


namespace Tiny3D
{
    Image::Image()
    {

    }

    Image::Image(int32_t width, int32_t height, EPixelFormat format)
    {

    }

    Image::~Image()
    {

    }

    bool Image::load(const String &name)
    {
        return true;
    }

    bool Image::save(const String &name) const
    {
        return true;
    }

    bool Image::flip()
    {
        return true;
    }

    bool Image::mirror()
    {
        return true;
    }

    bool Image::fill(const Color4 &rkColor)
    {
        return true;
    }

    int32_t Image::compare(const Image &other, bool compareAlpha /* = true */) const
    {
        return 0;
    }

    void Image::copy(const Image &other)
    {

    }
}
