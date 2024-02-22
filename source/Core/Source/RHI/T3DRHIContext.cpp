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


#include "RHI/T3DRHIContext.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderWindow.h"
#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"
#include "Render/T3DSamplerState.h"
#include "RHI/T3DRHIBlendState.h"
#include "RHI/T3DRHIDepthStencilState.h"
#include "RHI/T3DRHIRasterizerState.h"
#include "RHI/T3DRHISamplerState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RHIContext::RHIContext()
        // : mIsWorldMatrixDirty(false)
        // , mIsViewMatrixDirty(false)
        // , mIsProjMatrixDirty(false)
        // , mRenderTarget(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    
    RHIContext::~RHIContext()
    {

    }

    //--------------------------------------------------------------------------

    // TResult RHIContext::renderAllTargets()
    // {
    //     auto itr = mRenderTargets.begin();
    //
    //     while (itr != mRenderTargets.end())
    //     {
    //         // 设置当前渲染目标
    //         mRenderTarget = itr->second;
    //         // 渲染
    //         itr->second->render();
    //         // 清空当前渲染目标
    //         mRenderTarget = nullptr;
    //
    //         ++itr;
    //     }
    //
    //     return T3D_OK;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult RHIContext::attachRenderTarget(RenderTargetPtr target)
    // {
    //     auto ret = mRenderTargets.emplace(target->getName(), target);
    //
    //     if (ret.second)
    //     {
    //         return T3D_OK;
    //     }
    //
    //     return T3D_ERR_DUPLICATED_ITEM;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult RHIContext::detachRenderTarget(const String &name)
    // {
    //     auto itr = mRenderTargets.find(name);
    //     if (itr == mRenderTargets.end())
    //     {
    //         return T3D_ERR_NOT_FOUND;
    //     }
    //
    //     mRenderTargets.erase(itr);
    //
    //     return T3D_OK;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // RenderTargetPtr RHIContext::getRenderTarget(const String &name)
    // {
    //     auto itr = mRenderTargets.find(name);
    //
    //     if (itr != mRenderTargets.end())
    //     {
    //         return itr->second;
    //     }
    //
    //     return nullptr;
    // }

    //--------------------------------------------------------------------------

    TResult RHIContext::setTransform(TransformState state, const Matrix4 &mat)
    {
        switch (state)
        {
        case TransformState::kView:
            break;
        case TransformState::kWorld:
            break;
        case TransformState::kProjection:
            break;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &RHIContext::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------
}
