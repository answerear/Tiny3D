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

#ifndef __T3D_INDIRECT_LIGHT_H__
#define __T3D_INDIRECT_LIGHT_H__


#include "Light/T3DLight.h"


namespace Tiny3D
{
    /**
     * \brief 不依赖 Transform 位置与方向的间接光源基类
     * \remarks 当前唯一派生类为 AmbientLight
     */
    TCLASS()
    class T3D_ENGINE_API IndirectLight : public Light
    {
        TRTTI_ENABLE(Light)
        TRTTI_FRIEND
        
    public:        
        ~IndirectLight() override = default;

    protected:
        /// 默认构造，供子类与 RTTR 使用
        IndirectLight() = default;

        /**
         * \brief 以指定 UUID 构造间接光源
         * \param [in] uuid : 组件唯一标识
         */
        IndirectLight(const UUID &uuid);
    };
}


#endif  /*__T3D_INDIRECT_LIGHT_H__*/

