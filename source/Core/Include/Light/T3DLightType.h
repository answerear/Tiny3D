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

#ifndef __T3D_LIGHT_TYPE_H__
#define __T3D_LIGHT_TYPE_H__


#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /**
     * \brief 引擎光源组件的类型标识，供 Light::getLightType 与渲染管线分支使用
     */
    TENUM()
    enum class LightType : uint32_t
    {
        /// 无效或未指定类型
        kNone = 0,
        /// 环境光（IndirectLight 分支）
        kAmbient,
        /// 平行光（LocalLight 分支）
        kDirectional,
        /// 点光源（LocalLight 分支）
        kPoint,
        /// 聚光灯（LocalLight 分支）
        kSpot,
    };
}


#endif  /*__T3D_LIGHT_TYPE_H__*/
