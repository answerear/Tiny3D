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


#ifndef __T3D_D3D11_PREREQUISITES_BASE_H__
#define __T3D_D3D11_PREREQUISITES_BASE_H__


#include <Tiny3D.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <algorithm>
#undef min


namespace Tiny3D
{
    #define D3D_SAFE_RELEASE(p) \
        if ((p) != nullptr)   \
        {   \
            (p)->Release();   \
            (p) = nullptr;    \
        }

    #define D3D_SAFE_RELEASE_ARRAY(p) \
        for (UINT i = 0; i < sizeof(p) / sizeof((p)[0]); ++i)   \
        {   \
            if ((p[i]) != nullptr) \
            {   \
                (p[i])->Release();   \
                (p[i]) = nullptr;   \
            }   \
        }

#if defined (T3D_DEBUG)
    #define D3D_REF_COUNT(prefix, p)    \
        if ((p) != nullptr) \
        {   \
            (p)->AddRef();  \
            ULONG RefCount = (p)->Release();    \
            T3D_LOG_INFO(LOG_TAG_D3D11RENDERER, "%s %s - %s [IUnknown Pointer] : 0x%016p, RefCount : %u", prefix, __FUNCTION__, #p, (p), RefCount);    \
        }
#else
    #define D3D_REF_COUNT(prefix, p)    (void)0
#endif

    #define LOG_TAG_D3D11RENDERER        "D3D11Renderer"

    class D3D11ContextBase;
    T3D_DECLARE_SMART_PTR(D3D11ContextBase);
}


#endif  /*__T3D_D3D11_PREREQUISITES_BASE_H__*/
