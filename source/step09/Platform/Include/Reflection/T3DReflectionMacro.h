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


#ifndef __T3D_REFLECTION_MACRO_H__
#define __T3D_REFLECTION_MACRO_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    #define REF_JOIN(X, Y)  X##Y

    //repeaters
    #define __REPEAT0(M, C, S)         
    #define __REPEAT1(M, C, S)         S M(1)
    #define __REPEAT2(M, C, S)         __REPEAT1(M, C, S)  C M(2)
    #define __REPEAT3(M, C, S)         __REPEAT2(M, C, S)  C M(3)
    #define __REPEAT4(M, C, S)         __REPEAT3(M, C, S)  C M(4)
    #define __REPEAT5(M, C, S)         __REPEAT4(M, C, S)  C M(5)
    #define __REPEAT6(M, C, S)         __REPEAT5(M, C, S)  C M(6)
    #define __REPEAT7(M, C, S)         __REPEAT6(M, C, S)  C M(7)
    #define __REPEAT8(M, C, S)         __REPEAT7(M, C, S)  C M(8)
    #define __REPEAT9(M, C, S)         __REPEAT8(M, C, S)  C M(9)
    #define __REPEAT10(M, C, S)        __REPEAT9(M, C, S)  C M(10)
    #define __REPEAT11(M, C, S)        __REPEAT10(M, C, S) C M(11)
    #define __REPEAT12(M, C, S)        __REPEAT11(M, C, S) C M(12)
    #define __REPEAT13(M, C, S)        __REPEAT12(M, C, S) C M(13)
    #define __REPEAT14(M, C, S)        __REPEAT13(M, C, S) C M(14)
    #define __REPEAT15(M, C, S)        __REPEAT14(M, C, S) C M(15)
    #define __REPEAT16(M, C, S)        __REPEAT15(M, C, S) C M(16)
    #define __REPEAT17(M, C, S)        __REPEAT16(M, C, S) C M(17)
    #define __REPEAT18(M, C, S)        __REPEAT17(M, C, S) C M(18)
    #define __REPEAT19(M, C, S)        __REPEAT18(M, C, S) C M(19)
    #define __REPEAT20(M, C, S)        __REPEAT19(M, C, S) C M(20)
    #define __REPEAT(N, M, C, S)       __REPEAT##N(M, C, S)


    //various defs needed for parameters
    #define __MAX_PARAMS__       20
    #define __NOTHING__          
    #define __COMMA__            ,
    #define __TEMPLATE_ARG__(N)  class T##N
    #define __TYPE_ARG__(N)      T##N 
    #define __ARG__(N)           T##N t##N
    #define __PARAM__(N)         t##N
    #define __NOT_VIRTUAL__


    //calculates the offset of a field
    #define __OFFSET__(C, M) \
            ((unsigned long)(&((const C *)0)->M))

    //callable class macro with return type
    #define __CALLABLE__(N)\
        template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable##N##__ : public __callable__ {\
            typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
            MethodType method;\
            __callable##N##__(MethodType m) : method(m) {\
            }\
            R invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
                return (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            }\
        };


    //callable class macro with return type and const type
    #define __CALLABLE_CONST__(N)\
        template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable_const##N##__ : public __callable__ {\
            typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const;\
            MethodType method;\
            __callable_const##N##__(MethodType m) : method(m) {\
            }\
            R invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
                return (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            }\
        };


    //callable class macro with void return type
    #define __CALLABLE_VOID__(N)\
        template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable_void##N##__ : public __callable__ {\
            typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
            MethodType method;\
            __callable_void##N##__(MethodType m) : method(m) {\
            }\
            void invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
                (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            }\
        };


    //callable class macro with void return type and const type
    #define __CALLABLE_CONST_VOID__(N)\
        template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable_const_void##N##__ : public __callable__ {\
            typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const;\
            MethodType method;\
            __callable_const_void##N##__(MethodType m) : method(m) {\
            }\
            void invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
                (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            }\
        };


    //static callable class macro with return type
    #define __STATIC_CALLABLE__(N)\
        template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __static_callable##N##__ : public __callable__ {\
            typedef R (*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
            MethodType method;\
            __static_callable##N##__(MethodType m) : method(m) {\
            }\
            R invoke(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__)) const {\
                return (*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            }\
        };

}


#endif  /*__T3D_REFLECTION_MACRO_H__*/
