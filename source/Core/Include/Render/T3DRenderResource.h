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
