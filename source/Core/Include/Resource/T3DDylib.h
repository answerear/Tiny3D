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


#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    /**
     * @class   Dylib
     * @brief   A 3D engine api.
     */
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        /**
         * @fn  static DylibPtr Dylib::create(const String &name);
         * @brief   创建 Dylib 对象
         * @param   name    The name.
         * @return  A DylibPtr.
         */
        static DylibPtr create(const String &name);

        /**
         * @fn  virtual Dylib::~Dylib();
         * @brief   析构函数
         */
        ~Dylib() override;

        /**
         * @fn  virtual Type Dylib::getType() const override;
         * @brief   重写 Resource::getType()o
         * @return  The type.
         */
        Type getType() const override;

        /**
         * @fn  virtual void Dylib::*getSymbol(const String &name) const;
         * @brief   根据名称获取对应符号地址
         * @param   name    The name.
         * @return  Null if it fails, else the symbol.
         */
        virtual void *getSymbol(const String &name) const;

    protected:
        /**
         * @fn  Dylib::Dylib(const String &name);
         * @brief   构造函数
         * @param   name    The name.
         */
        Dylib(const String &name);

        ResourcePtr clone() const override;

        TResult onLoad() override;

        TResult onUnload() override;
     
    protected:
        THandle mHandle;    /**< The handle */
    };
}


#endif  /*__T3D_DYLIB_H__*/
