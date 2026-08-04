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


#ifndef __T3D_RENDER_STATE_H__
#define __T3D_RENDER_STATE_H__


#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"


namespace Tiny3D
{
    /**
     * \brief 渲染管线状态聚合体，持有混合/深度模板/光栅化描述及对应 RHI 状态
     */
    TCLASS()
    class T3D_ENGINE_API RenderState : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建 RenderState 实例
         * \return 新创建的 RenderState 指针
         */
        static RenderStatePtr create();
        
        /// 返回当前混合 RHI 状态；未设置描述时为 nullptr
        BlendStatePtr getBlendState() const { return mBlendState; }

        /// 返回当前深度/模板 RHI 状态；未设置描述时为 nullptr
        DepthStencilStatePtr getDepthStencilState() const { return mDepthStencilState; }

        /// 返回当前光栅化 RHI 状态；未设置描述时为 nullptr
        RasterizerStatePtr getRasterizerState() const { return mRasterizerState; }
        
        /// 返回混合状态描述
        TPROPERTY(RTTRFuncName="BlendDesc", RTTRFuncType="getter")
        const BlendDesc &getBlendDesc() const { return mBlendDesc; }

        /**
         * \brief 更新混合描述，描述变化时通过 RenderStateManager 加载新 RHI 状态
         * \param [in] desc : 新的混合状态描述
         */
        TPROPERTY(RTTRFuncName="BlendDesc", RTTRFuncType="setter")
        void setBlendDesc(const BlendDesc &desc);

        /// 返回深度/模板状态描述
        TPROPERTY(RTTRFuncName="DepthStencilDesc", RTTRFuncType="getter")
        const DepthStencilDesc &getDepthStencilDesc() const { return mDepthStencilDesc; }

        /**
         * \brief 更新深度/模板描述，描述变化时通过 RenderStateManager 加载新 RHI 状态
         * \param [in] desc : 新的深度/模板状态描述
         */
        TPROPERTY(RTTRFuncName="DepthStencilDesc", RTTRFuncType="setter")
        void setDepthStencilDesc(const DepthStencilDesc &desc);

        /// 返回光栅化状态描述
        TPROPERTY(RTTRFuncName="RasterizerDesc", RTTRFuncType="getter")
        const RasterizerDesc &getRasterizerDesc() const { return mRasterizerDesc; }

        /**
         * \brief 更新光栅化描述，描述变化时通过 RenderStateManager 加载新 RHI 状态
         * \param [in] desc : 新的光栅化状态描述
         */
        TPROPERTY(RTTRFuncName="RasterizerDesc", RTTRFuncType="setter")
        void setRasterizerDesc(const RasterizerDesc &desc);

    protected:
        /// 混合状态描述
        BlendDesc           mBlendDesc {};
        /// 深度/模板状态描述
        DepthStencilDesc    mDepthStencilDesc {};
        /// 光栅化状态描述
        RasterizerDesc      mRasterizerDesc{};

        /// 由 RenderStateManager 管理的混合 RHI 状态
        BlendStatePtr           mBlendState {nullptr};
        /// 由 RenderStateManager 管理的深度/模板 RHI 状态
        DepthStencilStatePtr    mDepthStencilState {nullptr};
        /// 由 RenderStateManager 管理的光栅化 RHI 状态
        RasterizerStatePtr      mRasterizerState {nullptr};
    };
}


#endif  /*__T3D_RENDER_STATE_H__*/
