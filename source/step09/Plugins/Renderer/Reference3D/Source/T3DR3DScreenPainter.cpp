/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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
 ******************************************************************************/


#include "T3DR3DScreenPainter.h"
#include "T3DR3DRenderWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(R3DScreenPainter);

    //--------------------------------------------------------------------------

    R3DScreenPainter::R3DScreenPainter(R3DRenderWindowPtr window)
        : mWindow(window)
    {
        
    }

    //--------------------------------------------------------------------------

    R3DScreenPainter::~R3DScreenPainter()
    {

    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawPoint(const Point &point, const Color4 &color)
    {
        uint8_t *fb = mWindow->getFramebuffer();
        size_t pitch = mWindow->getPitch();
        size_t bytesPerPixel = (mWindow->getColorDepth() >> 3);
        fb = fb + point.y * pitch + point.x * bytesPerPixel;
        *fb++ = color.blue();
        *fb++ = color.green();
        *fb++ = color.red();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawLine(const Point &start, const Point &end,
        const Color4 &color, size_t border /* = 1 */)
    {
        TResult ret = T3D_OK;
        switch (mWindow->getColorDepth())
        {
        case 24:
            ret = drawLine24(start, end, color);
            break;
        case 32:
            ret = drawLine32(start, end, color);
            break;
        default:
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawGradualLine(const Point &start,
        const Point &end, const Color4 &clrStart, const Color4 &clrEnd,
        size_t border /* = 1 */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawTriangle(const Point &p1, const Point &p2,
        const Point &p3, const Color4 &color, size_t border /* = 1 */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawSolidTriangle(const Point &p1,
        const Point &p2, const Point &p3, const Color4 &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawGradualTriangle(
        const Point &p1, const Color4 &clr1,
        const Point &p2, const Color4 &clr2,
        const Point &p3, const Color4 &clr3)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawRect(const Rect &rect, const Color4 &color,
        size_t border /* = 1 */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawSolidRect(const Rect &rect,
        const Color4 &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawLine24(const Point &start, const Point &end,
        const Color4 &color)
    {
        TResult ret = T3D_OK;

        uint8_t *fb = mWindow->getFramebuffer();
        size_t fbSize = mWindow->getFramebufferSize();
        size_t pitch = mWindow->getPitch();
        size_t bytesPerPixel = (mWindow->getColorDepth() >> 3);

        int32_t w = mWindow->getWidth();
        int32_t h = mWindow->getHeight();

        int32_t x1 = start.x;
        int32_t y1 = start.y;
        int32_t x2 = end.x;
        int32_t y2 = end.y;

        // 裁减掉超出屏幕的部分
        if (x1 < 0)
            x1 = 0;
        else if (x1 >= w)
            x1 = w - 1;

        if (y1 < 0)
            y1 = 0;
        else if (y1 >= h)
            y1 = h - 1;

        if (x2 < 0)
            x2 = 0;
        else if (x2 >= w)
            x2 = w - 1;

        if (y2 < 0)
            y2 = 0;
        else if (y2 >= h)
            y2 = h - 1;

        int32_t dx = x2 - x1;
        int32_t dy = y2 - y1;

        int32_t error = 0;

        int32_t x_inc = 0;
        int32_t y_inc = 0;

        if (dx > 0)
        {
            // X-axis 正方向
            x_inc = bytesPerPixel;
        }
        else
        {
            // X-axis 负方向
            x_inc = -bytesPerPixel;
            dx = -dx;
        }

        if (dy > 0)
        {
            // Y-axis 正方向
            y_inc = pitch;
        }
        else
        {
            // Y-axis 负方向
            y_inc = -pitch;
            dy = -dy;
        }

        int32_t dx2 = dx << 1;
        int32_t dy2 = dy << 1;

        int32_t i = 0;

        // 寻址到开始地址
        fb = fb + x1 * bytesPerPixel + y1 * pitch;

        if (dx > dy)
        {
            error = dy2 - dx;

            for (i = 0; i < dx; ++i)
            {
                fb[0] = color.blue();
                fb[1] = color.green();
                fb[2] = color.red();

                if (error >= 0)
                {
                    // 调整误差，跳到下一行
                    error -= dx2;
                    fb += y_inc;
                }

                error += dy2;
                fb += x_inc;
            }
        }
        else
        {
            error = dx2 - dy;

            for (i = 0; i < dy; ++i)
            {
                fb[0] = color.blue();
                fb[1] = color.green();
                fb[2] = color.red();

                if (error >= 0)
                {
                    error -= dy2;
                    fb += x_inc;
                }

                error += dx2;
                fb += y_inc;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawLine32(const Point &start, const Point &end, 
        const Color4 &color)
    {
        TResult ret = T3D_OK;
        
        uint8_t *fb = mWindow->getFramebuffer();
        size_t fbSize = mWindow->getFramebufferSize();
        size_t pitch = mWindow->getPitch();
        size_t bytesPerPixel = (mWindow->getColorDepth() >> 3);

        int32_t w = mWindow->getWidth();
        int32_t h = mWindow->getHeight();

        int32_t x1 = start.x;
        int32_t y1 = start.y;
        int32_t x2 = end.x;
        int32_t y2 = end.y;

        // 裁减掉超出屏幕的部分
        if (x1 < 0)
            x1 = 0;
        else if (x1 >= w)
            x1 = w - 1;

        if (y1 < 0)
            y1 = 0;
        else if (y1 >= h)
            y1 = h - 1;

        if (x2 < 0)
            x2 = 0;
        else if (x2 >= w)
            x2 = w - 1;

        if (y2 < 0)
            y2 = 0;
        else if (y2 >= h)
            y2 = h - 1;

        int32_t dx = x2 - x1;
        int32_t dy = y2 - y1;

        int32_t error = 0;

        int32_t x_inc = 0;
        int32_t y_inc = 0;

        if (dx > 0)
        {
            // X-axis 正方向
            x_inc = bytesPerPixel;
        }
        else
        {
            // X-axis 负方向
            x_inc = -bytesPerPixel;
            dx = -dx;
        }

        if (dy > 0)
        {
            // Y-axis 正方向
            y_inc = pitch;
        }
        else
        {
            // Y-axis 负方向
            y_inc = -pitch;
            dy = -dy;
        }

        int32_t dx2 = dx << 1;
        int32_t dy2 = dy << 1;

        int32_t i = 0;

        // 寻址到开始地址
        fb = fb + x1 * bytesPerPixel + y1 * pitch;

        if (dx > dy)
        {
            error = dy2 - dx;

            for (i = 0; i < dx; ++i)
            {
                fb[0] = color.blue();
                fb[1] = color.green();
                fb[2] = color.red();
                fb[3] = 0xFF;

                if (error >= 0)
                {
                    // 调整误差，跳到下一行
                    error -= dx2;
                    fb += y_inc;
                }

                error += dy2;
                fb += x_inc;
            }
        }
        else
        {
            error = dx2 - dy;

            for (i = 0; i < dy; ++i)
            {
                fb[0] = color.blue();
                fb[1] = color.green();
                fb[2] = color.red();
                fb[3] = 0xFF;

                if (error >= 0)
                {
                    error -= dy2;
                    fb += x_inc;
                }

                error += dx2;
                fb += y_inc;
            }
        }

        return ret;
    }
}
