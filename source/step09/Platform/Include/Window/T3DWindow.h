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

#ifndef __T3D_WINDOW_H__
#define __T3D_WINDOW_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class IWindow;

    class T3D_PLATFORM_API Window
    {
        T3D_DISABLE_COPY(Window);

    public:
        static const uint32_t WINDOW_FULLSCREEN;
        static const uint32_t WINDOW_FULLSCREEN_DESKTOP;
        static const uint32_t WINDOW_OPENGL;
        static const uint32_t WINDOW_SHOWN;
        static const uint32_t WINDOW_HIDDEN;
        static const uint32_t WINDOW_BORDERLESS;
        static const uint32_t WINDOW_RESIZABLE;
        static const uint32_t WINDOW_MINIMIZED;
        static const uint32_t WINDOW_MAXIMIZED;
        static const uint32_t WINDOW_INPUT_GRABBED;
        static const uint32_t WINDOW_INPUT_FOCUS;
        static const uint32_t WINDOW_MOUSE_CAPTURE;

        /** Constructor */
        Window();

        /** Destructor */
        virtual ~Window();

        /**
         * @brief 创建窗口.
         * @param [in] x : 窗口位置
         * @param [in] y : 窗口位置
         * @param [in] w : 窗口宽度
         * @param [in] h : 窗口高度
         * @param [in] flags : 创建窗口需要的一些标记位，可使用或操作合并标记
         * @return 创建成功返回T3D_ERR_OK.
         */
        TResult create(const char *title, int32_t x, int32_t y,
            int32_t w, int32_t h, uint32_t flags);

        /**
         * @brief 销毁窗口.
         * @remarks 析构函数同样会自动调用
         */
        void destroy();

    protected:
        IWindow *mWindow;
    };
}


#endif  /*__T3D_WINDOW_H__*/