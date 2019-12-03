/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "Render/T3DRenderer.h"
#include "Resource/T3DGPUConstBufferManager.h"
#include "Resource/T3DGPUProgramManager.h"


namespace Tiny3D
{
    const char * const Renderer::REFERENCE3D = "Reference3D";
    const char * const Renderer::DIRECT3D9 = "Direct3D9";
    const char * const Renderer::DIRECT3D11 = "Direct3D11";
    const char * const Renderer::OPENGL3 = "OpenGL 3.x";
    const char * const Renderer::OPENGLES2 = "OpenGL ES 2";
    const char * const Renderer::OPENGLES3 = "OpenGL ES 3";
    const char * const Renderer::VULKAN = "Vulkan";
    const char * const Renderer::METAL = "Metal";

    //--------------------------------------------------------------------------

    Renderer::Renderer()
        : mRenderTarget(nullptr)
        , mViewport(nullptr)
        , mCullingMode(CullingMode::NONE)
        , mPolygonMode(PolygonMode::SOLID)
        , mIsWorldMatrixDirty(false)
        , mIsViewMatrixDirty(false)
        , mIsProjMatrixDirty(false)
    {

    }

    Renderer::~Renderer()
    {

    }

    //--------------------------------------------------------------------------

    TResult Renderer::clear(const ColorRGB &color, uint32_t clearFlags, 
        Real depth, uint32_t stencil)
    {
        if (mRenderTarget != nullptr)
        {
            mRenderTarget->clear(color, clearFlags, depth, stencil);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Renderer::render()
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

    TResult Renderer::attachRenderTarget(RenderTargetPtr target)
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

    TResult Renderer::detachRenderTarget(const String &name)
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

    RenderTargetPtr Renderer::getRenderTarget(const String &name)
    {
        RenderTargetListItr itr = mRenderTargets.find(name);

        if (itr != mRenderTargets.end())
        {
            return itr->second;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult Renderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        TResult ret = T3D_OK;

        do
        {
            switch (state)
            {
            case TransformState::VIEW:
                {
                    mGPUConstUpdateFrame.mViewMatrix = mat;
                    mIsViewMatrixDirty = true;
                }
                break;
            case TransformState::WORLD:
                {
                    mGPUConstUpdateObject.mWorldMatrix = mat;
                    mIsWorldMatrixDirty = true;
                }
                break;
            case TransformState::PROJECTION:
                {
                    mGPUConstUpdateRarely.mProjMatrix = mat;
                    mIsProjMatrixDirty = true;
                }
                break;
            default:
                {
                    ret = T3D_ERR_INVALID_PARAM;
                    T3D_LOG_ERROR(LOG_TAG_RENDER, "Invalid transform state !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Renderer::getTransform(TransformState state) const
    {
        switch (state)
        {
        case TransformState::VIEW:
            return mGPUConstUpdateFrame.mViewMatrix;
            break;
        case TransformState::WORLD:
            return mGPUConstUpdateObject.mWorldMatrix;
            break;
        case TransformState::PROJECTION:
            return mGPUConstUpdateRarely.mProjMatrix;
            break;
        }

        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    TResult Renderer::postInit()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = T3D_GPU_CONST_BUFFER_MGR.loadBuiltInResources();
            if (ret != T3D_OK)
            {
                break;
            }

            ret = T3D_GPU_PROGRAM_MGR.loadBuiltInResources();
            if (ret != T3D_OK)
            {
                break;
            }

            mGPUBufferUpdateObject = smart_pointer_cast<GPUConstBuffer>(
                T3D_GPU_CONST_BUFFER_MGR.getResource("Builtin/UpdatePerObject"));
            size_t size = mGPUBufferUpdateObject->getBufferSize();
            if (size != sizeof(mGPUConstUpdateObject))
            {
                ret = T3D_ERR_INVALID_SIZE;
                T3D_LOG_ERROR(LOG_TAG_RENDER, 
                    "The size of data is not the same as the size of buffer !");
                break;
            }

            mGPUBufferUpdateFrame = smart_pointer_cast<GPUConstBuffer>(
                T3D_GPU_CONST_BUFFER_MGR.getResource("Builtin/UpdatePerFrame"));
            size = mGPUBufferUpdateObject->getBufferSize();
            if (size != sizeof(mGPUConstUpdateObject))
            {
                ret = T3D_ERR_INVALID_SIZE;
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "The size of data is not the same as the size of buffer !");
                break;
            }

            mGPUBufferUpdateRarely = smart_pointer_cast<GPUConstBuffer>(
                T3D_GPU_CONST_BUFFER_MGR.getResource("Builtin/UpdateRarely"));
            size = mGPUBufferUpdateObject->getBufferSize();
            if (size != sizeof(mGPUConstUpdateObject))
            {
                ret = T3D_ERR_INVALID_SIZE;
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "The size of data is not the same as the size of buffer !");
                break;
            }

            ret = mGPUBufferUpdateObject->initWithData(
                sizeof(mGPUConstUpdateObject), &mGPUConstUpdateObject, 
                HardwareBuffer::Usage::STREAM, 
                HardwareBuffer::AccessMode::CPU_WRITE);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Initialize GPU built-in constant buffer with data failed !");
                break;
            }

            ret = mGPUBufferUpdateFrame->initWithData(
                sizeof(mGPUConstUpdateFrame), &mGPUConstUpdateFrame,
                HardwareBuffer::Usage::STREAM, 
                HardwareBuffer::AccessMode::CPU_WRITE);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Initialize GPU built-in constant buffer with data failed !");
                break;
            }

            ret = mGPUBufferUpdateRarely->initWithData(
                sizeof(mGPUConstUpdateRarely), &mGPUConstUpdateRarely,
                HardwareBuffer::Usage::STREAM, 
                HardwareBuffer::AccessMode::CPU_WRITE);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Initialize GPU built-in constant buffer with data failed !");
                break;
            }
        } while (0);

        return ret;
    }
}
