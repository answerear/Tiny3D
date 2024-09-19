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
     * @class   Plugin
     * @brief   插件基类，所有插件均需从此类派生并实现其接口
     */
    class T3D_ENGINE_API Plugin : public Object
    {
    public:
        /**
         * @brief 获取插件名称
         * @return 返回插件名称.
         */
        virtual const String &getName() const = 0;

        /**
         * @brief 安装插件
         * @return 调用成功返回 T3D_OK.
         */
        virtual TResult install() = 0;

        /**
         * @brief 启动插件
         * @return 调用成功返回 T3D_OK.
         */
        virtual TResult startup() = 0;

        /**
         * @brief 关闭插件
         * @return 调用成功返回 T3D_OK.
         */
        virtual TResult shutdown() = 0;

        /**
         * @brief 卸载插件
         * @return 调用成功返回 T3D_OK.
         */
        virtual TResult uninstall() = 0;
    };
}


#endif  /*__T3D_PLUGIN_H__*/
