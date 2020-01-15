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


namespace Tiny3D
{
    #define T3D_DECLARE_CLASS()

    #define T3D_IMPLEMENT_CLASS_NO_BASE(cls)
    #define T3D_IMPLEMENT_CLASS_BASE_1(cls, base)
    #define T3D_IMPLEMENT_CLASS_BASE_2(cls, base1, base2)
    #define T3D_IMPLEMENT_CLASS_BASE_3(cls, base1, base2, base3)
    #define T3D_IMPLEMENT_CLASS_BASE_4(cls, base1, base2, base3, base4)
    #define T3D_IMPLEMENT_CLASS_BASE_5(cls, base1, base2, base3, base4, base5)
    #define T3D_IMPLEMENT_CLASS_BASE_6(cls, base1, base2, base3, base4, base5, base6)
    #define T3D_IMPLEMENT_CLASS_BASE_7(cls, base1, base2, base3, base4, base5, base6, base7)
    #define T3D_IMPLEMENT_CLASS_BASE_8(cls, base1, base2, base3, base4, base5, base6, base7, base8)

    #define T3D_PROPERTY()

    #define T3D_FIELD()

    #define T3D_STATIC_METHOD()

    #define T3D_METHOD()
}


#endif  /*__T3D_REFLECTION_H__*/
