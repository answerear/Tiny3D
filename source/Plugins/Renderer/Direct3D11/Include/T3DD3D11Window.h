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


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @class   D3D11RenderWindow
     * @brief   DirectX 11 渲染窗口.
     */
    class D3D11Window : public RHIRenderWindow
    {
    public:
        /**
         * @brief 创建渲染窗口对象
         * @return 返回一个渲染窗口对象
         */
        static D3D11WindowPtr create(RenderWindow *renderWindow, const RenderWindowCreateParam &param);

        /**
         * @fn  virtual D3D11RenderWindow::~D3D11RenderWindow();
         * @brief   析构函数
         */
        virtual ~D3D11Window();

        bool init(RenderWindow *renderWindow, const RenderWindowCreateParam &param);

        /**
         * @fn  virtual TResult D3D11RenderWindow::swapBuffers() override;
         * @brief   双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return  调用成功返回 T3D_OK.
         * @remarks  具体渲染系统子类实现本接口.
         */
        TResult swapBuffers() override;

        TResult beginRender() override;

        TResult endRender() override;

        /**
         * @fn  virtual void D3D11RenderWindow::clear(const ColorRGB &clrFill, 
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
         * @fn  D3D11RenderWindow::D3D11RenderWindow(const String &name);
         * @brief   Constructor
         */
        D3D11Window();

        IDXGISwapChain          *mD3DSwapChain; /**< The d 3D swap chain */
        ID3D11RenderTargetView  *mD3DRTView;    /**< The d 3D right view */
        ID3D11DepthStencilView  *mD3DDSView;    /**< The d 3D ds view */
    };
}



#endif  /*__T3D_D3D11_WINDOW_H__*/
