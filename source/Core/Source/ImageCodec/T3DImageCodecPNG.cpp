

#include "ImageCodec/T3DImageCodecPNG.h"
#include "Misc/T3DImage.h"
#include <png.h>


namespace Tiny3D
{
    #define T3D_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
        (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
        ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
        ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
        ((unsigned)(unsigned char)(va) << 24))

    typedef struct 
    {
        uint8_t *data;
        int size;
        int offset;
    }tImageSource;

    static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        tImageSource *isource = (tImageSource *)png_get_io_ptr(png_ptr);

        if ((int)(isource->offset + length) <= isource->size)
        {
            memcpy(data, isource->data+isource->offset, length);
            isource->offset += length;
        }
        else
        {
            png_error(png_ptr, "pngReaderCallback failed");
        }
    }

    ImageCodecPNGPtr ImageCodecPNG::create()
    {
        ImageCodecPNGPtr codec = new ImageCodecPNG();
        codec->release();
        return codec;
    }

    ImageCodecPNG::ImageCodecPNG()
    {

    }

    ImageCodecPNG::~ImageCodecPNG()
    {

    }

    ImageCodecBase::FileType ImageCodecPNG::getFileType() const
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
        size_t size = 0;
        uint8_t *data = nullptr;
        bool ret = encode(data, size, image);
        if (ret)
        {
            stream.write(data, size);
        }        
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

    bool ImageCodecPNG::decode(const uint8_t *rawData, size_t rawSize, Image &image)
    {
        // length of bytes to check if it is a valid png file
        const size_t PNGSIGSIZE = 8;
        bool ret = false;

        png_byte        header[PNGSIGSIZE]   = {0}; 
        png_structp     png_ptr = 0;
        png_infop       info_ptr = 0;

        int32_t width = 0;
        int32_t height = 0;
        int32_t pitch = 0;
        int32_t bpp = 0;
        uint8_t *data = nullptr;
        size_t size = 0;
        bool hasAlpha = false;
        bool isPreMulti = false;
        PixelFormat eFormat = E_PF_A8R8G8B8;

        do 
        {
            // png header len is 8 bytes
            if (rawSize < PNGSIGSIZE)
                break;

            // check the data is png or not
            memcpy(header, rawData, PNGSIGSIZE);
            if (png_sig_cmp(header, 0, PNGSIGSIZE))
                break;

            // init png_struct
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
            if (! png_ptr)
                break;

            // init png_info
            info_ptr = png_create_info_struct(png_ptr);
            if (!info_ptr)
                break;

            // set the read call back function
            tImageSource imageSource;
            imageSource.data    = (uint8_t*)rawData;
            imageSource.size    = rawSize;
            imageSource.offset  = 0;
            png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

            // read png header info

            // read png file info
            png_read_info(png_ptr, info_ptr);

            width = png_get_image_width(png_ptr, info_ptr);
            height = png_get_image_height(png_ptr, info_ptr);
            bpp = png_get_bit_depth(png_ptr, info_ptr);
            png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

            //CCLOG("color type %u", color_type);

            // force palette images to be expanded to 24-bit RGB
            // it may include alpha channel
            if (color_type == PNG_COLOR_TYPE_PALETTE)
            {
                png_set_palette_to_rgb(png_ptr);
            }
            // low-bit-depth grayscale images are to be expanded to 8 bits
            if (color_type == PNG_COLOR_TYPE_GRAY && bpp < 8)
            {
                png_set_expand_gray_1_2_4_to_8(png_ptr);
            }
            // expand any tRNS chunk data into a full alpha channel
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(png_ptr);
            }  
            // reduce images with 16-bit samples to 8 bits
            if (bpp == 16)
            {
                png_set_strip_16(png_ptr);            
            } 
            // expand grayscale images to RGB
            if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            {
                png_set_gray_to_rgb(png_ptr);
            }

            // read png data
            // m_nBitsPerComponent will always be 8
//             bpp = 8;
            png_uint_32 rowbytes;
            png_bytep* row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * height );

            png_read_update_info(png_ptr, info_ptr);

            rowbytes = png_get_rowbytes(png_ptr, info_ptr);

            pitch = rowbytes;
            size = rowbytes * height;
            data = new uint8_t[size];
            if (!data)
                break;

            for (uint16_t i = 0; i < height; ++i)
            {
                row_pointers[i] = data + i*rowbytes;
            }
            png_read_image(png_ptr, row_pointers);

            png_read_end(png_ptr, NULL);

            png_uint_32 channel = rowbytes / width;
            if (channel == 4)
            {
                hasAlpha = true;
                uint32_t *tmp = (uint32_t *)data;
                for (uint16_t i = 0; i < height; i++)
                {
                    for(uint32_t j = 0; j < rowbytes; j += 4)
                    {
                        *tmp++ = T3D_RGB_PREMULTIPLY_ALPHA( row_pointers[i][j], row_pointers[i][j + 1], 
                            row_pointers[i][j + 2], row_pointers[i][j + 3] );
                    }
                }

                isPreMulti = true;
            }

            T3D_SAFE_DELETE_ARRAY(row_pointers);

            ret = true;
        } while (0);

        if (png_ptr)
        {
            png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
        }

        if (ret)
        {
            setImageDimension(image, width, height, pitch);
            setImageData(image, data, size);
            setImageInfo(image, bpp, hasAlpha, isPreMulti, eFormat);
        }

        return ret;
    }
}
