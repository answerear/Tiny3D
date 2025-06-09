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
    enum CoreErrorCode
    {
        /// 解析 XML 出錯
        T3D_ERR_CFG_FILE_PARSING_XML    = T3D_ERR_CORE + 0x0000,
        /// 錯誤 XML 格式
        T3D_ERR_CFG_FILE_XML_FORMAT,
        /// 構建 XML 出錯
        T3D_ERR_CFG_FILE_BUILDING_XML,

        /// 加載插件出錯
        T3D_ERR_PLG_LOAD_FAILED         = T3D_ERR_CORE + 0x0020,
        /// 插件不存在
        T3D_ERR_PLG_NOT_EXISTS,
        /// 插件已經存在
        T3D_ERR_PLG_DUPLICATED,
        /// 不是插件資源
        T3D_ERR_PLG_NOT_DYLIB,
        /// 獲取插件函數失敗
        T3D_ERR_PLG_NO_FUNCTION,
        /// 無法獲取到插件路徑
        T3D_ERR_PLG_NO_PATH,
        /// 没有加載插件
        T3D_ERR_PLG_NOT_LOADED,

        /// 找不到對應的圖像編解碼器
        T3D_ERR_IMG_NOT_FOUND           = T3D_ERR_CORE + 0x0060,

        /// 找不到設置項
        T3D_ERR_SETTINGS_NOT_FOUND      = T3D_ERR_CORE + 0x0080,

        /// 加載資源失敗
        T3D_ERR_RES_LOAD_FAILED         = T3D_ERR_CORE + 0x00A0,
        /// 非法資源類型
        T3D_ERR_RES_INVALID_TYPE,
        /// 非法文件類型
        T3D_ERR_RES_INVALID_FILETYPE,
        /// 無效文件版本
        T3D_ERR_RES_INVALID_VERSION,
        /// 無效對象
        T3D_ERR_RES_INVALID_OBJECT,
        /// 無效文件內容
        T3D_ERR_RES_INVALID_CONTENT,
        /// 無效屬性
        T3D_ERR_RES_INVALID_PROPERTY,
        /// 克隆資源屬性失敗
        T3D_ERR_RES_CLONE,
        /// 無效的 Shader
        T3D_ERR_RES_INVALID_SHADER,
        /// 重複資源
        T3D_ERR_RES_DUPLICATED,
        /// 創建 Technique 失敗
        T3D_ERR_RES_CREATE_TECHNIQUE,
        /// 創建 Shader 失敗
        T3D_ERR_RES_CREATE_SHADER,
        /// 創建 Pass 失敗
        T3D_ERR_RES_CREATE_PASS,

        /// 写硬件缓冲失败
        T3D_ERR_HW_BUFFER_WRITE         = T3D_ERR_CORE + 0x00E0,
        /// 读硬件缓冲失败
        T3D_ERR_HW_BUFFER_READ,

        /// 同階段的著色器重複了
        T3D_ERR_GPU_DUPLICATED_STAGE    = T3D_ERR_CORE + 0x0100,
        /// 不存在的著色器對象
        T3D_ERR_GPU_NONEXISTENT,

        /// 应该用同步读接口
        T3D_ERR_RENDER_BUFFER_READ_SYNC = T3D_ERR_CORE + 0x0200,
        /// CPU 无法读取数据
        T3D_ERR_RENDER_BUFFER_CPY_READ,
        /// 渲染窗口已经存在了
        T3D_ERR_RENDER_WINDOW_ALREADY,
        /// 创建渲染窗口失败
        T3D_ERR_RENDER_CREATE_WINDOW,
        /// 创建阴影贴图失败
        T3D_ERR_RENDER_CRATE_SHADOWMAP, 

        /// 序列化對象失敗
        T3D_ERR_SERIALIZE_OBJECT        = T3D_ERR_CORE + 0x0400,
        /// 反序列化對象失敗
        T3D_ERR_DESERIALIZE_OBJECT,
        /// 反射无法解析文件
        T3D_ERR_PARSE_STREAM,
        /// 没有 magic 字段
        T3D_ERR_MISSING_MAGIC,
        /// 没有 version 字段
        T3D_ERR_MISSING_VERSION,
        /// 没有 object 内容
        T3D_ERR_MISSING_OBJECT,
    };
}


#endif    /*__T3D_ERROR_DEF_H__*/
