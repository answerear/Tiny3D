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

#ifndef __T3D_SDL_DESKTOP_WINDOW_H__
#define __T3D_SDL_DESKTOP_WINDOW_H__


#include "Adapter/T3DWindowInterface.h"
#include "T3DNoncopyable.h"
#include <SDL.h>


namespace Tiny3D
{
    class SDLDesktopWindow : public IWindow, public Noncopyable
    {
    public:
        SDLDesktopWindow();

        ~SDLDesktopWindow() override;

        /**
         * @brief 创建窗口.
         * @param [in] x : 窗口位置
         * @param [in] y : 窗口位置
         * @param [in] w : 窗口宽度
         * @param [in] h : 窗口高度
         * @param [in] flags : 创建窗口需要的一些标记位，可使用或操作合并标记
         * @return 创建成功返回T3D_OK.
         */
        TResult create(const char *title, int32_t x, int32_t y,
            int32_t w, int32_t h, uint32_t flags) override;

        /**
         * @brief 根据传入数据创建窗口
         * @param [in] data : 窗口依赖的数据指针
         * @return 成功返回 T3D_OK
         */
        TResult createFrom(const void *data) override;

        /**
         * @brief 销毁窗口
         * @return void
         */
        void destroy() override;

        /**
         * @brief 获取窗口系统相关信息
         * @param [in][out] info : 返回的窗口系统相关信息
         * @return 获取成功返回true
         */
        bool getSystemInfo(SysWMInfo &info) const override;

        /**
         * @brief 获取原生对象，这个视系统而定
         * @return 返回的原生窗口对象
         */
        void *getNativeObject() const override;
        
        /**
         * @brief 设置窗口图标
         * @param [in] pixels : 图标数据
         * @param [in] width : 图标宽度
         * @param [in] height : 图标高度
         * @param [in] depth : 图标色深
         * @param [in] pitch : 图标数据跨度
         * @param [in] format : 图标数据格式
         */
        void setWindowIcon(void *pixels, int32_t width, int32_t height,
            int32_t depth, int32_t pitch, uint32_t format) override;

        /**
         * @brief 获取窗口宽度和高度
         */
        void getWindowSize(int32_t &width, int32_t &height) const override;

        /**
         * @brief 获取色深
         */
        uint32_t getColorDepth() const override;

        /**
         * @brief 获取窗口显示帧缓冲
         */
        uint8_t *getFramebuffer() override;

        /**
         * @brief 获取窗口显示帧缓冲大小
         */
        size_t getFramebufferSize() const override;

        /**
         * @brief 更新窗口，把帧缓冲数据更新到窗口里
         */
        TResult updateWindow() override;

    protected:
        SDL_Window  *mSDLWindow;        /**< SDL 窗口对象 */
        SDL_Surface *mSDLIconSurface;   /**< SDL 窗口图标对象 */

        uint8_t     *mFramebuffer;      /**< 窗口显示缓冲区 */
        size_t      mFramebufferSize;   /**< 帧缓冲大小 */
    };
}


#endif  /*__T3D_SDL_DESKTOP_WINDOW_H__*/