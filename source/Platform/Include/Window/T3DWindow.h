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

#ifndef __T3D_WINDOW_H__
#define __T3D_WINDOW_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "Window/T3DSysWMInfo.h"
#include "T3DNoncopyable.h"


namespace Tiny3D
{
    class IWindow;

    class T3D_PLATFORM_API Window : public Noncopyable
    {
    public:
        static const uint32_t WINDOW_FULLSCREEN;            /**< 全屏窗口 */
        static const uint32_t WINDOW_FULLSCREEN_DESKTOP;    /**< 桌面全屏窗口 */
        static const uint32_t WINDOW_OPENGL;                /**< Open GL 窗口 */
        static const uint32_t WINDOW_SHOWN;                 /**< 显示窗口 */
        static const uint32_t WINDOW_HIDDEN;                /**< 隐藏窗口 */
        static const uint32_t WINDOW_BORDERLESS;            /**< 无边框窗口 */
        static const uint32_t WINDOW_RESIZABLE;             /**< 可调整大小 */
        static const uint32_t WINDOW_MINIMIZED;             /**< 最小化 */
        static const uint32_t WINDOW_MAXIMIZED;             /**< 最大化 */
        static const uint32_t WINDOW_INPUT_GRABBED;         /**< 获取输入 */
        static const uint32_t WINDOW_INPUT_FOCUS;           /**< 获取输入焦点 */
        static const uint32_t WINDOW_MOUSE_CAPTURE;         /**< 获取鼠标 */

        static const uint32_t PIXEL_FORMAT_INDEX8;
        static const uint32_t PIXEL_FORMAT_RGB444;
        static const uint32_t PIXEL_FORMAT_RGB555;
        static const uint32_t PIXEL_FORMAT_BGR555;
        static const uint32_t PIXEL_FORMAT_ARGB4444;
        static const uint32_t PIXEL_FORMAT_RGBA4444;
        static const uint32_t PIXEL_FORMAT_ABGR4444;
        static const uint32_t PIXEL_FORMAT_BGRA4444;
        static const uint32_t PIXEL_FORMAT_ARGB1555;
        static const uint32_t PIXEL_FORMAT_RGBA5551;
        static const uint32_t PIXEL_FORMAT_ABGR1555;
        static const uint32_t PIXEL_FORMAT_BGRA5551;
        static const uint32_t PIXEL_FORMAT_RGB565;
        static const uint32_t PIXEL_FORMAT_BGR565;
        static const uint32_t PIXEL_FORMAT_RGB24;
        static const uint32_t PIXEL_FORMAT_BGR24;
        static const uint32_t PIXEL_FORMAT_RGB888;
        static const uint32_t PIXEL_FORMAT_RGBX8888;
        static const uint32_t PIXEL_FORMAT_BGR888;
        static const uint32_t PIXEL_FORMAT_BGRX8888;
        static const uint32_t PIXEL_FORMAT_ARGB8888;
        static const uint32_t PIXEL_FORMAT_RGBA8888;
        static const uint32_t PIXEL_FORMAT_ABGR8888;
        static const uint32_t PIXEL_FORMAT_BGRA8888;

        /**
         * @brief 窗口图标信息
         */
        struct WindowIcon
        {
            void*       pixels;     /**< 像素数据 */
            size_t      width;      /**< 图标宽度 */
            size_t      height;     /**< 图标高度 */
            size_t      depth;      /**< 颜色色深 */
            size_t      pitch;      /**< 图像跨度 */
            uint32_t    format;     /**< 像素格式 */
        };

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
         * @return 成功返回 T3D_OK.
         */
        TResult create(const char *title, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t flags);

        /**
         * @brief 根据传入的数据创建窗口
         * @param [in] data : 创建窗口依赖的数据
         * @return 成功返回 T3D_OK
         */
        TResult createFrom(const void *data);

        /**
         * @brief 销毁窗口.
         * @remarks 析构函数同样会自动调用
         */
        void destroy();

        /**
         * @brief 设置窗口图标
         * @param [in] icon : 图标数据对象
         */
        void setWindowIcon(const WindowIcon &icon);

        /**
         * @brief 获取窗口系统相关信息
         * @param [in][out] info : 返回的窗口系统相关信息
         * @return 获取成功返回true
         */
        bool getSystemInfo(SysWMInfo &info) const;

        /**
         * @brief 获取原生对象，这个视系统而定
         * @return 返回的原生窗口对象
         */
        void *getNativeObject() const;

        /**
         * @brief 获取窗口宽度和高度
         */
        void getWindowSize(int32_t &width, int32_t &height) const;

        /**
         * @brief 获取窗口色深
         */
        uint32_t getColorDepth() const;

        /**
         * @brief 获取窗口显示帧缓冲
         */
        uint8_t *getFramebuffer();

        /**
         * @brief 获取窗口显示帧缓冲大小
         */
        size_t getFramebufferSize() const;

        /**
         * @brief 更新窗口，把帧缓冲数据更新到窗口里
         */
        TResult updateWindow();

    protected:
        IWindow *mWindow;
    };
}


#endif  /*__T3D_WINDOW_H__*/