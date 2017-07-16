

#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    const Color4 Color4::BLACK(0, 0, 0);
    const Color4 Color4::WHITE(255, 255, 255);
    const Color4 Color4::RED(255, 0, 0);
    const Color4 Color4::GREEN(0, 255, 0);
    const Color4 Color4::BLUE(0, 0, 255);

    void Color4::convert_B8G8R8A8toA8R8G8B8(void *src, void *dst, size_t count)
    {
        uint8_t *pSrc = (uint8_t*)src;
        uint8_t *pDst = (uint8_t*)dst;

        size_t x = 8;
        for (x = 0; x < count; x++)
        {
            pDst[0] = pSrc[3];
            pDst[1] = pSrc[2];
            pDst[2] = pSrc[1];
            pDst[3] = pSrc[0];

            pSrc += 4;
            pDst += 4;
        }
    }

    void Color4::convert(void *srcPixel, PixelFormat srcFmt, void *dstPixel, PixelFormat dstFmt)
    {
        switch (srcFmt)
        {
        case E_PF_R8G8B8:
            {
                uint8_t *src = (uint8_t *)srcPixel;
                uint8_t b = *src++;
                uint8_t g = *src++;
                uint8_t r = *src++;

                if (dstFmt == E_PF_A8R8G8B8 || dstFmt == E_PF_X8R8G8B8)
                {
                    uint8_t *dst = (uint8_t *)dstPixel;
                    *dst++ = b & 0xFF;
                    *dst++ = (g & 0xFF);
                    *dst++ = (r & 0xFF);
                    *dst++ = 0xFF;
                }
                else if (dstFmt == E_PF_B8G8R8A8 || dstFmt == E_PF_B8G8R8X8)
                {
                    uint8_t *dst = (uint8_t *)dstPixel;
                    *dst++ = (r & 0xFF);
                    *dst++ = (g & 0xFF);
                    *dst++ = (b & 0xFF);
                    *dst++ = 0xFF;
                }
            }
            break;
        }
    }
}
