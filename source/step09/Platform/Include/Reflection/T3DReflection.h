/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#ifndef __T3D_REFLECTION_H__
#define __T3D_REFLECTION_H__


#include "Reflection/T3DClass.h"
#include "Reflection/T3DField.h"
#include "Reflection/T3DProperty.h"
#include "Reflection/T3DMethod.h"
#include "Reflection/T3DReflectionMacro.h"


namespace Tiny3D
{
    #define T3D_DECLARE_CLASS() \
        public: \
            static const Class *getStaticClass() { return &msClass; } \
            virtual const Class *getClass() const { return &msClass; } \
        private: \
            static const Class msClass;

    #define T3D_IMPLEMENT_CLASS_NO_BASE(cls) \
        const Class cls::msClass(#cls, sizeof(cls), 0);

    #define T3D_IMPLEMENT_CLASS_BASE_1(cls, base) \
        const Class cls::msClass(#cls, sizeof(cls), 1, base::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_2(cls, base1, base2) \
        const Class cls::msClass(#cls, sizeof(cls), 2, \
            base1::getStaticClass(), base2::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_3(cls, base1, base2, base3) \
        const Class cls::msClass(#cls, sizeof(cls), 3, \
            base1::getStaticClass(), base2::getStaticClass(), \
            base3::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_4(cls, base1, base2, base3, base4) \
        const Class cls::msClass(#cls, sizeof(cls), 4, \
            base1::getStaticClass(), base2::getStaticClass(), \
            base3::getStaticClass(), base4::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_5(cls, base1, base2, base3, base4, base5) \
        const Class cls::msClass(#cls, sizeof(cls), 5, \
            base1::getStaticClass(), base2::getStaticClass(), \
            base3::getStaticClass(), base4::getStaticClass(), \
            base5::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_6(cls, base1, base2, base3, base4, base5, base6) \
        const Class cls::msClass(#cls, sizeof(cls), 6, \
            base1::getStaticClass(), base2::getStaticClass(), \
            base3::getStaticClass(), base4::getStaticClass(), \
            base5::getStaticClass(), base6::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_7(cls, base1, base2, base3, base4, base5, base6, base7) \
        const Class cls::msClass(#cls, sizeof(cls), 7, \
            base1::getStaticClass(), base2::getStaticClass(), \
            base3::getStaticClass(), base4::getStaticClass(), \
            base5::getStaticClass(), base6::getStaticClass(), \
            base7::getStaticClass());

    #define T3D_IMPLEMENT_CLASS_BASE_8(cls, base1, base2, base3, base4, base5, base6, base7, base8) \
        const Class cls::msClass(#cls, sizeof(cls), 8, \
            base1::getStaticClass(), base2::getStaticClass(), \
            base3::getStaticClass(), base4::getStaticClass(), \
            base5::getStaticClass(), base6::getStaticClass(), \
            base7::getStaticClass(), base8::getStaticClass());

    #define T3D_REFLECT_CLASS(cls) \
        private: \
            __CREATE_OBJECT__(cls);

    #define T3D_PROPERTY(type, name) __PROPERTY__(type, name)

    #define T3D_FIELD(access, type, name) __FIELD_LINE__(access, type, name, __LINE__)

    #define T3D_STATIC_FIELD(access, type, name) __STATIC_FIELD_LINE__(access, type, name, __LINE__)

    #define T3D_STATIC_METHOD(access, ret_type, name, args) __STATIC_METHOD_LINE__(access, ret_type, name, args, __LINE__)
    #define T3D_STATIC_METHOD_N(access, ret_type, name, args, num) __STATIC_METHOD_LINE__(access, ret_type, name, args, num)

    #define T3D_METHOD(access, ret_type, name, args) __METHOD_LINE__(access, __NOT_VIRTUAL__, ret_type, name, args, __LINE__)
    #define T3D_METHOD_N(access, ret_type, name, args, num) __METHOD_LINE__(access, __NOT_VIRTUAL__, ret_type, name, args, num)

    #define T3D_VIRTUAL_METHOD(access, ret_type, name, args) __METHOD_LINE__(access, virtual, ret_type, name, args, __LINE__)
    #define T3D_VRITUAL_METHOD_N(access, ret_type, name, args, num) __METHOD_LINE__(access, virtual, ret_type, name, args, num)

    #define T3D_CONSTRUCTOR(access, cls, args)  __CONSTRUCTOR_LINE__(access, cls*, cls, args, __LINE__)
    #define T3D_CONSTRUCTOR_N(access, cls, args, num) __CONSTRUCTOR_LINE__(access, cls*, cls, args, num)
}


#endif  /*__T3D_REFLECTION_H__*/
