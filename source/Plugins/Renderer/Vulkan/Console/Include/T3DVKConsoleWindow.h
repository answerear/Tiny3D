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


#ifndef __T3D_VKCONSOLE_WINDOW_H__
#define __T3D_VKCONSOLE_WINDOW_H__


#include "T3DVKConsolePrerequisites.h"


namespace Tiny3D
{
    class VKConsoleWindow : public RHIRenderWindow
    {
    public:
        /**
         * @brief 创建渲染窗口对象
         * @return 返回一个渲染窗口对象
         */
        static VKConsoleWindowPtr create();

        /**
         * @brief 析构函数
         */
        ~VKConsoleWindow() override;

        /**
         * @brief 双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return 调用成功返回 T3D_OK
         */
        TResult swapBuffers() override;

        TResult resize(uint32_t w, uint32_t h) override;

        void *getNativeObject() const override;
        
    protected:
        /**
         * @brief 构造函数
         */
        VKConsoleWindow();
    };
}


#endif  /*__T3D_VKCONSOLE_WINDOW_H__*/
