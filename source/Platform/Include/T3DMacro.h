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

#ifndef __T3D_MACRO_H__
#define __T3D_MACRO_H__

#include "T3DType.h"
#include <assert.h>


#if defined T3D_OS_WINDOWS      // Win32

    #ifdef _DEBUG   // debug
        #define T3D_DEBUG
    #else           // release
        #undef T3D_DEBUG
    #endif

//     #pragma warning(disable:4996)
    #pragma warning(disable:4251)
    #pragma warning(disable:4819)   // UTF-8 encode will trigger warning

    #define snprintf    _snprintf
    #define vsnprintf   _vsnprintf

    #define csrchr     strrchr

	#define T3D_EXPORT_API      __declspec(dllexport)
	#define T3D_IMPORT_API      __declspec(dllimport)

    #define FORCEINLINE __forceinline

#elif defined T3D_OS_IOS || defined T3D_OS_OSX || defined T3D_OS_LINUX  // iOS or Mac OS x or Linux

    #ifdef DEBUG    // debug
        #define T3D_DEBUG
    #else           // release
        #undef T3D_DEBUG
    #endif

    #define stricmp strcasecmp

	#define T3D_EXPORT_API
	#define T3D_IMPORT_API

    #define FORCEINLINE __forceinline

#elif defined T3D_OS_ANDROID    // Android

    #ifdef NDK_DEBUG    // debug
        #define T3D_DEBUG
    #else               // release
        #undef T3D_DEBUG
    #endif

    #define stricmp strcasecmp

	#define T3D_EXPORT_API      __attribute__ ((visibility ("default")))
	#define T3D_IMPORT_API

    #define FORCEINLINE __forceinline

#endif


#ifndef T3D_ASSERT
    #ifdef T3D_DEBUG
        #define T3D_ASSERT(exp)   assert(exp)
    #else
        #define T3D_ASSERT(exp)
    #endif
#endif


#define MK_DWORD(h, l)      (((h<<16)&0xFFFF0000)|(0xFFFF&l))
#define LO_WORD(x)          (x&0xFFFF)
#define HI_WORD(x)          ((x>>16)&0xFFFF)
#define MK_WORD(h, l)       (((h<<8)&0xFF00)|(0xFF&l))
#define LO_BYTE(x)          (x&0xFF)
#define HI_BYTE(x)          ((x&>>8)&0xFF)


#define T3D_DECLARE_INTERFACE(T)    \
    public:     \
        virtual ~T() = default;

//#define T3D_DISABLE_COPY(T) \
//    private:    \
//        T(const T &);   \
//        T &operator =(const T &);

#define T3D_SAFE_DELETE(p)  \
    if ((p) != nullptr)   \
    {   \
        delete (p);   \
        (p) = nullptr;    \
    }

#define T3D_POD_SAFE_DELETE(p)  \
    if ((p) != nullptr)   \
    {   \
        Delete(p);   \
        (p) = nullptr;    \
    }

#define T3D_SAFE_DELETE_ARRAY(p)    \
    if ((p) != nullptr)   \
    {   \
        delete [](p); \
        (p) = nullptr;    \
    }

#define T3D_POD_SAFE_DELETE_ARRAY(p)    \
    if ((p) != nullptr)   \
    {   \
        DeleteArray(p); \
        (p) = nullptr;    \
    }

#define T3D_SAFE_RELEASE(p) \
    if ((p) != nullptr)   \
    {   \
        (p)->release();   \
        (p) = nullptr;    \
    }

#define T3D_SAFE_ACQUIRE(p) \
    if ((p) != nullptr)   \
    {   \
        (p)->acquire();   \
    }


#endif  /*__T3D_MACRO_H__*/
