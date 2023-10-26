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


#include "Render/T3DForwardRenderPipeline.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderWindow.h"
#include "Render/T3DRenderTexture.h"
#include "RHI/T3DRHIContext.h"



namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ForwardRenderPipelinePtr ForwardRenderPipeline::create()
    {
        return new ForwardRenderPipeline();
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::render(RHIContextPtr ctx)
    {
        // 先逐个渲染到纹理，然后最后按照顺序把渲染到纹理 blit 到渲染窗口

        // for (auto &group : mRenderTargetGroups)
        // {
        //     // 渲染窗口
        //     ctx->setRenderTarget(group.second.renderWindow);
        //
        //     ctx->clearColor(ColorRGB::BLACK);
        //     
        //     for (auto &texture : group.second.renderTextures)
        //     {
        //         // TODO : 渲染每一个渲染纹理
        //         ctx->setRenderTarget(texture.second);
        //
        //         // 把渲染纹理渲染到渲染窗口上
        //         if (group.second.renderWindow != nullptr)
        //         {
        //             // ctx->blit(texture.second, group.second.renderWindow);
        //         }
        //     }
        //
        //     // 刷新屏幕，把 back buffer 交换到 front buffer 上显示
        //     group.second.renderWindow->swapBuffers();
        // }
        //
        // ctx->resetRenderTarget();
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    // TResult ForwardRenderPipeline::attachRenderTarget(RenderTargetPtr target, uint32_t group, uint32_t order)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         
    //         if (target == nullptr)
    //         {
    //             ret = T3D_ERR_INVALID_PARAM;
    //             T3D_LOG_ERROR(LOG_TAG_RENDER, "Render target is nullptr !");
    //             break;
    //         }
    //         
    //         switch (target->getType())
    //         {
    //         case RenderTarget::Type::E_RT_WINDOW:
    //             {
    //                 ret = attachRenderWindow(smart_pointer_cast<RenderWindow>(target), group, order);
    //             }
    //             break;
    //         case RenderTarget::Type::E_RT_TEXTURE:
    //             {
    //                 ret = attachRenderTexture(smart_pointer_cast<RenderTexture>(target), group, order);
    //             }
    //             break;
    //         default:
    //             {
    //                 ret = T3D_ERR_INVALID_PARAM;
    //                 T3D_LOG_ERROR(LOG_TAG_RENDER, "Invalid render target type ! It must be one of RenderWindow or RenderTexture !");
    //             }
    //             break;
    //         }
    //     } while (false);
    //
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult ForwardRenderPipeline::attachRenderTexture(RenderTexturePtr texture)
    // {
    //     return attachRenderTexture(texture, NONE_GROUP, DEFAULT_ORDER);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult ForwardRenderPipeline::detachRenderTarget(const String &name)
    // {
    //     return T3D_OK;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult ForwardRenderPipeline::detachAllRenderTargets()
    // {
    //     mRenderTargets.clear();
    //     mRenderTargetGroups.clear();
    //     mRenderTargetBindings.clear();
    //     mCurrentRenderTarget = nullptr;
    //     return T3D_OK;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // RenderTargetPtr ForwardRenderPipeline::getRenderTarget(const String &name) const
    // {
    //     return nullptr;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult ForwardRenderPipeline::attachRenderWindow(RenderWindowPtr window, uint32_t group, uint32_t order)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         // 没缓存加缓存，有缓存直接跳过
    //         addRenderTarget(window);
    //
    //         // 设置渲染顺序
    //         auto itrGroup = mRenderTargetGroups.find(group);
    //         if (itrGroup == mRenderTargetGroups.end())
    //         {
    //             // 没有渲染分组
    //             auto rval = mRenderTargetGroups.emplace(group, RenderTargetGroup());
    //             T3D_ASSERT(rval.second, "mRenderTargetGroups.emplace() failed !");
    //             itrGroup = rval.first;
    //         }
    //         
    //         // 不管有没有，都直接覆盖
    //         itrGroup->second.renderWindow = window;
    //         
    //         // 设置绑定关系
    //         addBindingMap(window->getName(), group, DEFAULT_ORDER);
    //     } while (false);
    //     
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult ForwardRenderPipeline::attachRenderTexture(RenderTexturePtr texture, uint32_t group, uint32_t order)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         // 没缓存加缓存，有缓存直接跳过
    //         addRenderTarget(texture);
    //
    //         auto itrGroup = mRenderTargetGroups.find(group);
    //         if (itrGroup == mRenderTargetGroups.end())
    //         {
    //             // 没有渲染分组
    //             auto rval = mRenderTargetGroups.emplace(group, RenderTargetGroup());
    //             T3D_ASSERT(rval.second, "attach render window !");
    //             itrGroup = rval.first;
    //         }
    //
    //         if (order == DEFAULT_ORDER)
    //         {
    //             // 直接加到末尾，则从最后一个元素的 key(order) 值加一，作为新的 order
    //             order = itrGroup->second.renderTextures.rbegin()->first + 1;
    //         }
    //
    //         // 加到对应顺序位置
    //         itrGroup->second.renderTextures[order] = texture;
    //
    //         // 添加绑定关系
    //         addBindingMap(texture->getName(), group, order);
    //     } while (false);
    //
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ForwardRenderPipeline::addRenderTarget(RenderTargetPtr target)
    // {
    //     auto itrTarget = mRenderTargets.find(target->getName());
    //     if (itrTarget == mRenderTargets.end())
    //     {
    //         // 没有缓存
    //         auto rval = mRenderTargets.emplace(target->getName(), target);
    //         T3D_ASSERT(rval.second, "attach render window !");
    //         itrTarget = rval.first;
    //     }
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ForwardRenderPipeline::addBindingMap(const String &name, uint32_t group, uint32_t order)
    // {
    //     auto itrBinding = mRenderTargetBindings.find(name);
    //     if (itrBinding == mRenderTargetBindings.end())
    //     {
    //         // 新建绑定关系
    //         auto rval = mRenderTargetBindings.emplace(name, RenderTargetBindings());
    //         T3D_ASSERT(rval.second, "mRenderTargetBindings.emplace() !");
    //         itrBinding = rval.first;
    //     }
    //
    //     itrBinding->second[group] = order;
    // }

    //--------------------------------------------------------------------------
}
