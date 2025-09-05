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

#ifndef __T3D_MEMORY_H__
#define __T3D_MEMORY_H__


#include "Memory/T3DMemoryManager.h"


#if defined T3D_USE_CUSTOM_NEW
#define T3D_NEW new
#define T3D_DELETE delete
#define T3D_POD_NEW(T) New<T>()
#define T3D_POD_NEW_ARRAY(T, size) NewArray<T>(size)
#define T3D_POD_DELETE(ptr) Delete(ptr)
#define T3D_POD_DELETE_ARRAY(ptr) DeleteArray(ptr)
// #define new CUSTOM_NEW
#else
#define T3D_NEW new
#define T3D_DELETE delete
#define T3D_POD_NEW(T) new T
#define T3D_POD_NEW_ARRAY(T, size) new T[size]
#define T3D_POD_DELETE(ptr) delete ptr
#define T3D_POD_DELETE_ARRAY(ptr) delete[] ptr
#endif


#endif  /*__T3D_MEMORY_H__*/
