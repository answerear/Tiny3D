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


#pragma once


#include "ImGuiDX11Prerequisites.h"


namespace Tiny3D
{
    class ImGuiDX11Plugin : public Plugin
    {
    public:
        /**
         * @brief 获取插件名称
         */
        const String &getName() const override;

        /**
         * @brief 安装插件
         */
        TResult install() override;

        /**
         * @brief 启动插件
         */
        TResult startup() override;

        /**
         * @brief 关闭插件
         */
        TResult shutdown() override;

        /**
         * @brief 卸载插件
         */
        TResult uninstall() override;

    protected:
        /// 插件名称
        String mName = "ImGuiDX11";
        /// 当前 imgui 渲染器
        EditorImGuiImpl  *mEditorImGuiImpl {nullptr};
    };

}


