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
#include "RHI/T3DRHIRenderWindow.h"
#include "RHI/T3DRHIContext.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    RenderWindowPtr RenderWindow::create(const String &name, const RenderWindowDesc &desc) 
    {
        RenderWindowPtr window = new RenderWindow(name);
        if (window != nullptr)
        {
            TResult ret = window->init(desc);
            if (T3D_FAILED(ret))
            {
                window = nullptr;
            }
        }
        return window;
    }

    //--------------------------------------------------------------------------

    RenderWindow::RenderWindow(const String &name)
        : RenderTarget(name)
    {

    }

    //--------------------------------------------------------------------------

    RenderWindow::~RenderWindow()
    {
        destroy();
    }

    //--------------------------------------------------------------------------

    RenderWindow::Type RenderWindow::getType() const
    {
        return Type::E_RT_WINDOW;
    }

    //--------------------------------------------------------------------------

    TResult RenderWindow::init(const RenderWindowDesc &desc)
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
            void *externalWnd = desc.externalHandle;
            if (externalWnd != nullptr)
            {
                // 外部创建的窗口，这里关联上
                ret = mWindow->createFrom(externalWnd);
            }
            else
            {
                // 自己创建窗口对象
                uint32_t flags = Window::WINDOW_SHOWN;
                if (desc.IsFullscreen)
                {
                    flags |= Window::WINDOW_FULLSCREEN;
                }

                String title = desc.Title + " - " + T3D_AGENT.getActiveRHIRenderer()->getName();
                ret = mWindow->create(title.c_str(), desc.Left, desc.Top, desc.Width, desc.Height, flags);
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_ENGINE, 
                        "Create native window failed !");
                    break;
                }
            }

            // 加载图标
            ret = loadIcon(desc.IconPath);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // mPitch = Image::calcPitch(mWidth, mColorDepth);

            // ret = setupD3D11Environment(param, paramEx);
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }

            mDesc = desc;

            mRHIRenderTarget = T3D_AGENT.getActiveRHIContext()->createRenderWindow(this);
            // mRHIRenderTarget = T3D_AGENT.getActiveRHIContext()->createRenderWindow(this);
            //
            // ViewportPtr viewport = addViewport(0, 0, 0, 0.5f, 1.0f);
            // viewport = addViewport(1, 0.5f, 0, 0.5f, 1.0f);
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

            mRHIRenderTarget = nullptr;
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
            if (mRHIRenderTarget == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER, "RHI render window has not created !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }
            
            RHIRenderWindowPtr rw = smart_pointer_cast<RHIRenderWindow>(mRHIRenderTarget);
            ret = rw->swapBuffers();
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

    bool RenderWindow::getSystemInfo(SysWMInfo &info) const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getSystemInfo(info);
        }
        return false;
    }

    //--------------------------------------------------------------------------

    void RenderWindow::getMetrics(int32_t &width, int32_t &height, int32_t &clrDepth) const
    {
        width = mDesc.Width;
        height = mDesc.Height;
        clrDepth = mDesc.ColorDepth;
    }

    //--------------------------------------------------------------------------

    int32_t RenderWindow::getWidth() const
    {
        return mDesc.Width;
    }

    //--------------------------------------------------------------------------

    int32_t RenderWindow::getHeight() const
    {
        return mDesc.Height;
    }

    //--------------------------------------------------------------------------

    int32_t RenderWindow::getColorDepth() const
    {
        return mDesc.ColorDepth;
    }

    //--------------------------------------------------------------------------
}

