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
// #include "Render/T3DRenderWindow.h"
// #include "Render/T3DBlendState.h"
// #include "Render/T3DDepthStencilState.h"
// #include "Render/T3DRasterizerState.h"
// #include "Render/T3DSamplerState.h"
// #include "RHI/T3DRHIBlendState.h"
// #include "RHI/T3DRHIDepthStencilState.h"
// #include "RHI/T3DRHIRasterizerState.h"
// #include "RHI/T3DRHISamplerState.h"


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

    const Matrix4& RHIContext::getDepthRemapMatrix() const
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    RHIQueryPtr RHIContext::createQuery(QueryType type)
    {
        T3D_ASSERT(!mCapabilities.supportsQuery(type));
        T3D_LOG_WARNING(LOG_TAG_RENDER, "createQuery : %s query is not supported by this RHI backend", getQueryTypeName(type));
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult RHIContext::beginQuery(RHIQuery *query)
    {
        // 后端不支持时 createQuery 不会返回对象，走到这里说明传了别的后端的查询
        T3D_LOG_ERROR(LOG_TAG_RENDER, "beginQuery is not supported by this RHI backend");
        return query == nullptr ? T3D_ERR_INVALID_PARAM : T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    TResult RHIContext::endQuery(RHIQuery *query)
    {
        T3D_LOG_ERROR(LOG_TAG_RENDER, "endQuery is not supported by this RHI backend");
        return query == nullptr ? T3D_ERR_INVALID_PARAM : T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    QueryStatus RHIContext::resolveQuery(RHIQuery *query, QueryResult &result)
    {
        if (query == nullptr)
        {
            return QueryStatus::kInvalid;
        }
        return query->resolve(result);
    }

    //--------------------------------------------------------------------------

    TResult RHIContext::setPredication(RHIQuery *query)
    {
        if (query == nullptr)
        {
            // 取消条件渲染在任何后端都是合法的空操作
            return T3D_OK;
        }
        T3D_RHI_UNSUPPORTED(supportsPredication);
    }

    //--------------------------------------------------------------------------
}
