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


#ifndef __T3D_RHI_CONTEXT_H__
#define __T3D_RHI_CONTEXT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @class   Renderer
     * @brief   渲染器抽象类，负责提供抽象渲染接口，具体渲染器实现这些接口
     */
    class T3D_ENGINE_API RHIContext : public Object
    {
    public:
        /**
         * @enum    TransformState
         * @brief   设置变换矩阵状态
         */
        enum class TransformState : uint32_t
        {
            VIEW = 0,      /**< 视口变换矩阵 */
            WORLD,         /**< 世界变换矩阵 */
            PROJECTION,    /**< 投影变换矩阵 */
            MAX
        };

        /**
         * @enum    PrimitiveType
         * @brief   渲染图元类型
         */
        enum class PrimitiveType : uint32_t
        {
            E_PT_POINT_LIST = 0,    /**< 点列表图元 */
            E_PT_LINE_LIST,         /**< 线列表图元 */
            E_PT_LINE_STRIP,        /**< 线带图元 */
            E_PT_TRIANGLE_LIST,     /**< 三角形列表图元 */
            E_PT_TRIANGLE_STRIP,    /**< 三角形带图元 */
            E_PT_TRIANGLE_FAN,      /**< 三角形扇形图元 */
            E_PT_MAX
        };

        /**
         * @enum    ClearFlags
         * @brief   Values that represent clear flags
         */
        enum ClearFlags : uint32_t
        {
            E_CLEAR_STENCIL = 0,
            E_CLEAR_TARGET,
            E_CLEAR_ZBUFFER,
        };

        /**
         * @fn  Renderer::Renderer();
         * @brief   构造函数
         */
        RHIContext();

        /**
         * @fn  virtual Renderer::~Renderer();
         * @brief   析构函数
         */
        virtual ~RHIContext();
     
        /**
         * @fn  virtual TResult Renderer::renderAllTargets();
         * @brief   渲染一帧
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult renderAllTargets();
     
        /**
         * @fn  virtual TResult 
         *      Renderer::attachRenderTarget(RenderTargetPtr target);
         * @brief   关联渲染目标到渲染器上
         * @param [in]  target  : 渲染目标.
         * @returns 成功返回 T3D_OK.
         */
        virtual TResult attachRenderTarget(RHIRenderTargetPtr target);

        /**
         * @fn  virtual TResult Renderer::detachRenderTarget(const String &name);
         * @brief   从渲染器上分离渲染目标
         * @param [in]  name    : 渲染目标名称.
         * @returns 成功返回 T3D_OK.
         */
        virtual TResult detachRenderTarget(const String &name);

        /**
         * @fn  RenderTargetPtr Renderer::getRenderTarget(const String &name);
         * @brief   获取渲染器上绑定的渲染目标
         * @param [in]  name    : 渲染目标名称.
         * @returns 成功返回渲染目标对象.
         */
        RHIRenderTargetPtr getRenderTarget(const String &name);

    protected:
        typedef TMap<String, RHIRenderTargetPtr>    RenderTargetList;
        typedef RenderTargetList::iterator          RenderTargetListItr;
        typedef RenderTargetList::const_iterator    RenderTargetListConstItr;
        typedef RenderTargetList::value_type        RenderTargetListValue;

        bool    mIsWorldMatrixDirty;    /**< 世界变换矩阵是否更新标识 */
        bool    mIsViewMatrixDirty;     /**< 视图变换矩阵是否更新标识 */
        bool    mIsProjMatrixDirty;     /**< 投影变换矩阵是否更新标识 */

        RenderTargetList        mRenderTargets;     /**< 渲染目标列表 */

        // RenderCapabilitiesPtr   mCapabilities;      /**< 渲染能力值 */
        
        RHIRenderTargetPtr      mRenderTarget;      /**< 当前渲染目标 */
    };
}


#include "T3DRHIContext.inl"


#endif  /*__T3D_RHI_CONTEXT_H__*/
