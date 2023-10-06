/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#ifndef __T3D_D3D11_WINDOW_H__
#define __T3D_D3D11_WINDOW_H__


#include "T3DNullPrerequisites.h"


namespace Tiny3D
{
    class NullWindow : public RHIRenderWindow
    {
    public:
        /**
         * @brief 创建渲染窗口对象
         * @param [in] name : 渲染窗口名字
         * @return 返回一个渲染窗口对象
         */
        static NullWindowPtr create();

        /**
         * @fn  virtual NullRenderWindow::~NullRenderWindow();
         * @brief   析构函数
         */
        virtual ~NullWindow();

        /**
         * @fn  virtual TResult NullRenderWindow::swapBuffers() override;
         * @brief   双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return  调用成功返回 T3D_OK.
         * @remarks  具体渲染系统子类实现本接口.
         */
        TResult swapBuffers() override;

        TResult beginRender() override { return T3D_OK; }

        TResult endRender() override { return T3D_OK; }

        /**
         * @fn  virtual void NullRenderWindow::clear(const ColorRGB &clrFill, 
         *      uint32_t clearFlags, Real depth, uint32_t stencil) override;
         * @brief   Clears this object to its blank/initial state
         * @param   clrFill     The color fill.
         * @param   clearFlags  The clear flags.
         * @param   depth       The depth.
         * @param   stencil     The stencil.
         */
        TResult clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil) override;

    protected:
        /**
         * @fn  NullRenderWindow::NullRenderWindow(const String &name);
         * @brief   Constructor
         * @param   name    The name.
         */
        NullWindow();
    };
}



#endif  /*__T3D_NULL_WINDOW_H__*/
