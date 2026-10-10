/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "T3DColor4.h"


namespace Tiny3D
{
    const Color4 Color4::BLACK(0, 0, 0);
    const Color4 Color4::WHITE(255, 255, 255);
    const Color4 Color4::RED(255, 0, 0);
    const Color4 Color4::GREEN(0, 255, 0);
    const Color4 Color4::BLUE(0, 0, 255);
    const Color4 Color4::YELLOW(255, 255, 0);

    const uint16_t Color4::RGB555_RED_MASK = 0x7C00;
    const uint16_t Color4::RGB555_GREEN_MASK = 0x03E0;
    const uint16_t Color4::RGB555_BLUE_MASK = 0x001F;
    const uint16_t Color4::RGB555_ALPHA_MASK = 0x8000;

    const uint16_t Color4::RGB565_RED_MASK = 0xF800;
    const uint16_t Color4::RGB565_GREEN_MASK = 0x07E0;
    const uint16_t Color4::RGB565_BLUE_MASK = 0x001F;

    const uint16_t Color4::RGB444_RED_MASK = 0x0F00;
    const uint16_t Color4::RGB444_GREEN_MASK = 0x00F0;
    const uint16_t Color4::RGB444_BLUE_MASK = 0x000F;
    const uint16_t Color4::RGB444_ALPHA_MASK = 0xF000;

    const uint32_t Color4::RGB_RED_MASK = 0x00FF0000;
    const uint32_t Color4::RGB_GREEN_MASK = 0x0000FF00;
    const uint32_t Color4::RGB_BLUE_MASK = 0x000000FF;
    const uint32_t Color4::RGB_ALPHA_MASK = 0xFF000000;
}
