/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __T3D_ANDROID_WINDOW_H__
#define __T3D_ANDROID_WINDOW_H__


#include "Adapter/T3DWindowInterface.h"


namespace Tiny3D
{
    class AndroidWindow : public IWindow
    {
    public:
        AndroidWindow();
        virtual ~AndroidWindow();

        /**
         * @brief 创建窗口.
         * @param [in] x : 窗口位置
         * @param [in] y : 窗口位置
         * @param [in] w : 窗口宽度
         * @param [in] h : 窗口高度
         * @param [in] isFullscreen : 是否全屏
         * @param [in] argc : 窗口附加参数数量
         * @param [in] args : 附加参数列表
         * @return 创建成功返回true，否则返回false.
         * @remarks 各平台各自定义argc的值和args的内容
         */
        virtual bool create(const char *title, int32_t x, int32_t y,
            int32_t w, int32_t h, bool isFullscreen, 
            int32_t argc, va_list args) override;

        /**
         * @brief 销毁窗口
         * @return void
         */
        virtual void destroy() override;

        /**
         * @brief 返回原生窗口对象
         * @return 返回平台原生窗口对象或者句柄
         * @remarks 不同平台返回不同的对象，根据各自平台各自解析
         */
        virtual void *getNativeWinObject() override;
    };
}


#endif  /*__T3D_ANDROID_WINDOW_H__*/
