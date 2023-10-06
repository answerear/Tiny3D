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
        : mIsWorldMatrixDirty(false)
        , mIsViewMatrixDirty(false)
        , mIsProjMatrixDirty(false)
        , mRenderTarget(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    
    RHIContext::~RHIContext()
    {

    }

    //--------------------------------------------------------------------------

    TResult RHIContext::renderAllTargets()
    {
        auto itr = mRenderTargets.begin();

        while (itr != mRenderTargets.end())
        {
            // 设置当前渲染目标
            mRenderTarget = itr->second;
            // 渲染
            itr->second->render();
            // 清空当前渲染目标
            mRenderTarget = nullptr;

            ++itr;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult RHIContext::attachRenderTarget(RenderTargetPtr target)
    {
        TPair<RenderTargetListItr, bool> ret =
            mRenderTargets.insert(RenderTargetListValue(target->getName(),
                target));

        if (ret.second)
        {
            return T3D_OK;
        }

        return T3D_ERR_DUPLICATED_ITEM;
    }

    //--------------------------------------------------------------------------

    TResult RHIContext::detachRenderTarget(const String &name)
    {
        auto itr = mRenderTargets.find(name);
        if (itr == mRenderTargets.end())
        {
            return T3D_ERR_NOT_FOUND;
        }

        mRenderTargets.erase(itr);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RenderTargetPtr RHIContext::getRenderTarget(const String &name)
    {
        RenderTargetListItr itr = mRenderTargets.find(name);

        if (itr != mRenderTargets.end())
        {
            return itr->second;
        }

        return nullptr;
    }

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
