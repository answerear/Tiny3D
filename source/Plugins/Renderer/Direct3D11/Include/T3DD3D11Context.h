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


#ifndef __T3D_D3D11_CONTEXT_H__
#define __T3D_D3D11_CONTEXT_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @class   D3D11Renderer
     * @brief   DirectX 11 渲染器.
     */
    class D3D11Context : public RHIContext
    {
    public:
        /**
         * @fn  static D3D11ContextPtr create();
         * @brief   创建一个D3D11Renderer对象
         * @returns 返回一个新建对象.
         */
        static D3D11ContextPtr create();

        /**
         * @fn  virtual ~D3D11Renderer();
         * @brief   析构函数
         */
        virtual ~D3D11Context();

        TResult init() override;
     
        /**
         * @fn  virtual TResult renderAllTarget() override;
         * @brief   渲染一帧
         * @return  调用成功返回 T3D_OK.
         */
        TResult renderAllTargets() override;

        Matrix4 makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const override;

        Matrix4 makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist) override;

        Matrix4 makeViewportMatrix(Viewport *viewport) override;

        RHIRenderWindowPtr createRenderWindow(RenderWindow *window, const RenderWindowCreateParam &param) override;

        TResult clear(const ColorRGB &color, uint32_t clearFlags, Real depth, uint32_t stencil) override;

        RHIBlendStatePtr createBlendState(const BlendDesc &desc) override;

        RHIDepthStencilStatePtr createDepthStencilState(const DepthStencilDesc &desc) override;

        RHIRasterizerStatePtr createRasterizerState(const RasterizerDesc &desc) override;

        RHISamplerStatePtr createSamplerState(const SamplerDesc &desc) override;

        TResult setBlendState(RHIBlendStatePtr state) override;

        TResult setDepthStencilState(RHIDepthStencilStatePtr state) override;

        TResult setRasterizerState(RHIRasterizerStatePtr state) override;
        
        TResult setSamplerState(RHISamplerStatePtr state) override;

        TResult setViewport(Viewport *viewport) override;

        TResult setShader() override;

        TResult setTexture() override;

        TResult renderObject() override;

        ID3D11Device *getD3DDevice() const { return mD3DDevice; }

        ID3D11DeviceContext *getD3DDeviceContext() const { return mD3DDeviceContext; }

        TResult checkMultiSampleQuality(UINT width, UINT height, UINT uMSAAQuality, UINT uMSAACount, DXGI_FORMAT format);
        
    protected:
        /**
         * @fn  D3D11Renderer();
         * @brief   构造函数
         */
        D3D11Context();

        /**
         * @fn  TResult collectInformation();
         * @brief   收集驱动、开发商等信息
         * @return  调用成功返回 T3D_OK.
         */
        TResult collectInformation();

    protected:
        /**< The instance */
        HINSTANCE                   mInstance;

        /**< D3D11 设备对象 */
        ID3D11Device        *mD3DDevice;
        /**< D3D11 上下文对象 */
        ID3D11DeviceContext *mD3DDeviceContext;
        /**< D3D 特性级别 */
        D3D_FEATURE_LEVEL   mFeatureLevel;
    };
}


#endif  /*__T3D_D3D11_CONTEXT_H__*/
