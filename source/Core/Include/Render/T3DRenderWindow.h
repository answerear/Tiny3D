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


#ifndef __T3D_RENDER_WINDOW_H__
#define __T3D_RENDER_WINDOW_H__


#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderWindowDesc.h"


namespace Tiny3D
{
    /**
     * @brief 渲染窗口
     */
    class T3D_ENGINE_API RenderWindow : public RenderTarget
    {
    public:
        static RenderWindowPtr create(const String &name, const RenderWindowDesc &desc);

        /**
         * @brief 获取渲染目标类型
         * @remarks 实现基类接口
         */
        Type getType() const override;

        /**
         * @brief 创建渲染窗口实体
         * @param [in] param : 渲染窗口需要必要参数
         * @return 调用成功返回 T3D_OK
         * @remarks 具体渲染系统子类实现本接口
         */
        TResult init(const RenderWindowDesc &param);

        /**
         * @brief 双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return 调用成功返回 T3D_OK
         * @remarks 具体渲染系统子类实现本接口
         */
        virtual TResult swapBuffers();

        /**
         * @brief 获取是否全屏窗口
         */
        bool isFullscreen() const { return mIsFullscreen; }

        bool getSystemInfo(SysWMInfo &info) const;
        
    protected:
        RenderWindow(const String &name);

        ~RenderWindow() override;

        TResult loadIcon(const String &iconPath);

        TResult destroy();
        
        /// OS 相关的窗口对象
        Window  *mWindow {nullptr};
        bool    mIsFullscreen {false};
    };

    class T3D_ENGINE_API NullRenderWindow : public RenderWindow
    {
    public:
        static NullRenderWindowPtr create(const String &name) { return new NullRenderWindow(name); }

        /**
         * @brief 双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return 调用成功返回 T3D_OK
         * @remarks 具体渲染系统子类实现本接口
         */
        TResult swapBuffers() override { return T3D_OK; }

    protected:
        NullRenderWindow(const String &name) : RenderWindow(name) {}
    };
}


#endif  /*__T3D_RENDER_WINDOW_H__*/
