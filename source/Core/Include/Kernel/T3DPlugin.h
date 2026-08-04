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


#ifndef __T3D_PLUGIN_H__
#define __T3D_PLUGIN_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 引擎插件抽象接口；由 Agent::installPlugin / uninstallPlugin 按生命周期驱动
     * \remarks 调用顺序：install → startup；卸载时：shutdown → uninstall。
     *          动态库插件通常在 dllStartPlugin 内构造并 installPlugin，在 dllStopPlugin 内 uninstallPlugin。
     */
    class T3D_ENGINE_API Plugin : public Object
    {
    public:
        /**
         * \brief 返回插件唯一名称（用作 Agent 插件表的 key）
         * \return 插件名称的常量引用
         */
        virtual const String &getName() const = 0;

        /**
         * \brief 安装插件：注册到引擎（如档案创建器、渲染器等），尚未进入可用态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult install() = 0;

        /**
         * \brief 启动插件：在 install 成功后进入可用态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult startup() = 0;

        /**
         * \brief 关闭插件：停止运行并准备卸载（与 startup 相对）
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult shutdown() = 0;

        /**
         * \brief 卸载插件：撤销 install 时的注册（与 install 相对）
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult uninstall() = 0;
    };
}


#endif  /*__T3D_PLUGIN_H__*/
