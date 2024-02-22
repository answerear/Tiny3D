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

#ifndef __T3D_RP_ERROR_CODE_H__
#define __T3D_RP_ERROR_CODE_H__


#include "T3DRPPrerequisites.h"


namespace Tiny3D
{
    // Framework 模块错误码从 T3D_ERR_FRAMEWORK 开始
    enum RPErrorCode
    {
        /**< 错误码开始 */
        T3D_ERR_REFLECTION_PREPROCESSOR         = T3D_ERR_USER,
        /**< 警告级别 */
        T3D_ERR_RP_WARNING                      = T3D_ERR_REFLECTION_PREPROCESSOR,
        /**< clang 编译警告*/
        T3D_ERR_RP_COMPILE_WARNING              = T3D_ERR_RP_WARNING,
        
        /**< 错误级别 */
        T3D_ERR_RP_ERROR                        = T3D_ERR_REFLECTION_PREPROCESSOR + 0x100,
        /**< clang 解析源码失败 */
        T3D_ERR_RP_PARSE_SOURCE                 = T3D_ERR_RP_ERROR,
        /**< clang 编译错误 */
        T3D_ERR_RP_COMPILE_ERROR                = T3D_ERR_RP_ERROR + 1,
        /**< 反射标签语法错误 */
        T3D_ERR_RP_TAG_SYNTAX                   = T3D_ERR_RP_ERROR + 2,
        /**< 没有分类名 */
        T3D_ERR_RP_INVALID_SPECFIER             = T3D_ERR_RP_ERROR + 3,
        /**< 属性函数数量错误 */
        T3D_ERR_RP_INVALID_NUM_PROPERTY_FUNC    = T3D_ERR_RP_ERROR + 4,
        /**< 函数名冲突 */
        T3D_ERR_RP_FUNCTION_NAME_CONFLICT       = T3D_ERR_RP_ERROR + 5,
        /**< RTTI 开关没打开，但是又打了标签 */
        T3D_ERR_RP_RTTI_DISABLED                = T3D_ERR_RP_ERROR + 6,
        /**< 基类没有开启反射 */
        T3D_ERR_RP_BASE_CLASS_NO_RTTI           = T3D_ERR_RP_ERROR + 7,
        /**< 父结点不存在 */
        T3D_ERR_RP_AST_NO_PARENT                = T3D_ERR_RP_ERROR + 8,
        /**< 没有默认构造函数 */
        T3D_ERR_RP_NO_DEFAULT_CONSTRUCTOR       = T3D_ERR_RP_ERROR + 9,
        
        /**< 致命级别 */
        T3D_ERR_RP_FATAL                        = T3D_ERR_REFLECTION_PREPROCESSOR + 0x200,
    };

    #define T3D_RP_WARNING(ret) (ret >= T3D_ERR_RP_WARNING && ret < T3D_ERR_RP_ERROR)
    #define T3D_RP_ERROR(ret)   (ret >= T3D_ERR_RP_ERROR && ret < T3D_ERR_RP_FATAL)
    #define T3D_RP_FATAL(ret)   (ret >= T3D_ERR_RP_FATAL)
}


#endif  /*__T3D_RP_ERROR_CODE_H__*/
