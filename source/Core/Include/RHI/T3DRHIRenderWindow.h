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


#ifndef __T3D_RHI_RENDER_WINDOW_H__
#define __T3D_RHI_RENDER_WINDOW_H__


#include "Render/T3DRenderTarget.h"


namespace Tiny3D
{
    /**
     * @brief 渲染窗口
     */
    class T3D_ENGINE_API RHIRenderWindow : public Object
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~RHIRenderWindow();

        /**
         * @fn  virtual TResult D3D11RenderWindow::swapBuffers() override;
         * @brief   双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return  调用成功返回 T3D_OK.
         * @remarks  具体渲染系统子类实现本接口.
         */
        virtual TResult swapBuffers() = 0;

        /**
         * @fn  virtual void D3D11RenderWindow::clear(const ColorRGB &clrFill, 
         *      uint32_t clearFlags, Real depth, uint32_t stencil) override;
         * @brief   Clears this object to its blank/initial state
         * @param   clrFill     The color fill.
         * @param   clearFlags  The clear flags.
         * @param   depth       The depth.
         * @param   stencil     The stencil.
         */
        virtual void clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil) = 0;
        
    protected:
        /**
         * @brief 构造函数
         */
        RHIRenderWindow();
    };
}


#endif  /*__T3D_RHI_RENDER_WINDOW_H__*/
