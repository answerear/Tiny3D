/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        /** 创建 Dylib 对象 */
        static DylibPtr create(const String &name);

        /** 析构函数 */
        virtual ~Dylib();

        /** 重写 Resource::getType() */
        virtual Type getType() const override;

        /** 根据名称获取对应符号地址 */
        virtual void *getSymbol(const String &name) const;

    protected:
        /** 构造函数 */
        Dylib(const String &name);

        /** 重写 Resource::load() */
        virtual TResult load() override;

        /** 重写 Resource::unlaod() */
        virtual TResult unload() override;

        /** 重写 Resource::clone() */
        virtual ResourcePtr clone() const override;

    protected:
        THandle mHandle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
