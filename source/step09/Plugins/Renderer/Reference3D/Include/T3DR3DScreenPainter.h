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


#ifndef __T3D_R3D_SCREEN_PAINTER_H__
#define __T3D_R3D_SCREEN_PAINTER_H__


#include "T3DR3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 屏幕绘制类
     */
    class R3DScreenPainter : public Singleton<R3DScreenPainter>
    {
    public:
        /**
         * @brief 构造函数
         */
        R3DScreenPainter(R3DRenderWindowPtr window);
        
        /**
         * @brief 析构函数
         */
        virtual ~R3DScreenPainter();

        /**
         * @brief 绘制点
         * @param [in] point : 点屏幕坐标
         * @param [in] color : 点颜色
         * @return 调用成功返回 T3D_OK
         */
        TResult drawPoint(const Point &point, const Color4f &color);

        /**
         * @brief 绘制直线
         * @param [in] start : 起点屏幕坐标
         * @param [in] end : 终点屏幕坐标
         * @param [in] color : 直线颜色
         * @param [in] border : 直线大小，默认1像素
         * @return 调用成功返回 T3D_OK
         */
        TResult drawLine(const Point &start, const Point &end, 
            const Color4f &color, size_t border = 1);

        /**
         * @brief 绘制渐变直线
         * @param [in] start : 起点屏幕坐标
         * @param [in] end : 终点屏幕坐标
         * @param [in] clrStart : 起点颜色
         * @param [in] clrEnd : 终点颜色
         * @param [in] border : 直线大小，默认1像素
         * @return 调用成功返回 T3D_OK
         */
        TResult drawGradualLine(const Point &start, const Point &end,
            const Color4f &clrStart, const Color4f &clrEnd, size_t border = 1);

        /**
         * @brief 绘制空心三角形
         * @param [in] p1 : 三角形顶点
         * @param [in] p2 : 三角形顶点
         * @param [in] p3 : 三角形顶点
         * @param [in] color : 边的颜色
         * @param [in] border : 边框大小，默认1像素
         * @return 调用成功返回 T3D_OK
         */
        TResult drawTriangle(const Point &p1, const Point &p2, const Point &p3,
            const Color4f &color, size_t border = 1);

        /**
         * @brief 绘制实心三角形
         * @param [in] p1 : 三角形顶点
         * @param [in] p2 : 三角形顶点
         * @param [in] p3 : 三角形顶点
         * @param [in] color : 填充的颜色
         * @return 调用成功返回 T3D_OK
         */
        TResult drawSolidTriangle(const Point &p1, const Point &p2,
            const Point &p3, const Color4f &color);

        /**
         * @brief 绘制渐变三角形
         * @param [in] p1 : 三角形顶点
         * @param [in] clr1 : 三角形颜色
         * @param [in] p2 : 三角形顶点
         * @param [in] clr2 : 三角形颜色
         * @param [in] p3 : 三角形顶点
         * @param [in] clr3 : 三角形颜色
         * @return 调用成功返回 T3D_OK
         */
        TResult drawGradualTriangle(const Point &p1, const Color4f &clr1,
            const Point &p2, const Color4f &clr2,
            const Point &p3, const Color4f &clr3);

        /**
         * @brief 绘制空心矩形
         * @param [in] rect : 矩形区域
         * @param [in] color : 矩形边框颜色
         * @param [in] border : 边框大小
         * @return 调用成功返回 T3D_OK
         */
        TResult drawRect(const Rect &rect, const Color4f &color,
            size_t border = 1);

        /**
         * @brief 绘制实心矩形
         * @param [in] rect : 矩形区域
         * @param [in] color : 矩形颜色
         * @return 调用成功返回 T3D_OK
         */
        TResult drawSolidRect(const Rect &rect, const Color4f &color);

    protected:
        /**
         * @brief 24位色深窗口上基于 Bresenham 算法绘制直线
         */
        TResult drawLine24(const Point &start, const Point &end,
            const Color4f &color);

        /**
         * @brief 32位色深窗口上基于 Bresenham 算法绘制直线
         */
        TResult drawLine32(const Point &start, const Point &end,
            const Color4f &color);

    protected:
        R3DRenderWindowPtr  mWindow;
    };

    #define SCREEN_PAINTER      R3DScreenPainter::getInstance()
}


#endif  /*__T3D_R3D_SCREEN_PAINTER_H__*/
