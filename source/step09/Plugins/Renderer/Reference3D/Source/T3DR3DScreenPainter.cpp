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

    inline void drawPointFast(const Point &pt, const Color4 &color, uint8_t *fb,
        size_t pitch, size_t bytesPerPixel)
    {
        fb = fb + pt.y * pitch + pt.x * bytesPerPixel;
        *fb++ = color.blue();
        *fb++ = color.green();
        *fb++ = color.red();
    }

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
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawLine(const Point &start, const Point &end,
        const Color4 &color, size_t border /* = 1 */)
    {
        return drawLineByDoubleStep(start, end, color);
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

    TResult R3DScreenPainter::drawLineByBresenham(const Point &start,
        const Point &end, const Color4 &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DScreenPainter::drawLineByDoubleStep(const Point &start,
        const Point &end, const Color4 &color)
    {
        TResult ret = T3D_OK;

        int32_t dx = end.x - start.x;
        int32_t dy = end.y - start.y;
        int32_t error = (dy << 2) - dx;
        int32_t w = mWindow->getWidth();
        int32_t h = mWindow->getHeight();

        // 裁减掉超出屏幕的部分
        int32_t x1 = (start.x < 0 ? 0 : start.x);
        int32_t y1 = (start.y < 0 ? 0 : start.y);
        int32_t x2 = (end.x >= w ? w - 1 : end.x);
        int32_t y2 = (end.y >= h ? h - 1 : end.y);

        Point pt(x1, y1);

        uint8_t *fb = mWindow->getFramebuffer();
        size_t fbSize = mWindow->getFramebufferSize();
        size_t pitch = mWindow->getPitch();
        size_t bytesPerPixel = (mWindow->getColorDepth() >> 3);

        // 绘制起点
        drawPointFast(pt, color, fb, pitch, bytesPerPixel);

        while (pt.x < x2)
        {
            if (error > dx)
            {
                if (error > (dx << 1))  // error < dx * 4
                {
                    error += ((dy - dx) << 2);  // error += (dy - dx) * 4
                    ++pt.x;
                    ++pt.y;
                    // 填充颜色
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                    // 跳到下一像素
                    ++pt.x;
                    ++pt.y;
                    // 填充颜色
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                }
                else
                {
                    error += ((dy << 2) - (dx << 1));
                    ++pt.x;
                    ++pt.y;
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                    ++pt.x;
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                }
            }
            else
            {
                if (error > 0)
                {
                    error += ((dy << 2) - (dx << 1));
                    ++pt.x;
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                    ++pt.x;
                    ++pt.y;
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                }
                else
                {
                    ++pt.x;
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                    ++pt.x;
                    drawPointFast(pt, color, fb, pitch, bytesPerPixel);
                    error += (dy << 2);
                }
            }
        }

        return ret;
    }
}
