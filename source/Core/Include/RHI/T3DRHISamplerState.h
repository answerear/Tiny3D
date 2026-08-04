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


#ifndef __T3D_RHI_SAMPLER_STATE_H__
#define __T3D_RHI_SAMPLER_STATE_H__


#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    /**
     * \brief RHI 层纹理采样状态对象，封装后端 sampler state 资源
     */
    class T3D_ENGINE_API RHISamplerState : public RHIResource
    {
    public:
        /**
         * \brief 返回资源类型 kSamplerState
         * \return ResourceType::kSamplerState
         */
        ResourceType getResourceType() const override
        {
            return ResourceType::kSamplerState;
        }

    protected:
        /// 受保护构造，由 RHIContext::createSamplerState 及后端派生类创建
        RHISamplerState();
    };
}


#endif  /*__T3D_RHI_SAMPLER_STATE_H__*/
