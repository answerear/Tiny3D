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

#ifndef __T3D_SYSTEM_PREREQUISITES_H__
#define __T3D_SYSTEM_PREREQUISITES_H__


#if defined T3DSYSTEM_EXPORT
#define T3D_SYSTEM_API T3D_EXPORT_API
#else
#define T3D_SYSTEM_API T3D_IMPORT_API
#endif


#include <T3DPlatformLib.h>
#include <rttr/registration>
#include <rttr/registration_friend>


namespace Tiny3D
{
    using RTTRObject = rttr::instance;
    using RTTRVariant = rttr::variant;
    using RTTRType = rttr::type;

    struct T3D_SYSTEM_API RTTRTypeHash
    {
        std::size_t operator()(const RTTRType& type) const
        {
            // 使用std::hash来计算每个字节的哈希值，然后将它们组合起来
            return type.get_id();
        }
    };

    struct T3D_SYSTEM_API RTTRTypeEqual
    {
        bool operator()(const RTTRType& lhs, const RTTRType& rhs) const
        {
            // 使用std::equal来比较两个UUID是否相等
            return lhs == rhs;
        }
    };
}


#endif  /*__T3D_SYSTEM_PREREQUISITES_H__*/
