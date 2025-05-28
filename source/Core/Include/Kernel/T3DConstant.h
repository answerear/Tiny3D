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

#ifndef __T3D_CONSTANT_H__
#define __T3D_CONSTANT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @enum    PixelFormat
     * @brief   Values that represent pixel formats
     */
    TENUM()
    enum class PixelFormat : uint32_t
    {
        E_PF_UNKNOWN = 0,
        E_PF_PALETTE8,
        E_PF_B5G6R5,
        E_PF_B5G5R5A1,
        E_PF_B4R4G4A4,
        E_PF_R8G8B8,
        E_PF_B8G8R8,
        E_PF_R8G8B8A8,
        E_PF_B8G8R8A8,
        E_PF_R8G8B8X8,
        E_PF_B8G8R8X8,
        
        /// 24位无符号归一化深度 + 8位模板
        E_PF_D24_UNORM_S8_UINT,
        /// 32位浮点深度 + 8位模板 + 24位未用
        E_PF_D32_FLOAT_S8X24_UINT,
        /// 32位浮点深度，无模板
        E_PF_D32_FLOAT,
        /// 16位无符号归一化深度，无模板，精度较低
        E_PF_D16_UNORM,
    };

    TENUM()
    enum class TEXTURE_TYPE : uint32_t
    {
        TT_1D = 0,
        TT_2D,
        TT_2D_ARRAY,
        TT_3D,
        TT_CUBE,
        TT_CUBE_ARRAY,
        TT_RENDER_TEXTURE,
        TT_MAX
    };

    /**
     * @enum    PrimitiveType
     * @brief   渲染图元类型
     */
    TENUM()
    enum class PrimitiveType : uint32_t
    {
        kPointList = 0,     /**< 点列表图元 */
        kLineList,          /**< 线列表图元 */
        kLineStrip,         /**< 线带图元 */
        kTriangleList,      /**< 三角形列表图元 */
        kTriangleStrip,     /**< 三角形带图元 */
    };

    #define SHADER_TAG_QUEUE        "Queue"
}


#endif    /*__T3D_CONFIG_H__*/
