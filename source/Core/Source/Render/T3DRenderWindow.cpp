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


#include "Render/T3DRenderWindow.h"
#include "T3DErrorDef.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    RenderWindowPtr RenderWindow::create(const String &name) 
    {
        return new RenderWindow(name);
    }

    //--------------------------------------------------------------------------

    RenderWindow::RenderWindow(const String &name)
        : RenderTarget(name)
    {

    }

    //--------------------------------------------------------------------------

    RenderWindow::~RenderWindow()
    {

    }

    //--------------------------------------------------------------------------

    RenderWindow::Type RenderWindow::getType() const
    {
        return E_RT_WINDOW;
    }

    //--------------------------------------------------------------------------

    TResult RenderWindow::create(const RenderWindowCreateParam &param)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mWindow != nullptr)
            {
                // 窗口已经创建
                ret = T3D_ERR_RENDER_WINDOW_ALREADY;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, 
                    "Render window already created !");
                break;
            }

            mWindow = new Window();

            // 创建窗口
            void *externalWnd = param.externalHandle;
            if (externalWnd != nullptr)
            {
                // 外部创建的窗口，这里关联上
                ret = mWindow->createFrom(externalWnd);
            }
            else
            {
                // 自己创建窗口对象
                uint32_t flags = Window::WINDOW_SHOWN;
                if (param.fullscreen)
                {
                    flags |= Window::WINDOW_FULLSCREEN;
                }

                String title = param.windowTitle + " - " 
                    + T3D_AGENT.getActiveRHIRenderer()->getName();
                ret = mWindow->create(title.c_str(),
                    param.windowLeft, param.windowTop,
                    param.windowWidth, param.windowHeight, flags);
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_ENGINE, 
                        "Create native window failed !");
                    break;
                }
            }

            // 加载图标
            ret = loadIcon(param.iconPath);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mWidth = param.windowWidth;
            mHeight = param.windowHeight;
            mColorDepth = mWindow->getColorDepth();
            mIsFullscreen = param.fullscreen;
            // mPitch = Image::calcPitch(mWidth, mColorDepth);

            // ret = setupD3D11Environment(param, paramEx);
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }

            mRHIRenderWindow = T3D_AGENT.getActiveRHIContext()->createRenderWindow(this, param);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RenderWindow::destroy()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mWindow == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid window pointer !");
                break;
            }
            
            mWindow->destroy();

            T3D_SAFE_DELETE(mWindow);

            ret = T3D_OK;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RenderWindow::swapBuffers()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mRHIRenderWindow == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER, "RHI render window has not created !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = mRHIRenderWindow->swapBuffers();
        } while (false);
       
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RenderWindow::loadIcon(const String &iconPath)
    {
        TResult ret = T3D_OK;

        do 
        {
            // // 加载图标资源
            // Image image;
            // ret = image.load(iconPath);
            // if (T3D_FAILED(ret))
            // {
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, 
            //         "Load icon image [%s] failed !", iconPath.c_str());
            //     break;
            // }
            //
            // // 设置窗口图标
            // Window::WindowIcon icon;
            // icon.pixels = image.getData();
            // icon.width = image.getWidth();
            // icon.height = image.getHeight();
            // icon.depth = image.getBPP();
            // icon.pitch = image.getPitch();
            //
            // switch (image.getFormat())
            // {
            // case PixelFormat::E_PF_PALETTE8:
            //     icon.format = Window::PIXEL_FORMAT_INDEX8;
            //     break;
            // case PixelFormat::E_PF_A1R5G5B5:
            //     icon.format = Window::PIXEL_FORMAT_ARGB1555;
            //     break;
            // case PixelFormat::E_PF_A4R4G4B4:
            //     icon.format = Window::PIXEL_FORMAT_ARGB4444;
            //     break;
            // case PixelFormat::E_PF_R5G6B5:
            //     icon.format = Window::PIXEL_FORMAT_RGB565;
            //     break;
            // case PixelFormat::E_PF_R8G8B8:
            //     icon.format = Window::PIXEL_FORMAT_RGB24;
            //     break;
            // case PixelFormat::E_PF_A8R8G8B8:
            //     icon.format = Window::PIXEL_FORMAT_ARGB8888;
            //     break;
            // default:
            //     break;
            // }
            //
            // if (T3D_FAILED(ret))
            // {
            //     ret = T3D_ERR_D3D11_UNSUPPORT_FORMAT_ICON;
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, 
            //         "Do not support icon [%s] format !", iconPath.c_str());
            //     break;
            // }
            //
            // mWindow->setWindowIcon(icon);
        } while (0);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool RenderWindow::isFullscreen() const
    {
        return true;
    }

    //--------------------------------------------------------------------------

    void RenderWindow::render()
    {
        RenderTarget::render();
        swapBuffers();
    }

    //--------------------------------------------------------------------------
    
    TResult RenderWindow::clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil) 
    {
        TResult ret = T3D_OK;

        do
        {
            if (mRHIRenderWindow == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER, "RHI render window has not created !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = mRHIRenderWindow->clear(clrFill, clearFlags, depth, stencil);
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool RenderWindow::getSystemInfo(SysWMInfo &info) const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getSystemInfo(info);
        }
        return false;
    }

    //--------------------------------------------------------------------------
}

