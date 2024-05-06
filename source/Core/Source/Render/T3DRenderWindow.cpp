/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Render/T3DRenderWindow.h"
#include "Render/T3DRenderTarget.h"
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
        : mName(name)
    {

    }

    //--------------------------------------------------------------------------

    RenderWindow::~RenderWindow()
    {
        destroy();
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

            mRHIRenderWindow = T3D_AGENT.getActiveRHIContext()->createRenderWindow(this);
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

            mRHIRenderWindow = nullptr;
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

    bool RenderWindow::getSystemInfo(SysWMInfo &info) const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getSystemInfo(info);
        }
        return false;
    }

    //--------------------------------------------------------------------------

    void *RenderWindow::getNativeObject() const
    {
        if (mWindow != nullptr)
        {
            return mWindow->getNativeObject();
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------
}

