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


#ifndef __T3D_VK_PREREQUISITES_BASE_H__
#define __T3D_VK_PREREQUISITES_BASE_H__


#include <Tiny3D.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


namespace Tiny3D
{
    #define VK_CHECK_RESULT(result) \
        if ((result) != VK_SUCCESS) \
        { \
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Vulkan error : %d", (result)); \
        }

    #define LOG_TAG_VKRENDERER        "VKRenderer"

    class VKContextBase;
    T3D_DECLARE_SMART_PTR(VKContextBase);
}


#endif  /*__T3D_VK_PREREQUISITES_BASE_H__*/
