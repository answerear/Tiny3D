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
#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"
#include "Render/T3DSamplerState.h"
#include "RHI/T3DRHIBlendState.h"
#include "RHI/T3DRHIDepthStencilState.h"
#include "RHI/T3DRHIRasterizerState.h"
#include "RHI/T3DRHISamplerState.h"
#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    class Viewport;

    /**
     * @enum    TransformState
     * @brief   设置变换矩阵状态
     */
    enum class TransformState : uint32_t
    {
        kView = 0,      /**< 视口变换矩阵 */
        kWorld,         /**< 世界变换矩阵 */
        kProjection,    /**< 投影变换矩阵 */
    };

    /**
     * @enum    PrimitiveType
     * @brief   渲染图元类型
     */
    enum class PrimitiveType : uint32_t
    {
        kPointList = 0,     /**< 点列表图元 */
        kLineList,          /**< 线列表图元 */
        kLineStrip,         /**< 线带图元 */
        kTriangleList,      /**< 三角形列表图元 */
        kTriangleStrip,     /**< 三角形带图元 */
        kTriangleFan,       /**< 三角形扇形图元 */
    };

    /**
     * @enum    ClearFlags
     * @brief   Values that represent clear flags
     */
    enum ClearFlags : uint32_t
    {
        kClearStencil = 0,
        kClearTarget,
        kClearZBuffer,
    };
    
    /**
     * @class   RHIContext
     * @brief   渲染器抽象类，负责提供抽象渲染接口，具体渲染器实现这些接口
     */
    class T3D_ENGINE_API RHIContext : public Object
    {
    public:
        /**
         * @brief   构造函数
         */
        RHIContext();

        /**
         * @brief   析构函数
         */
        ~RHIContext() override;

        virtual TResult init() = 0;
     
        virtual TResult renderAllTargets();
     
        virtual TResult attachRenderTarget(RenderTargetPtr target);

        virtual TResult detachRenderTarget(const String &name);

        RenderTargetPtr getRenderTarget(const String &name);

        virtual TResult setTransform(TransformState state, const Matrix4 &mat);

        TResult setWorldTransform(const Matrix4 &mat);
        
        TResult setViewTransform(const Matrix4 &mat);

        TResult setProjectionTransform(const Matrix4 &mat);

        virtual const Matrix4 &getTransform(TransformState state) const;

        virtual Matrix4 makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const = 0;

        virtual Matrix4 makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist) = 0;

        virtual Matrix4 makeViewportMatrix(Viewport *viewport) = 0;

        virtual RHIRenderWindowPtr createRenderWindow(RenderWindow *window, const RenderWindowCreateParam &param) = 0;

        virtual TResult clear(const ColorRGB &color, uint32_t clearFlags, Real depth, uint32_t stencil) = 0;

        virtual RHIBlendStatePtr createBlendState(const BlendDesc &desc) = 0;

        virtual RHIDepthStencilStatePtr createDepthStencilState(const DepthStencilDesc &desc) = 0;

        virtual RHIRasterizerStatePtr createRasterizerState(const RasterizerDesc &desc) = 0;

        virtual RHISamplerStatePtr createSamplerState(const SamplerDesc &desc) = 0;

        virtual TResult setBlendState(RHIBlendStatePtr state) = 0;

        virtual TResult setDepthStencilState(RHIDepthStencilStatePtr state) = 0;

        virtual TResult setRasterizerState(RHIRasterizerStatePtr state) = 0;
        
        virtual TResult setSamplerState(RHISamplerStatePtr state) = 0;

        virtual TResult setViewport(Viewport *viewport) = 0;

        virtual TResult setShader() = 0;

        virtual TResult setTexture() = 0;

        virtual TResult renderObject() = 0;
        
    protected:
        typedef TMap<String, RenderTargetPtr>      RenderTargetList;
        typedef RenderTargetList::iterator          RenderTargetListItr;
        typedef RenderTargetList::const_iterator    RenderTargetListConstItr;
        typedef RenderTargetList::value_type        RenderTargetListValue;

        bool    mIsWorldMatrixDirty;    /**< 世界变换矩阵是否更新标识 */
        bool    mIsViewMatrixDirty;     /**< 视图变换矩阵是否更新标识 */
        bool    mIsProjMatrixDirty;     /**< 投影变换矩阵是否更新标识 */

        RenderTargetList        mRenderTargets;     /**< 渲染目标列表 */

        // RenderCapabilitiesPtr   mCapabilities;      /**< 渲染能力值 */
        
        RenderTargetPtr         mRenderTarget;      /**< 当前渲染目标 */
    };
}


#include "T3DRHIContext.inl"


#endif  /*__T3D_RHI_CONTEXT_H__*/
