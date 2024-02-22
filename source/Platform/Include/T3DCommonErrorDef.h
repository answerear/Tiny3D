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

#ifndef __T3D_COMMON_ERROR_DEF_H__
#define __T3D_COMMON_ERROR_DEF_H__


#include "T3DMacro.h"


namespace Tiny3D
{
    enum CommonErrorCode
    {
        T3D_ERR_NONE                = 0,            /**< 没有错误 */
        T3D_OK                      = T3D_ERR_NONE, /**< 成功 */
        T3D_SUCCESS                 = T3D_ERR_NONE, /**< 成功 */
        T3D_ERR_FAIL                = 0x00000001,   /**< 通用错误 */
        T3D_ERR_INVALID_POINTER     = 0x00000002,   /**< 空指针或者非法指针 */
        T3D_ERR_INVALID_TIMERID     = 0x00000003,   /**< 无效定时器ID */
        T3D_ERR_FILE_NOT_EXIST      = 0x00000004,   /**< 文件不存在 */
        T3D_ERR_FILE_DATA_MISSING   = 0x00000005,   /**< 文件内容缺失 */
        T3D_ERR_OUT_OF_BOUND        = 0x00000006,   /**< 数组越界 */
        T3D_ERR_NOT_FOUND           = 0x00000007,   /**< 没有找到 */
        T3D_ERR_DUPLICATED_ITEM     = 0x00000008,   /**< 重复元素 */
        T3D_ERR_SYS_NOT_INIT        = 0x00000009,   /**< 系统没有初始化 */
        T3D_ERR_INVALID_ID          = 0x0000000A,   /**< 无效ID */
        T3D_ERR_INVALID_PARAM       = 0x0000000B,   /**< 错误参数 */
        T3D_ERR_INVALID_SIZE        = 0x0000000C,   /**< 错误大小 */
        T3D_ERR_TIMEOUT             = 0x0000000D,   /**< 超时 */
        T3D_ERR_NOT_IMPLEMENT       = 0x0000000E,   
    };

    #define T3D_FAILED(ret)         (ret != T3D_OK)
    #define T3D_SUCCEEDED(ret)      (ret == T3D_OK)
};

#endif  /*__T3D_COMMON_ERROR_DEF_H__*/
