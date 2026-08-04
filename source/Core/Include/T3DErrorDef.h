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

#ifndef __T3D_ERROR_DEF_H__
#define __T3D_ERROR_DEF_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief Core 模块错误码，基址为编译期常量 T3D_ERR_CORE
     */
    enum CoreErrorCode : TResult
    {
        /** \brief 配置文件与 XML（+0x0000） */
        /// 解析 XML 出错
        T3D_ERR_CFG_FILE_PARSING_XML    = T3D_ERR_CORE + 0x0000,
        /// XML 格式错误
        T3D_ERR_CFG_FILE_XML_FORMAT,
        /// 构建 XML 出错
        T3D_ERR_CFG_FILE_BUILDING_XML,

        /** \brief 插件加载与管理（+0x0020） */
        /// 加载插件出错
        T3D_ERR_PLG_LOAD_FAILED         = T3D_ERR_CORE + 0x0020,
        /// 插件不存在
        T3D_ERR_PLG_NOT_EXISTS,
        /// 插件已存在
        T3D_ERR_PLG_DUPLICATED,
        /// 不是插件动态库
        T3D_ERR_PLG_NOT_DYLIB,
        /// 获取插件函数失败
        T3D_ERR_PLG_NO_FUNCTION,
        /// 无法获取插件路径
        T3D_ERR_PLG_NO_PATH,
        /// 插件未加载
        T3D_ERR_PLG_NOT_LOADED,

        /** \brief 图像编解码（+0x0060） */
        /// 找不到对应的图像编解码器
        T3D_ERR_IMG_NOT_FOUND           = T3D_ERR_CORE + 0x0060,

        /** \brief 引擎设置项（+0x0080） */
        /// 找不到设置项
        T3D_ERR_SETTINGS_NOT_FOUND      = T3D_ERR_CORE + 0x0080,

        /** \brief 资源加载与材质构建（+0x00A0） */
        /// 加载资源失败
        T3D_ERR_RES_LOAD_FAILED         = T3D_ERR_CORE + 0x00A0,
        /// 非法资源类型
        T3D_ERR_RES_INVALID_TYPE,
        /// 非法文件类型
        T3D_ERR_RES_INVALID_FILETYPE,
        /// 无效文件版本
        T3D_ERR_RES_INVALID_VERSION,
        /// 无效对象
        T3D_ERR_RES_INVALID_OBJECT,
        /// 无效文件内容
        T3D_ERR_RES_INVALID_CONTENT,
        /// 无效属性
        T3D_ERR_RES_INVALID_PROPERTY,
        /// 克隆资源属性失败
        T3D_ERR_RES_CLONE,
        /// 无效的 Shader
        T3D_ERR_RES_INVALID_SHADER,
        /// 重复资源
        T3D_ERR_RES_DUPLICATED,
        /// 创建 Technique 失败
        T3D_ERR_RES_CREATE_TECHNIQUE,
        /// 创建 Shader 失败
        T3D_ERR_RES_CREATE_SHADER,
        /// 创建 Pass 失败
        T3D_ERR_RES_CREATE_PASS,

        /** \brief 硬件缓冲读写（+0x00E0） */
        /// 写硬件缓冲失败
        T3D_ERR_HW_BUFFER_WRITE         = T3D_ERR_CORE + 0x00E0,
        /// 读硬件缓冲失败
        T3D_ERR_HW_BUFFER_READ,

        /** \brief GPU 着色器对象（+0x0100） */
        /// 同阶段着色器重复
        T3D_ERR_GPU_DUPLICATED_STAGE    = T3D_ERR_CORE + 0x0100,
        /// 着色器对象不存在
        T3D_ERR_GPU_NONEXISTENT,

        /** \brief 渲染与窗口（+0x0200） */
        /// 应使用同步读接口
        T3D_ERR_RENDER_BUFFER_READ_SYNC = T3D_ERR_CORE + 0x0200,
        /// CPU 无法读取数据
        T3D_ERR_RENDER_BUFFER_CPY_READ,
        /// 渲染窗口已存在
        T3D_ERR_RENDER_WINDOW_ALREADY,
        /// 创建渲染窗口失败
        T3D_ERR_RENDER_CREATE_WINDOW,
        /// 创建阴影贴图失败
        T3D_ERR_RENDER_CRATE_SHADOWMAP,

        /** \brief 序列化与反射解析（+0x0400） */
        /// 序列化对象失败
        T3D_ERR_SERIALIZE_OBJECT        = T3D_ERR_CORE + 0x0400,
        /// 反序列化对象失败
        T3D_ERR_DESERIALIZE_OBJECT,
        /// 反射无法解析数据流
        T3D_ERR_PARSE_STREAM,
        /// 缺少 magic 字段
        T3D_ERR_MISSING_MAGIC,
        /// 缺少 version 字段
        T3D_ERR_MISSING_VERSION,
        /// 缺少 object 内容
        T3D_ERR_MISSING_OBJECT,
    };
}


#endif    /*__T3D_ERROR_DEF_H__*/
