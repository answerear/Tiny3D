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


#ifndef __T3D_RENDER_RESOURCE_H__
#define __T3D_RENDER_RESOURCE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API RenderResource : public Object
    {
        friend class RenderResourceManager;
        
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        enum class Type : uint32_t
        {
            /// 初始值
            kNone = 0,
            /// 渲染混合状态
            kBlendState,
            /// 深度和模板缓冲状态
            kDepthStencilState,
            /// 光栅化抓紧给你抬
            kRasterizerState,
            /// 纹理采样器状态
            kSamplerState,
            /// 顶点缓冲
            kVertexBuffer,
            /// 索引缓冲
            kIndexBuffer,
            /// 像素缓冲
            kPixelBuffer1D,
            kPixelBuffer2D,
            kPixelBuffer3D,
            kPixelBufferCubemap,
            /// 常量缓冲
            kConstantBuffer,
            /// 顶点声明
            kVertexDeclaration,
        };
        
        ~RenderResource() override = default;

        virtual Type getType() const = 0;

        RHIResourcePtr getRHIResource() const { return mRHIResource; }

    protected:
        RenderResource() = default;

        virtual bool onLoad() = 0;

        virtual bool onUnload() = 0;
        
        RHIResourcePtr mRHIResource {nullptr};
    };
    
    template<typename DESC_TYPE, typename RHI_STATE_TYPE>
    class T3D_ENGINE_API RenderStateResource : public RenderResource
    {
    public:
        using desc_t = DESC_TYPE;
        using rhi_state_t = RHI_STATE_TYPE;

        uint32_t hash() const { return mHash; }

        const DESC_TYPE &getStateDesc() const { return mDesc; }

        SmartPtr<rhi_state_t> getRHIState() const { return smart_pointer_cast<rhi_state_t>(mRHIResource); }
        
    protected:
        RenderStateResource(uint32_t hash, const desc_t &desc)
            : mHash(hash)
            , mDesc(desc)
        {
            if (mHash == 0)
            {
                mHash = CRC::crc32((uint8_t*)&desc, sizeof(desc));
            }
        }

        ~RenderStateResource() override = default;

        uint32_t                mHash {0};
        desc_t                  mDesc {};
    };
}


#endif  /*__T3D_RENDER_RESOURCE_H__*/
