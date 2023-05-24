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
    class D3D11Context 
        : public RHIContext
        , public Singleton<D3D11Context>
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
         * @fn  virtual TResult renderAllTarget() override;
         * @brief   渲染一帧
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult renderAllTargets() override;

        /**
         * @fn  virtual RenderWindowPtr createRenderWindow( const String &name, 
         *      const RenderWindowCreateParam &param, 
         *      const RenderWindowCreateParamEx &paramEx) override;
         * @brief   创建渲染窗口
         * @param [in]  name    : 用于标识窗口的名称，不需要跟窗口标题栏相同.
         * @param [in]  param   : 创建渲染窗口必要数据.
         * @returns 调用成功返回一个渲染窗口对象，失败返回nullptr.
         * @remarks 具体渲染系统实现本接口以创建渲染系统相关的渲染窗口对象.
         */
        virtual RHIRenderWindowPtr createRenderWindow(
            const String &name,
            const RenderWindowCreateParam &param) override;

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
        HINSTANCE                   mInstance;          /**< The instance */
    };

    #define D3D11_CONTEXT      (D3D11Context::getInstance())
}


#endif  /*__T3D_D3D11_CONTEXT_H__*/
