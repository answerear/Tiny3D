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


#include "T3DR3DFramebuffer.h"
#include "T3DR3DRenderWindow.h"
#include "T3DR3DError.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DFramebufferPtr R3DFramebuffer::create(RenderTargetPtr target)
    {
        R3DFramebufferPtr fbo = new R3DFramebuffer();
        fbo->release();

        if (fbo->init(target) != T3D_OK)
        {
            fbo = nullptr;
        }

        return fbo;
    }

    //--------------------------------------------------------------------------

    R3DFramebuffer::R3DFramebuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    R3DFramebuffer::~R3DFramebuffer()
    {

    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::init(RenderTargetPtr target)
    {
        TResult ret = T3D_OK;

        switch (target->getType())
        {
        case RenderTarget::E_RT_WINDOW:
            {
                R3DRenderWindowPtr window 
                    = smart_pointer_cast<R3DRenderWindow>(target);
                mFramebuffer = window->getFramebuffer();
                mFramebufferSize = window->getFramebufferSize();
                mWidth = window->getWidth();
                mHeight = window->getHeight();
                mColorDepth = window->getColorDepth();
                mPitch = window->getPitch();
            }
            break;
        case RenderTarget::E_RT_TEXTURE:
            break;
        default:
            ret = T3D_ERR_R3D_INVALID_TARGET;
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawPoint(const Point &point, const Color4f &color)
    {
        uint8_t *fb = mFramebuffer;
        size_t pitch = mPitch;
        size_t bytesPerPixel = (mColorDepth >> 3);
        fb = fb + point.y * pitch + point.x * bytesPerPixel;
        Color4 clr;
        clr.from(color);
        *fb++ = clr.blue();
        *fb++ = clr.green();
        *fb++ = clr.red();
        if (bytesPerPixel == 4)
            *fb++ = 0xFF;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawLine(const Point &start, const Point &end,
        const Color4f &color, size_t border /* = 1 */)
    {
        TResult ret = T3D_OK;
        switch (mColorDepth)
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

    TResult R3DFramebuffer::drawGradualLine(const Point &start,
        const Point &end, const Color4f &clrStart, const Color4f &clrEnd,
        size_t border /* = 1 */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawTriangle(const Point &p1, const Point &p2,
        const Point &p3, const Color4f &color, size_t border /* = 1 */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawSolidTriangle(const Point &p1,
        const Point &p2, const Point &p3, const Color4f &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawGradualTriangle(
        const Point &p1, const Color4f &clr1,
        const Point &p2, const Color4f &clr2,
        const Point &p3, const Color4f &clr3)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawRect(const Rect &rect, const Color4f &color,
        size_t border /* = 1 */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawSolidRect(const Rect &rect,
        const Color4f &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DFramebuffer::drawLine24(const Point &start, const Point &end,
        const Color4f &color)
    {
        TResult ret = T3D_OK;

        Color4 clr;
        clr.from(color);

        uint8_t *fb = mFramebuffer;
        size_t fbSize = mFramebufferSize;
        size_t pitch = mPitch;
        size_t bytesPerPixel = (mColorDepth >> 3);

        int32_t w = mWidth;
        int32_t h = mHeight;

        int32_t x1 = start.x;
        int32_t y1 = start.y;
        int32_t x2 = end.x;
        int32_t y2 = end.y;

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
                fb[0] = clr.blue();
                fb[1] = clr.green();
                fb[2] = clr.red();

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
                fb[0] = clr.blue();
                fb[1] = clr.green();
                fb[2] = clr.red();

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

    TResult R3DFramebuffer::drawLine32(const Point &start, const Point &end, 
        const Color4f &color)
    {
        TResult ret = T3D_OK;
        
        Color4 clr;
        clr.from(color);

        uint8_t *fb = mFramebuffer;
        size_t fbSize = mFramebufferSize;
        size_t pitch = mPitch;
        size_t bytesPerPixel = (mColorDepth >> 3);

        int32_t w = mWidth;
        int32_t h = mHeight;

        int32_t x1 = start.x;
        int32_t y1 = start.y;
        int32_t x2 = end.x;
        int32_t y2 = end.y;

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
                fb[0] = clr.blue();
                fb[1] = clr.green();
                fb[2] = clr.red();
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
                fb[0] = clr.blue();
                fb[1] = clr.green();
                fb[2] = clr.red();
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
