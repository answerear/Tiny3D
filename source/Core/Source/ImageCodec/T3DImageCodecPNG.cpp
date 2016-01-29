

#include "ImageCodec/T3DImageCodecPNG.h"
#include <png.h>


namespace Tiny3D
{
    ImageCodecPNG::ImageCodecPNG()
    {

    }

    ImageCodecPNG::~ImageCodecPNG()
    {

    }

    ImageCodec::EType ImageCodecPNG::getType() const
    {
        return E_TYPE_PNG;
    }

    bool ImageCodecPNG::encode(const String &name, const Image &image)
    {
        bool ret = false;
        FileDataStream fs;
        if (fs.open(name.c_str(), FileDataStream::E_MODE_TRUNCATE|FileDataStream::E_MODE_WRITE_ONLY))
        {
            ret = encode(fs, image);
            fs.close();
        }

        return ret;
    }

    bool ImageCodecPNG::encode(DataStream &stream, const Image &image)
    {
        size_t size = stream.size();
        uint8_t *data = new uint8_t[size];
        bool ret = encode(data, size, image);
        T3D_SAFE_DELETE_ARRAY(data);
        return ret;
    }

    bool ImageCodecPNG::encode(uint8_t *&data, size_t &size, const Image &image)
    {
        return true;
    }

    bool ImageCodecPNG::decode(const String &name, Image &image)
    {
        bool ret = false;
        FileDataStream fs;
        if (fs.open(name.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            ret = decode(fs, image);
            fs.close();
        }

        return ret;
    }

    bool ImageCodecPNG::decode(DataStream &stream, Image &image)
    {
        size_t size = stream.size();
        uint8_t *data = new uint8_t[size];
        bool ret = decode(data, size, image);
        T3D_SAFE_DELETE_ARRAY(data);
        return ret;
    }

    bool ImageCodecPNG::decode(const uint8_t *data, size_t size, Image &image)
    {
        const size_t PNGSIGSIZE = 8;
        bool ret = false;

        png_byte        header[PNGSIGSIZE] = {0};
        png_structp     png_ptr = 0;
        png_infop       info_ptr = 0;

        do
        {
            // png header len is 8 bytes
            if (size < PNGSIGSIZE)
                break;

            // check the data is png or not
            memcpy(header, data, PNGSIGSIZE);
            if (png_sig_cmp(header, 0, PNGSIGSIZE))
                break;

            // init png_struct
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
            if (png_ptr != nullptr)
                break;

        } while (0);

        return ret;
    }
}
