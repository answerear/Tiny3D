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


#ifndef __T3D_D3D11_RENDERER_H__
#define __T3D_D3D11_RENDERER_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @class   D3D11Renderer
     * @brief   A D3D11 renderer.
     */
    class D3D11Renderer 
        : public Renderer
        , public Singleton<D3D11Renderer>
    {
    public:
        /**
         * @fn  static D3D11RendererPtr create();
         * @brief   创建一个D3D9Renderer对象
         * @returns A D3D11RendererPtr.
         */
        static D3D11RendererPtr create();

        /**
         * @fn  virtual ~D3D11Renderer();
         * @brief   析构函数
         */
        virtual ~D3D11Renderer();

        /**
         * @fn  virtual TResult init() override;
         * @brief   初始化渲染器
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult init() override;

        /**
         * @fn  virtual TResult destroy() override;
         * @brief   销毁渲染器
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult destroy() override;

        /**
         * @fn  virtual TResult render() override;
         * @brief   Renders this object
         * @returns A TResult.
         */
        virtual TResult render() override;

        /**
         * @fn  virtual RenderWindowPtr createRenderWindow( const String &name, 
         *      const RenderWindowCreateParam &param, 
         *      const RenderWindowCreateParamEx &paramEx) override;
         * @brief   创建渲染窗口
         * @param [in]  name    : 用于标识窗口的名称，不需要跟窗口标题栏相同.
         * @param [in]  param   : 创建渲染窗口必要数据.
         * @param [in]  paramEx : 创建渲染窗口额外数据.
         * @returns 调用成功返回一个渲染窗口对象，失败返回nullptr.
         * @remarks 具体渲染系统实现本接口以创建渲染系统相关的渲染窗口对象.
         */
        virtual RenderWindowPtr createRenderWindow(
            const String &name,
            const RenderWindowCreateParam &param,
            const RenderWindowCreateParamEx &paramEx) override;

        /**
         * @fn  virtual bool queryCapability(Capability cap) const override;
         * @brief   查询渲染器能力
         * @param [in]  cap : 能力枚举值.
         * @returns 具备cap对应的能力则返回true，否则返回false.
         * @sa  Renderer::Capability
         */
        virtual bool queryCapability(Capability cap) const override;

        /**
         * @fn  virtual Matrix4 perspective(const Radian &amp;fovY, Real aspect,
         *  Real nearDist, Real farDist) override;
         * @brief   获取透视投影矩阵
         * @param [in]  fovY        : top 和 bottom 的夹角.
         * @param [in]  aspect      : 宽高比.
         * @param [in]  nearDist    : 观察空间的近平面.
         * @param [in]  farDist     : 观察空间的远平面.
         * @returns 返回一个平台相关的透视投影矩阵.
         */
        virtual Matrix4 perspective(const Radian &fovY, Real aspect,
            Real nearDist, Real farDist) override;

        /**
         * @fn  virtual Matrix4 orthographic(Real width, Real height, 
         *      Real nearDist, Real farDist) override;
         * @brief   获取正交投影矩阵
         * @param [in]  width       : 投影平面宽度.
         * @param [in]  height      : 投影平面高度.
         * @param [in]  nearDist    : 观察空间的近平面.
         * @param [in]  farDist     : 观察空间的远平面.
         * @returns 返回一个平台相关的正交投影矩阵.
         */
        virtual Matrix4 orthographic(Real width, Real height,
            Real nearDist, Real farDist) override;

        /**
         * @fn  virtual Matrix4 
         *      makeViewportMatrix(ViewportPtr viewport) override;
         * @brief   根据视口生成渲染器相关的视口变换矩阵
         * @param [in]  viewport    : 视口对象.
         * @returns 返回的视口变换矩阵.
         */
        virtual Matrix4 makeViewportMatrix(ViewportPtr viewport) override;

        /**
         * @fn  virtual TResult updateFrustum(const Matrix4 &m, 
         *      FrustumBoundPtr bound) override;
         * @brief   更新视锥体的多个平面
         * @param [in]  m       : 投影变换矩阵和观察矩阵的连接， 即 (M_proj * M_view)
         * @param [in]  bound   frustum : 需要更新的视锥体.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult updateFrustum(const Matrix4 &m,
            FrustumBoundPtr bound) override;

        /**
         * @brief 设置裁剪模式
         * @param [in] mode : 裁剪模式
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setCullingMode(CullingMode mode) override;

        /**
         * @brief 设置渲染模式
         * @param [in] mode : 渲染模式
         * @return 成功返回 T3D_OK
         * @see enum PolygonMode
         */
        virtual TResult setPolygonMode(PolygonMode mode) override;

        /**
         * @brief 设置渲染视口
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setViewport(ViewportPtr viewport) override;

        /**
         * @fn  virtual TResult setConstantBuffer(size_t slot, 
         *      HardwareConstantBufferPtr buffer) override;
         * @brief   Sets constant buffer
         * @param [in]  slot    The slot.
         * @param [in]  buffer  The buffer.
         * @returns A TResult.
         */
        virtual TResult setConstantBuffer(size_t slot, 
            HardwareConstantBufferPtr buffer) override;

        /**
         * @fn  virtual TResult setGPUProgram(GPUProgramPtr program) override;
         * @brief   Sets GPU program
         * @param   program The program.
         * @returns A TResult.
         */
        virtual TResult setGPUProgram(GPUProgramPtr program) override;

        /**
         * @brief 绘制顶点数组
         * @param [in] vao : 顶点数组对象
         * @return 调动成功返回 T3D_OK
         */
        virtual TResult drawVertexArray(VertexArrayObjectPtr vao) override;

        ID3D11Device *getD3DDevice()
        {
            return mD3DDevice;
        }

        ID3D11DeviceContext *getD3DDeviceContext()
        {
            return mD3DDeviceContext;
        }

    protected:
        /**
         * @fn  D3D11Renderer();
         * @brief   构造函数
         */
        D3D11Renderer();

    protected:
        HINSTANCE                   mInstance;          /**< The instance */
        ID3D11Device                *mD3DDevice;        /**< D3D11 设备对象 */
        ID3D11DeviceContext         *mD3DDeviceContext; /**< D3D11 设备上下文对象 */

        HardwareBufferManagerPtr    mHardwareBufferMgr; /**< Manager for hardware buffer */
    };

    #define D3D11_RENDERER      (D3D11Renderer::getInstance())
}


#endif  /*__T3D_D3D11_RENDERER_H__*/
