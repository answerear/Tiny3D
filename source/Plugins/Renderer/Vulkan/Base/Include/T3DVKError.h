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


#ifndef __T3D_VK_ERROR_H__
#define __T3D_VK_ERROR_H__


#include "T3DVKPrerequisitesBase.h"


namespace Tiny3D
{
    enum VKErrorCode : int32_t
    {
        /**< 窗口已经创建 */
        T3D_ERR_VK_WINDOW_ALREADY = T3D_ERR_RENDERER,
        /**< 不支持的图标文件格式 */
        T3D_ERR_VK_UNSUPPORT_FORMAT_ICON,
        /**< 图片大小不一致 */
        T3D_ERR_VK_IMAGE_SIZE_NOT_EQL,
        /**< 错误的渲染目标 */
        T3D_ERR_VK_INVALID_TARGET,
        /**< 创建 Vulkan 对象失败 */
        T3D_ERR_VK_CREATE_FAILED,
        /**< 创建顶点着色器失败 */
        T3D_ERR_VK_CREATE_VERTEX_SHADER,
        /**< 创建像素着色器失败 */
        T3D_ERR_VK_CREATE_PIXEL_SHADER,
        /**< 创建 hull shader 失败 */
        T3D_ERR_VK_CREATE_HULL_SHADER,
        /**< 创建 domain shader 失败 */
        T3D_ERR_VK_CREATE_DOMAIN_SHADER,
        /**< 创建 geometry shader 失败 */
        T3D_ERR_VK_CREATE_GEOMETRY_SHADER,
        /**< 创建 compute shader 失败 */
        T3D_ERR_VK_CREATE_COMPUTE_SHADER,
        /**< Shader 没有编译 */
        T3D_ERR_VK_SHADER_NOT_COMPILED,
        /**< Shader 编译失败 */
        T3D_ERR_VK_SHADER_COMPILED,
        /**< 创建 Pipeline Layout 失败 */
        T3D_ERR_VK_CREATE_PIPELINE_LAYOUT,
        /**< 创建缓冲区失败 */
        T3D_ERR_VK_CREATE_BUFFER,
        /**< 创建 Image 失败 */
        T3D_ERR_VK_CREATE_IMAGE,
        /**< 创建 ImageView 失败 */
        T3D_ERR_VK_CREATE_IMAGE_VIEW,
        /// 创建 ShaderModule 失败
        T3D_ERR_VK_CREATE_SHADER_MODULE,
        /// 分配显存失败
        T3D_ERR_VK_ALLOCATE_MEMORY,
        /// 创建 DescriptorSetLayout 失败
        T3D_ERR_VK_CREATE_DESCRIPTOR_SET_LAYOUT,
        /// 分配 DescriptorSet 失败
        T3D_ERR_VK_ALLOCATE_DESCRIPTOR_SET,
        /// 创建 Sampler 失败
        T3D_ERR_VK_CREATE_SAMPLER,
        /**< 锁定缓冲区失败 */
        T3D_ERR_VK_LOCK_BUFFER,
        /**< 接口调用不配对 */
        T3D_ERR_VK_MISMATCH_CALLING,
        /**< Present 失败 */
        T3D_ERR_VK_PRESENT,
        /// 创建 VkInstance 失败
        T3D_ERR_VK_CREATE_INSTANCE,
        /// 创建 VkDevice 失败
        T3D_ERR_VK_CREATE_DEVICE,
        /// 创建 VkSwapchain 失败
        T3D_ERR_VK_CREATE_SWAPCHAIN,
        /// 创建 VkRenderPass 失败
        T3D_ERR_VK_CREATE_RENDER_PASS,
        /// 创建 VkFramebuffer 失败
        T3D_ERR_VK_CREATE_FRAMEBUFFER,
        /// 创建 VkCommandPool 失败
        T3D_ERR_VK_CREATE_COMMAND_POOL,
        /// 创建 VkPipeline 失败
        T3D_ERR_VK_CREATE_PIPELINE,
        /// 编译 shader 失败
        T3D_ERR_VK_COMPILE_SHADER,
        /// 反射 shader 失败
        T3D_ERR_VK_SHADER_REFLECTION,
        /// 获取 shader 描述失败
        T3D_ERR_VK_GET_SHADER_DESC,
        /// shader sampler 命名错误
        T3D_ERR_VK_INVALID_SHADER_SAMPLER_NAME,
        /// Usage 和调用的接口不匹配
        T3D_ERR_VK_INVALID_USAGE,
        /// 映射 GPU 内存失败
        T3D_ERR_VK_MAP_MEMORY,
        /// 改变所有 frame buffer 大小失败
        T3D_ERR_VK_RESIZE_BUFFERS,
        /// 创建 VkSurface 失败
        T3D_ERR_VK_CREATE_SURFACE,
        /// 创建 Semaphore 失败
        T3D_ERR_VK_CREATE_SEMAPHORE,
        /// 创建 Fence 失败
        T3D_ERR_VK_CREATE_FENCE,
    };
}


#endif  /*__T3D_VK_ERROR_H__*/
