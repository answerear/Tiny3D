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


#ifndef __T3D_D3D11_ERROR_H__
#define __T3D_D3D11_ERROR_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    enum R3DErrorCode : int32_t
    {
        /**< 窗口已经创建 */
        T3D_ERR_D3D11_WINDOW_ALREADY = T3D_ERR_RENDERER,
        /**< 不支持的图标文件格式 */
        T3D_ERR_D3D11_UNSUPPORT_FORMAT_ICON,
        /**< 图片大小不一致 */
        T3D_ERR_D3D11_IMAGE_SIZE_NOT_EQL,
        /**< 错误的渲染目标 */
        T3D_ERR_D3D11_INVALID_TARGET,
        /**< 创建 D3D11 对象失败 */
        T3D_ERR_D3D11_CREATE_FAILED,
        /**< 创建顶点着色器失败 */
        T3D_ERR_D3D11_CREATE_VERTEX_SHADER,
        /**< 创建像素着色器失败 */
        T3D_ERR_D3D11_CREATE_PIXEL_SHADER,
        /**< Shader 没有编译 */
        T3D_ERR_D3D11_SHADER_NOT_COMPILED,
        /**< Shader 编译失败 */
        T3D_ERR_D3D11_SHADER_COMPILED,
        /**< 创建输入布局失败 */
        T3D_ERR_D3D11_CREATE_INPUT_LAYOUT,
        /**< 创建缓冲区失败 */
        T3D_ERR_D3D11_CREATE_BUFFER,
        /**< 创建 2D 纹理失败 */
        T3D_ERR_D3D11_CREATE_TEXTURE2D,
        /**< 创建 RenderTargetView 失败 */
        T3D_ERR_D3D11_CREATE_RENDER_TARGET_VIEW,
        /// 创建 ShaderResourceView 失败
        T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW,
        /// 创建 DepthStencilView 失败
        T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_VIEW,
        /// 创建 BlendState 失败
        T3D_ERR_D3D11_CREATE_BLEND_STATE,
        /// 创建 DepthStencilState 失败
        T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_STATE,
        /// 创建 RasterizerState 失败
        T3D_ERR_D3D11_CREATE_RASTERIZER_STATE,
        /// 创建 SamplerState 失败
        T3D_ERR_D3D11_CREATE_SAMPLER_STATE,
        /**< 锁定缓冲区失败 */
        T3D_ERR_D3D11_LOCK_BUFFER,
        /**< 接口调用不配对 */
        T3D_ERR_D3D11_MISMATCH_CALLING,
        /**< 获取 Multiple Sample Quality 失败 */
        T3D_ERR_D3D11_CHECK_MULTISAMPLE,
        /**< Present 失败 */
        T3D_ERR_D3D11_PRESENT,
        /**< 获取 COM 对象失败 */
        T3D_ERR_D3D11_GET_INTERFACE,
        /**< 获取 D3D 版本失败 */
        T3D_ERR_D3D11_CHECK_INTERFACE_SUPPORT,
        /**< 获取驱动描述失败 */
        T3D_ERR_D3D11_GET_DRIVER_DESC,
        /// 编译 shader 失败
        T3D_ERR_D3D11_COMPILE_SHADER,
        /// 创建 blob 失败
        T3D_ERR_D3D11_CREATE_BLOB,
        /// 反射 shader 失败
        T3D_ERR_D3D11_SHADER_REFLECTION,
        /// 获取 shader 描述失败
        T3D_ERR_D3D11_GET_SHADER_DESC,
        /// shader sampler 命名错误
        T3D_ERR_D3D11_INVALID_SHADER_SAMPLER_NAME,
    };
}



#endif  /*__T3D_D3D11_ERROR_H__*/
