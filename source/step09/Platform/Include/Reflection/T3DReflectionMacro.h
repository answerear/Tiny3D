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
    //joins
    #define REF_JOIN( X, Y ) REF_DO_JOIN( X, Y )
    #define REF_DO_JOIN( X, Y ) REF_DO_JOIN2(X,Y)
    #define REF_DO_JOIN2( X, Y ) X##Y

    #define __IF0(E)
    #define __IF1(E)      _##E
    #define __IF2(E)      _##E
    #define __IF3(E)      _##E
    #define __IF4(E)      _##E
    #define __IF5(E)      _##E
    #define __IF6(E)      _##E
    #define __IF7(E)      _##E
    #define __IF8(E)      _##E
    #define __IF9(E)      _##E
    #define __IF10(E)      _##E
    #define __IF11(E)      _##E
    #define __IF12(E)      _##E
    #define __IF13(E)      _##E
    #define __IF14(E)      _##E
    #define __IF15(E)      _##E
    #define __IF16(E)      _##E
    #define __IF17(E)      _##E
    #define __IF18(E)      _##E
    #define __IF19(E)      _##E
    #define __IF20(E)      _##E
    #define __IF(N,E)      __IF##N(E)



    //repeaters
    #define __REPEAT0(M, C, S)         
    #define __REPEAT1(M, C, S)      _##S M(1)
    #define __REPEAT2(M, C, S)         __REPEAT1(M, C, S)  _##C M(2)
    #define __REPEAT3(M, C, S)         __REPEAT2(M, C, S)  _##C M(3)
    #define __REPEAT4(M, C, S)         __REPEAT3(M, C, S)  _##C M(4)
    #define __REPEAT5(M, C, S)         __REPEAT4(M, C, S)  _##C M(5)
    #define __REPEAT6(M, C, S)         __REPEAT5(M, C, S)  _##C M(6)
    #define __REPEAT7(M, C, S)         __REPEAT6(M, C, S)  _##C M(7)
    #define __REPEAT8(M, C, S)         __REPEAT7(M, C, S)  _##C M(8)
    #define __REPEAT9(M, C, S)         __REPEAT8(M, C, S)  _##C M(9)
    #define __REPEAT10(M, C, S)        __REPEAT9(M, C, S)  _##C M(10)
    #define __REPEAT11(M, C, S)        __REPEAT10(M, C, S) _##C M(11)
    #define __REPEAT12(M, C, S)        __REPEAT11(M, C, S) _##C M(12)
    #define __REPEAT13(M, C, S)        __REPEAT12(M, C, S) _##C M(13)
    #define __REPEAT14(M, C, S)        __REPEAT13(M, C, S) _##C M(14)
    #define __REPEAT15(M, C, S)        __REPEAT14(M, C, S) _##C M(15)
    #define __REPEAT16(M, C, S)        __REPEAT15(M, C, S) _##C M(16)
    #define __REPEAT17(M, C, S)        __REPEAT16(M, C, S) _##C M(17)
    #define __REPEAT18(M, C, S)        __REPEAT17(M, C, S) _##C M(18)
    #define __REPEAT19(M, C, S)        __REPEAT18(M, C, S) _##C M(19)
    #define __REPEAT20(M, C, S)        __REPEAT19(M, C, S) _##C M(20)
    #define __REPEAT(N, M, C, S, E)     __REPEAT##N(M, C, S) __IF##N(E)


    //various defs needed for parameters
    #define __MAX_PARAMS__       20
    #define ___NOTHING__          
    #define ___COMMA__            ,
    #define ___SEMICOLON__        ;
    #define ___TEMPLATE_DECLARE_BEGIN__   template <
    #define ___TEMPLATE_BEGIN__   <
    #define ___TEMPLATE_END__     >
    #define __TEMPLATE_ARG__(N)  class T##N
    #define __TYPE_ARG__(N)      T##N 
    #define __ARG__(N)           T##N t##N
    #define __ARG_PTR__(N)       T##N* t##N
    #define __PARAM__(N)         t##N
    #define __PARAM_PTR__(N)     & __PARAM__(N)
    #define __NOT_VIRTUAL__

    class Class;

    class T3D_PLATFORM_API __callable__
    {
    public:
        typedef std::vector<const Class *> arg_list_type;
        virtual ~__callable__() {}

        virtual int get_args_count() const = 0;
        virtual const arg_list_type &get_args() const = 0;
        virtual const Class *get_ret_type() const = 0;
    };

    //calculates the offset of a field
    #define __OFFSET__(C, M) \
        ((unsigned long)(&((const C *)1024)->M)-1024)

    // parameter for generating 
    #define __TYPE_INFO_ARG__(N)  this->args_list.push_back((__TYPE_ARG__(N))::getStaticClass())

    //callable that implements the argument capturing and reporting
    #define __CALLABLE__ARGS(N) \
        template <int args_size __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        class __callable_args##N##__ : public __callable__ \
        { \
        public: \
            const Class *ret_type; \
	        arg_list_type args_list; \
            __callable_args##N##__(const Class *ret_id) : ret_type(ret_id), args_list() \
            {   \
	            __REPEAT(N, __TYPE_INFO_ARG__, __SEMICOLON__, __NOTHING__, __NOTHING__); \
	        };\
	        virtual int get_args_count () const override { return args_size; } \
	        virtual const arg_list_type &get_args() const override { return args_list; } \
            virtual const Class *get_ret_type() const override { return ret_type; } \
        }


    //callable class macro with return type
    #define __CALLABLE__(N) \
        template <typename R, typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        class __callable##N##__ : public __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        { \
        public: \
            typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)); \
            MethodType method; \
            __callable##N##__(MethodType m) \
                : __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) >(R::getStaticClass()) \
                , method(m) \
            {} \
            R invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
            { \
                return (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
            } \
        };


    //callable class macro with return type and const type
    #define __CALLABLE_CONST__(N) \
        template <typename R, typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        class __callable_const##N##__ : public __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        { \
        public: \
            typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const; \
            MethodType method; \
            __callable_const##N##__(MethodType m) \
                : __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) >(R::getStaticClass()) \
                , method(m) \
            {} \
            R invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
            { \
                return (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
            } \
        };


    //callable class macro with void return type
    #define __CALLABLE_VOID__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        class __callable_void##N##__ : public __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        { \
        public: \
            typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)); \
            MethodType method; \
            __callable_void##N##__(MethodType m) \
                : __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) >(nullptr) \
                , method(m) \
            {} \
            void invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
            { \
                (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
            } \
        };


    //callable class macro with void return type and const type
    #define __CALLABLE_CONST_VOID__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        class __callable_const_void##N##__ : public __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        { \
        public: \
            typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const; \
            MethodType method; \
            __callable_const_void##N##__(MethodType m) \
                : __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) >(nullptr) \
                , method(m) \
            {} \
            void invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
            { \
                (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
            } \
        };


    //static callable class macro with return type
    #define __STATIC_CALLABLE__(N) \
        template <typename R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        class __static_callable##N##__ : public __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        { \
        public: \
            typedef R (*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)); \
            MethodType method; \
            __static_callable##N##__(MethodType m) \
                : __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) >(R::getStaticClass()) \
                , method(m) \
            {} \
            R invoke(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const \
            { \
                return (*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
            } \
        };

    //void version
    #define __STATIC_CALLABLE_VOID__(N) \
        __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __TEMPLATE_DECLARE_BEGIN__, __TEMPLATE_END__) \
        class __static_callable_void##N##__ : public __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        { \
        public: \
            typedef void (*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)); \
            MethodType method; \
            __static_callable_void##N##__(MethodType m) \
                : __callable_args##N##__<N __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__) >(nullptr) \
                , method(m) \
            {} \
            void invoke(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const \
            { \
                (*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
            } \
        };


    //define callables
    __CALLABLE__ARGS(0);
    __CALLABLE__ARGS(1);
    __CALLABLE__ARGS(2);
    __CALLABLE__ARGS(3);
    __CALLABLE__ARGS(4);
    __CALLABLE__ARGS(5);
    __CALLABLE__ARGS(6);
    __CALLABLE__ARGS(7);
    __CALLABLE__ARGS(8);
    __CALLABLE__ARGS(9);
    __CALLABLE__ARGS(10);
    __CALLABLE__ARGS(11);
    __CALLABLE__ARGS(12);
    __CALLABLE__ARGS(13);
    __CALLABLE__ARGS(14);
    __CALLABLE__ARGS(15);
    __CALLABLE__ARGS(16);
    __CALLABLE__ARGS(17);
    __CALLABLE__ARGS(18);
    __CALLABLE__ARGS(19);
    __CALLABLE__ARGS(20);


    //define callables
    __CALLABLE__(0);
    __CALLABLE__(1);
    __CALLABLE__(2);
    __CALLABLE__(3);
    __CALLABLE__(4);
    __CALLABLE__(5);
    __CALLABLE__(6);
    __CALLABLE__(7);
    __CALLABLE__(8);
    __CALLABLE__(9);
    __CALLABLE__(10);
    __CALLABLE__(11);
    __CALLABLE__(12);
    __CALLABLE__(13);
    __CALLABLE__(14);
    __CALLABLE__(15);
    __CALLABLE__(16);
    __CALLABLE__(17);
    __CALLABLE__(18);
    __CALLABLE__(19);
    __CALLABLE__(20);


    //define const callables
    __CALLABLE_CONST__(0);
    __CALLABLE_CONST__(1);
    __CALLABLE_CONST__(2);
    __CALLABLE_CONST__(3);
    __CALLABLE_CONST__(4);
    __CALLABLE_CONST__(5);
    __CALLABLE_CONST__(6);
    __CALLABLE_CONST__(7);
    __CALLABLE_CONST__(8);
    __CALLABLE_CONST__(9);
    __CALLABLE_CONST__(10);
    __CALLABLE_CONST__(11);
    __CALLABLE_CONST__(12);
    __CALLABLE_CONST__(13);
    __CALLABLE_CONST__(14);
    __CALLABLE_CONST__(15);
    __CALLABLE_CONST__(16);
    __CALLABLE_CONST__(17);
    __CALLABLE_CONST__(18);
    __CALLABLE_CONST__(19);
    __CALLABLE_CONST__(20);


    //define void callables
    __CALLABLE_VOID__(0);
    __CALLABLE_VOID__(1);
    __CALLABLE_VOID__(2);
    __CALLABLE_VOID__(3);
    __CALLABLE_VOID__(4);
    __CALLABLE_VOID__(5);
    __CALLABLE_VOID__(6);
    __CALLABLE_VOID__(7);
    __CALLABLE_VOID__(8);
    __CALLABLE_VOID__(9);
    __CALLABLE_VOID__(10);
    __CALLABLE_VOID__(11);
    __CALLABLE_VOID__(12);
    __CALLABLE_VOID__(13);
    __CALLABLE_VOID__(14);
    __CALLABLE_VOID__(15);
    __CALLABLE_VOID__(16);
    __CALLABLE_VOID__(17);
    __CALLABLE_VOID__(18);
    __CALLABLE_VOID__(19);
    __CALLABLE_VOID__(20);


    //define const void callables
    __CALLABLE_CONST_VOID__(0);
    __CALLABLE_CONST_VOID__(1);
    __CALLABLE_CONST_VOID__(2);
    __CALLABLE_CONST_VOID__(3);
    __CALLABLE_CONST_VOID__(4);
    __CALLABLE_CONST_VOID__(5);
    __CALLABLE_CONST_VOID__(6);
    __CALLABLE_CONST_VOID__(7);
    __CALLABLE_CONST_VOID__(8);
    __CALLABLE_CONST_VOID__(9);
    __CALLABLE_CONST_VOID__(10);
    __CALLABLE_CONST_VOID__(11);
    __CALLABLE_CONST_VOID__(12);
    __CALLABLE_CONST_VOID__(13);
    __CALLABLE_CONST_VOID__(14);
    __CALLABLE_CONST_VOID__(15);
    __CALLABLE_CONST_VOID__(16);
    __CALLABLE_CONST_VOID__(17);
    __CALLABLE_CONST_VOID__(18);
    __CALLABLE_CONST_VOID__(19);
    __CALLABLE_CONST_VOID__(20);


    //static callable classes
    __STATIC_CALLABLE__(0)
    __STATIC_CALLABLE__(1)
    __STATIC_CALLABLE__(2)
    __STATIC_CALLABLE__(3)
    __STATIC_CALLABLE__(4)
    __STATIC_CALLABLE__(5)
    __STATIC_CALLABLE__(6)
    __STATIC_CALLABLE__(7)
    __STATIC_CALLABLE__(8)
    __STATIC_CALLABLE__(9)
    __STATIC_CALLABLE__(10)
    __STATIC_CALLABLE__(11)
    __STATIC_CALLABLE__(12)
    __STATIC_CALLABLE__(13)
    __STATIC_CALLABLE__(14)
    __STATIC_CALLABLE__(15)
    __STATIC_CALLABLE__(16)
    __STATIC_CALLABLE__(17)
    __STATIC_CALLABLE__(18)
    __STATIC_CALLABLE__(19)
    __STATIC_CALLABLE__(20)
    __STATIC_CALLABLE_VOID__(0)
    __STATIC_CALLABLE_VOID__(1)
    __STATIC_CALLABLE_VOID__(2)
    __STATIC_CALLABLE_VOID__(3)
    __STATIC_CALLABLE_VOID__(4)
    __STATIC_CALLABLE_VOID__(5)
    __STATIC_CALLABLE_VOID__(6)
    __STATIC_CALLABLE_VOID__(7)
    __STATIC_CALLABLE_VOID__(8)
    __STATIC_CALLABLE_VOID__(9)
    __STATIC_CALLABLE_VOID__(10)
    __STATIC_CALLABLE_VOID__(11)
    __STATIC_CALLABLE_VOID__(12)
    __STATIC_CALLABLE_VOID__(13)
    __STATIC_CALLABLE_VOID__(14)
    __STATIC_CALLABLE_VOID__(15)
    __STATIC_CALLABLE_VOID__(16)
    __STATIC_CALLABLE_VOID__(17)
    __STATIC_CALLABLE_VOID__(18)
    __STATIC_CALLABLE_VOID__(19)
    __STATIC_CALLABLE_VOID__(20)


    //macro of a inline method that accepts a method pointer and creates a callable for it
    #define __CREATE_CALLABLE__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static inline __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        *create(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__))) \
        { \
            return new __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)>(method); \
        }


    //macro of a inline method that accepts a method pointer and creates a const callable for it
    #define __CREATE_CALLABLE_CONST__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static inline __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        *create(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const) \
        { \
            return new __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)>(method); \
        }


    //void version
    #define __CREATE_CALLABLE_VOID__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static inline __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        *create(void (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__))) \
        { \
            return new __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)>(method); \
        }


    //const void version
    #define __CREATE_CALLABLE_CONST_VOID__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static inline __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        *create(void (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const) \
        { \
            return new __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)>(method); \
        }


    //macro to create a static callable
    #define __CREATE_STATIC_CALLABLE__(N) \
        template <typename R1 __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static inline __static_callable##N##__<R1 __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        *create(R1 (*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__))) \
        { \
            return new __static_callable##N##__<R1 __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)>(method); \
        }


    //void version
    #define __CREATE_STATIC_CALLABLE_VOID__(N) \
        __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __TEMPLATE_DECLARE_BEGIN__, __TEMPLATE_END__) \
        static inline __static_callable_void##N##__ __REPEAT(N, __TYPE_ARG__, __COMMA__, __TEMPLATE_BEGIN__, __TEMPLATE_END__) \
        *create(void (*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__))) \
        { \
            return new __static_callable_void##N##__ __REPEAT(N, __TYPE_ARG__, __COMMA__, __TEMPLATE_BEGIN__, __TEMPLATE_END__)(method); \
        }


    //factory method for non-voids
    template <typename R> 
    class __callable_factory__ 
    {
        __CREATE_CALLABLE__(0)
        __CREATE_CALLABLE__(1)
        __CREATE_CALLABLE__(2)
        __CREATE_CALLABLE__(3)
        __CREATE_CALLABLE__(4)
        __CREATE_CALLABLE__(5)
        __CREATE_CALLABLE__(6)
        __CREATE_CALLABLE__(7)
        __CREATE_CALLABLE__(8)
        __CREATE_CALLABLE__(9)
        __CREATE_CALLABLE__(10)
        __CREATE_CALLABLE__(11)
        __CREATE_CALLABLE__(12)
        __CREATE_CALLABLE__(13)
        __CREATE_CALLABLE__(14)
        __CREATE_CALLABLE__(15)
        __CREATE_CALLABLE__(16)
        __CREATE_CALLABLE__(17)
        __CREATE_CALLABLE__(18)
        __CREATE_CALLABLE__(19)
        __CREATE_CALLABLE__(20)
        __CREATE_CALLABLE_CONST__(0)
        __CREATE_CALLABLE_CONST__(1)
        __CREATE_CALLABLE_CONST__(2)
        __CREATE_CALLABLE_CONST__(3)
        __CREATE_CALLABLE_CONST__(4)
        __CREATE_CALLABLE_CONST__(5)
        __CREATE_CALLABLE_CONST__(6)
        __CREATE_CALLABLE_CONST__(7)
        __CREATE_CALLABLE_CONST__(8)
        __CREATE_CALLABLE_CONST__(9)
        __CREATE_CALLABLE_CONST__(10)
        __CREATE_CALLABLE_CONST__(11)
        __CREATE_CALLABLE_CONST__(12)
        __CREATE_CALLABLE_CONST__(13)
        __CREATE_CALLABLE_CONST__(14)
        __CREATE_CALLABLE_CONST__(15)
        __CREATE_CALLABLE_CONST__(16)
        __CREATE_CALLABLE_CONST__(17)
        __CREATE_CALLABLE_CONST__(18)
        __CREATE_CALLABLE_CONST__(19)
        __CREATE_CALLABLE_CONST__(20)
    };


    //factory method for non-voids
    template <> 
    struct __callable_factory__<void> 
    {
        __CREATE_CALLABLE_VOID__(0)
        __CREATE_CALLABLE_VOID__(1)
        __CREATE_CALLABLE_VOID__(2)
        __CREATE_CALLABLE_VOID__(3)
        __CREATE_CALLABLE_VOID__(4)
        __CREATE_CALLABLE_VOID__(5)
        __CREATE_CALLABLE_VOID__(6)
        __CREATE_CALLABLE_VOID__(7)
        __CREATE_CALLABLE_VOID__(8)
        __CREATE_CALLABLE_VOID__(9)
        __CREATE_CALLABLE_VOID__(10)
        __CREATE_CALLABLE_VOID__(11)
        __CREATE_CALLABLE_VOID__(12)
        __CREATE_CALLABLE_VOID__(13)
        __CREATE_CALLABLE_VOID__(14)
        __CREATE_CALLABLE_VOID__(15)
        __CREATE_CALLABLE_VOID__(16)
        __CREATE_CALLABLE_VOID__(17)
        __CREATE_CALLABLE_VOID__(18)
        __CREATE_CALLABLE_VOID__(19)
        __CREATE_CALLABLE_VOID__(20)
        __CREATE_CALLABLE_CONST_VOID__(0)
        __CREATE_CALLABLE_CONST_VOID__(1)
        __CREATE_CALLABLE_CONST_VOID__(2)
        __CREATE_CALLABLE_CONST_VOID__(3)
        __CREATE_CALLABLE_CONST_VOID__(4)
        __CREATE_CALLABLE_CONST_VOID__(5)
        __CREATE_CALLABLE_CONST_VOID__(6)
        __CREATE_CALLABLE_CONST_VOID__(7)
        __CREATE_CALLABLE_CONST_VOID__(8)
        __CREATE_CALLABLE_CONST_VOID__(9)
        __CREATE_CALLABLE_CONST_VOID__(10)
        __CREATE_CALLABLE_CONST_VOID__(11)
        __CREATE_CALLABLE_CONST_VOID__(12)
        __CREATE_CALLABLE_CONST_VOID__(13)
        __CREATE_CALLABLE_CONST_VOID__(14)
        __CREATE_CALLABLE_CONST_VOID__(15)
        __CREATE_CALLABLE_CONST_VOID__(16)
        __CREATE_CALLABLE_CONST_VOID__(17)
        __CREATE_CALLABLE_CONST_VOID__(18)
        __CREATE_CALLABLE_CONST_VOID__(19)
        __CREATE_CALLABLE_CONST_VOID__(20)
    };


    //factory method for static non-voids
    template <typename R>
    struct __static_callable_factory__ 
    {
        __CREATE_STATIC_CALLABLE__(0)
        __CREATE_STATIC_CALLABLE__(1)
        __CREATE_STATIC_CALLABLE__(2)
        __CREATE_STATIC_CALLABLE__(3)
        __CREATE_STATIC_CALLABLE__(4)
        __CREATE_STATIC_CALLABLE__(5)
        __CREATE_STATIC_CALLABLE__(6)
        __CREATE_STATIC_CALLABLE__(7)
        __CREATE_STATIC_CALLABLE__(8)
        __CREATE_STATIC_CALLABLE__(9)
        __CREATE_STATIC_CALLABLE__(10)
        __CREATE_STATIC_CALLABLE__(11)
        __CREATE_STATIC_CALLABLE__(12)
        __CREATE_STATIC_CALLABLE__(13)
        __CREATE_STATIC_CALLABLE__(14)
        __CREATE_STATIC_CALLABLE__(15)
        __CREATE_STATIC_CALLABLE__(16)
        __CREATE_STATIC_CALLABLE__(17)
        __CREATE_STATIC_CALLABLE__(18)
        __CREATE_STATIC_CALLABLE__(19)
        __CREATE_STATIC_CALLABLE__(20)
    };


    //factory method for static voids
    template <> 
    struct __static_callable_factory__<void> 
    {
        __CREATE_STATIC_CALLABLE_VOID__(0)
        __CREATE_STATIC_CALLABLE_VOID__(1)
        __CREATE_STATIC_CALLABLE_VOID__(2)
        __CREATE_STATIC_CALLABLE_VOID__(3)
        __CREATE_STATIC_CALLABLE_VOID__(4)
        __CREATE_STATIC_CALLABLE_VOID__(5)
        __CREATE_STATIC_CALLABLE_VOID__(6)
        __CREATE_STATIC_CALLABLE_VOID__(7)
        __CREATE_STATIC_CALLABLE_VOID__(8)
        __CREATE_STATIC_CALLABLE_VOID__(9)
        __CREATE_STATIC_CALLABLE_VOID__(10)
        __CREATE_STATIC_CALLABLE_VOID__(11)
        __CREATE_STATIC_CALLABLE_VOID__(12)
        __CREATE_STATIC_CALLABLE_VOID__(13)
        __CREATE_STATIC_CALLABLE_VOID__(14)
        __CREATE_STATIC_CALLABLE_VOID__(15)
        __CREATE_STATIC_CALLABLE_VOID__(16)
        __CREATE_STATIC_CALLABLE_VOID__(17)
        __CREATE_STATIC_CALLABLE_VOID__(18)
        __CREATE_STATIC_CALLABLE_VOID__(19)
        __CREATE_STATIC_CALLABLE_VOID__(20)
    };


    //callable generator
    #define __CALLABLE_GENERATOR__(N) \
        template <typename R, typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        inline __callable__ *__create_callable__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__))) \
        { \
            return __callable_factory__<R>::create(method); \
        }


    //const callable generator
    #define __CALLABLE_GENERATOR_CONST__(N) \
        template <typename R, typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        inline __callable__ *__create_callable__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const) \
        { \
            return __callable_factory__<R>::create(method); \
        }


    //static callable generator
    #define __STATIC_CALLABLE_GENERATOR__(N) \
        template <typename R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        inline __callable__ *__create_static_callable__(R (*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__))) \
        { \
            return __static_callable_factory__<R>::create(method); \
        }


    //generators
    __CALLABLE_GENERATOR__(0)
    __CALLABLE_GENERATOR__(1)
    __CALLABLE_GENERATOR__(2)
    __CALLABLE_GENERATOR__(3)
    __CALLABLE_GENERATOR__(4)
    __CALLABLE_GENERATOR__(5)
    __CALLABLE_GENERATOR__(6)
    __CALLABLE_GENERATOR__(7)
    __CALLABLE_GENERATOR__(8)
    __CALLABLE_GENERATOR__(9)
    __CALLABLE_GENERATOR__(10)
    __CALLABLE_GENERATOR__(11)
    __CALLABLE_GENERATOR__(12)
    __CALLABLE_GENERATOR__(13)
    __CALLABLE_GENERATOR__(14)
    __CALLABLE_GENERATOR__(15)
    __CALLABLE_GENERATOR__(16)
    __CALLABLE_GENERATOR__(17)
    __CALLABLE_GENERATOR__(18)
    __CALLABLE_GENERATOR__(19)
    __CALLABLE_GENERATOR__(20)
    __CALLABLE_GENERATOR_CONST__(0)
    __CALLABLE_GENERATOR_CONST__(1)
    __CALLABLE_GENERATOR_CONST__(2)
    __CALLABLE_GENERATOR_CONST__(3)
    __CALLABLE_GENERATOR_CONST__(4)
    __CALLABLE_GENERATOR_CONST__(5)
    __CALLABLE_GENERATOR_CONST__(6)
    __CALLABLE_GENERATOR_CONST__(7)
    __CALLABLE_GENERATOR_CONST__(8)
    __CALLABLE_GENERATOR_CONST__(9)
    __CALLABLE_GENERATOR_CONST__(10)
    __CALLABLE_GENERATOR_CONST__(11)
    __CALLABLE_GENERATOR_CONST__(12)
    __CALLABLE_GENERATOR_CONST__(13)
    __CALLABLE_GENERATOR_CONST__(14)
    __CALLABLE_GENERATOR_CONST__(15)
    __CALLABLE_GENERATOR_CONST__(16)
    __CALLABLE_GENERATOR_CONST__(17)
    __CALLABLE_GENERATOR_CONST__(18)
    __CALLABLE_GENERATOR_CONST__(19)
    __CALLABLE_GENERATOR_CONST__(20)
    __STATIC_CALLABLE_GENERATOR__(0)
    __STATIC_CALLABLE_GENERATOR__(1)
    __STATIC_CALLABLE_GENERATOR__(2)
    __STATIC_CALLABLE_GENERATOR__(3)
    __STATIC_CALLABLE_GENERATOR__(4)
    __STATIC_CALLABLE_GENERATOR__(5)
    __STATIC_CALLABLE_GENERATOR__(6)
    __STATIC_CALLABLE_GENERATOR__(7)
    __STATIC_CALLABLE_GENERATOR__(8)
    __STATIC_CALLABLE_GENERATOR__(9)
    __STATIC_CALLABLE_GENERATOR__(10)
    __STATIC_CALLABLE_GENERATOR__(11)
    __STATIC_CALLABLE_GENERATOR__(12)
    __STATIC_CALLABLE_GENERATOR__(13)
    __STATIC_CALLABLE_GENERATOR__(14)
    __STATIC_CALLABLE_GENERATOR__(15)
    __STATIC_CALLABLE_GENERATOR__(16)
    __STATIC_CALLABLE_GENERATOR__(17)
    __STATIC_CALLABLE_GENERATOR__(18)
    __STATIC_CALLABLE_GENERATOR__(19)
    __STATIC_CALLABLE_GENERATOR__(20)


    //static new instance generator
    #define __NEWINSTANCE_CALLER__(CNAME, N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__) > \
        static C *__new_instance__(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__, __NOTHING__)) \
        { \
            return new CNAME (__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
        }

    #define __NEWINSTANCE_PTR_CALLER__(CNAME, N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static C *__new_instance_ptr__(void* ptr __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) \
        { \
            return new (ptr) CNAME (__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
        } \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        static C * (*__create_new_instance_ptr__(C* (*) (__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__, __NOTHING__)))) \
        (void * __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)) \
        { \
            return  & __new_instance_ptr__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)>; \
        }

    //macro that defines an 'invoke' method with a return type
    #define __INVOKE__(N) \
        template <typename R, typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        void invoke(R &result, C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
        { \
            if (getAccess() != AccessType::PUBLIC) return; \
            typedef const __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> CallableType1; \
            typedef const __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> CallableType2; \
            CallableType1 *cb1 = dynamic_cast<CallableType1 *>(mCallable);\
            if (cb1 != nullptr) \
            { \
                result = cb1->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__, __NOTHING__)); \
                return; \
            } \
            CallableType2 *cb2 = dynamic_cast<CallableType2 *>(mCallable); \
            if (cb2 != nullptr) \
            { \
                result = cb2->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__, __NOTHING__)); \
                return; \
            } \
        }


    //macro that defines an 'invoke' method without a return type
    #define __INVOKE_VOID__(N) \
        template <typename C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        void invokeVoid(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
        { \
            if (getAccess() != AccessType::PUBLIC) return; \
            typedef const __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> CallableType1; \
            typedef const __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> CallableType2; \
            CallableType1 *cb1 = dynamic_cast<CallableType1 *>(mCallable);\
            if (cb1 != nullptr) \
            { \
                cb1->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__, __NOTHING__)); \
                return; \
            } \
            CallableType2 *cb2 = dynamic_cast<CallableType2 *>(mCallable); \
            if (cb2 != nullptr) \
            { \
                cb2->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__, __NOTHING__)); \
                return; \
            } \
        }



    //static invoke non-void method
    #define __STATIC_INVOKE__(N, METHOD_NAME, CALLABLE) \
        template <typename R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        void METHOD_NAME(R &result __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
        { \
            if (getAccess() != AccessType::PUBLIC) return; \
            typedef const __static_callable##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> CallableType; \
            CallableType *cb = dynamic_cast<CallableType *>(CALLABLE); \
            if (cb != nullptr) \
            { \
                result = cb->invoke(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
                return; \
            } \
        }

    //static invoke non-void method
    #define __CONSTRUCTOR_PLACEMENT_INVOKE__(N) \
        template <typename R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__, __NOTHING__)> \
        void invokePlacement(R &result, void* ptr __REPEAT(N, __ARG__, __COMMA__, __COMMA__, __NOTHING__)) const \
        { \
            if (mPlacementCallable == 0) return; \
            this->_invoke_placement(result, ptr __REPEAT(N, __PARAM__, __COMMA__, __COMMA__, __NOTHING__)); \
        }

    //static invoke void method with 0 params
    #define __STATIC_INVOKE_VOID__0 __STATIC_INVOKE_VOID__(0)

    //static invoke void method
    #define __STATIC_INVOKE_VOID__(N)\
        __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __TEMPLATE_DECLARE_BEGIN__, __TEMPLATE_END__) \
        void invokeVoid(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__, __NOTHING__)) const \
        { \
            if (getAccess() != AccessType::PUBLIC) return; \
            typedef const __static_callable_void##N##__ __REPEAT(N, __TYPE_ARG__, __COMMA__, __TEMPLATE_BEGIN__, __TEMPLATE_END__) CallableType; \
            CallableType *cb = dynamic_cast<CallableType *>(mCallable); \
            if (cb != nullptr) \
            { \
                cb->invoke(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__, __NOTHING__)); \
                return; \
            } \
        }

    #define ACCESS_public        Access::PUBLIC
    #define ACCESS_protected     Access::PROTECTED
    #define ACCESS_private       Access::PRIVATE

    #define __CREATE_OBJECT__(CLASS_NAME)  \
        __NEWINSTANCE_CALLER__(CLASS_NAME,0); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,1); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,2); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,3); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,4); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,5); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,6); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,7); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,8); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,9); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,10); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,11); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,12); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,13); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,14); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,15); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,16); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,17); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,18); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,19); \
        __NEWINSTANCE_CALLER__(CLASS_NAME,20); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,0); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,1); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,2); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,3); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,4); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,5); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,6); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,7); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,8); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,9); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,10); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,11); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,12); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,13); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,14); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,15); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,16); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,17); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,18); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,19); \
        __NEWINSTANCE_PTR_CALLER__(CLASS_NAME,20); \


    #define __PROPERTY__(TYPE, NAME) \
        private: \
            template <typename T> \
            class REF_JOIN(__property__, NAME) \
            { \
            public: \
                typedef REF_JOIN(__property__, NAME) <T> Type; \
                REF_JOIN(__property__, NAME)<T>() \
                { \
                    static __register_property__ reg(__create_property__<ClassType, T>(&ClassType::get_##NAME, &ClassType::set_##NAME), getStaticClass(), #TYPE, #NAME, TYPE::getCalssStatic()); \
                } \
                inline TYPE get() const \
                { \
                    return owner()->get_##NAME(); \
                } \
                inline void set(TYPE value) \
                { \
                    owner()->set_##NAME(value); \
                } \
                inline operator TYPE () const \
                { \
                    return get(); \
                } \
                inline bool operator == (TYPE value) const \
                { \
                    return get() == value; \
                } \
                inline bool operator != (TYPE value) const \
                { \
                    return get() != value; \
                } \
                inline bool operator < (TYPE value) const \
                { \
                    return get() < value; \
                } \
                inline bool operator > (TYPE value) const \
                { \
                    return get() > value; \
                } \
                inline bool operator <= (TYPE value) const \
                { \
                    return get() <= value; \
                } \
                inline bool operator >= (TYPE value) const \
                { \
                    return get() >= value; \
                } \
                inline Type &operator = (TYPE value) \
                { \
                    set(value); \
                    return *this; \
                } \
                inline Type &operator = (const Type &prop) \
                { \
                    if (&prop != this) set(prop.get()); \
                    return *this; \
                } \
                inline Type &operator += (TYPE value) \
                { \
                    set(get() + value); \
                    return *this; \
                } \
                inline Type &operator -= (TYPE value) \
                { \
                    set(get() - value); \
                    return *this; \
                } \
                inline Type &operator *= (TYPE value) \
                { \
                    set(get() * value);\
                    return *this;\
                } \
                inline Type &operator /= (TYPE value) \
                { \
                    set(get() / value); \
                    return *this; \
                } \
                inline Type &operator %= (TYPE value) \
                { \
                    set(get() % value); \
                    return *this; \
                } \
                inline Type &operator <<= (int bits) \
                { \
                    set(get() << bits); \
                    return *this; \
                } \
                inline Type &operator >>= (int bits) \
                { \
                    set(get() >> bits); \
                    return *this; \
                } \
                inline Type &operator &= (TYPE value) \
                { \
                    set(get() & value); \
                    return *this; \
                } \
                inline Type &operator |= (TYPE value) \
                { \
                    set(get() | value); \
                    return *this; \
                } \
                inline Type &operator ^= (TYPE value) \
                { \
                    set(get() ^ value); \
                    return *this; \
                } \
                inline TYPE operator ~() \
                { \
                    return ~get(); \
                } \
                inline TYPE operator ++() \
                { \
                    TYPE val = get(); \
                    set(val + 1); \
                    return val; \
                } \
                inline TYPE operator ++(int i) \
                { \
                    TYPE val = get(); \
                    set(val + 1); \
                    return get(); \
                } \
                inline TYPE operator --() \
                { \
                    TYPE val = get(); \
                    set(val - 1); \
                    return val; \
                } \
                inline TYPE operator --(int i) \
                { \
                    TYPE val = get(); \
                    set(val - 1); \
                    return get(); \
                } \
            private: \
                ClassType *owner() \
                { \
                    return (ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME)); \
                } \
                const ClassType *owner() const \
                { \
                    return (const ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME)); \
                } \
            }; \
            friend class REF_JOIN(__property__,NAME)< TYPE >; \
        public: \
            REF_JOIN(__property__,NAME)< TYPE > NAME;


    #define __FIELD_LINE__(ACCESS_ATTR, FIELD_TYPE, FIELD_NAME, LINE) \
        private: \
            struct REF_JOIN(__field_##FIELD_NAME##__, LINE) \
            { \
                REF_JOIN(__field_##FIELD_NAME##__, LINE)() \
                { \
                    static __register_field__ reg(__OFFSET__(ClassType, FIELD_NAME), FIELD_TYPE::getStaticClass(), getStaticClass(), ACCESS_##ACCESS_ATTR, #FIELD_TYPE, #FIELD_NAME); \
                } \
            } REF_JOIN(__field_##FIELD_NAME##__, LINE); \
            friend struct REF_JOIN(__field_##FIELD_NAME##__, LINE); \
        ACCESS_ATTR: \
            FIELD_TYPE FIELD_NAME;


    #define __STATIC_FIELD_LINE__(ACCESS_ATTR, FIELD_TYPE, FIELD_NAME, LINE) \
        private: \
            struct REF_JOIN(__static_field_##FIELD_NAME##__, LINE) \
            { \
                REF_JOIN(__static_field_##FIELD_NAME##__, LINE)() \
                { \
                    static __register_static_field__ reg((void *)&FIELD_NAME, FIELD_TYPE::getStaticClass(), getStaticClass(), ACCESS_##ACCESS_ATTR, #FIELD_TYPE, #FIELD_NAME); \
                } \
            } REF_JOIN(__static_field_##FIELD_NAME##__, LINE); \
            friend struct REF_JOIN(__static_field_##FIELD_NAME##__, LINE); \
        ACCESS_ATTR: \
            static FIELD_TYPE FIELD_NAME;


    #define __METHOD_LINE__(ACCESS_ATTR, VIRTUAL, RETURN_TYPE, METHOD_NAME, METHOD_ARGS, LINE) \
        private: \
            struct REF_JOIN(__method_##METHOD_NAME##__,LINE) \
            { \
                REF_JOIN(__method_##METHOD_NAME##__,LINE) () \
                { \
                    typedef RETURN_TYPE (ClassType::*method_ptr_type) METHOD_ARGS; \
                    method_ptr_type method_ptr = & ClassType::METHOD_NAME; \
                    static __register_method__ reg(__create_callable__(method_ptr), getStaticClass(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS, #VIRTUAL); \
                } \
            } REF_JOIN(__method_##METHOD_NAME##__,LINE); \
            friend struct REF_JOIN(__method_##METHOD_NAME##__,LINE); \
        ACCESS_ATTR: \
            VIRTUAL RETURN_TYPE METHOD_NAME METHOD_ARGS;

    #define __STATIC_METHOD_LINE__(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS, LINE) \
        private: \
            struct REF_JOIN(__static_method_##METHOD_NAME##__,LINE) \
            { \
                REF_JOIN(__static_method_##METHOD_NAME##__,LINE)() \
                { \
                    typedef RETURN_TYPE (* method_fun_ptr) METHOD_ARGS; \
                    method_fun_ptr fun_ptr = & ClassType::METHOD_NAME ; \
                    static __register_static_method__ reg(__create_static_callable__(fun_ptr), getStaticClass(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS); \
                } \
            } REF_JOIN(__static_method_##METHOD_NAME##__,LINE); \
            friend struct REF_JOIN(__static_method_##METHOD_NAME##__,LINE); \
        ACCESS_ATTR: \
            static RETURN_TYPE METHOD_NAME METHOD_ARGS;


    #define __CONSTRUCTOR_LINE__(ACCESS_ATTR, RETURN_TYPE, CLASS_NAME, METHOD_ARGS, LINE) \
        private: \
            struct REF_JOIN(__constructor_##CLASS_NAME##__,LINE) \
            { \
                REF_JOIN(__constructor_##CLASS_NAME##__,LINE) () \
                { \
                    typedef RETURN_TYPE (*new_instance_fun_ptr) METHOD_ARGS; \
                    new_instance_fun_ptr fun_ptr = & ClassType :: __new_instance__; \
                    static __register_constructor_method__ reg(__create_static_callable__(fun_ptr), __create_static_callable__(__create_new_instance_ptr__(fun_ptr)), getStaticClass(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #CLASS_NAME, #METHOD_ARGS); \
                } \
            } REF_JOIN(__constructor_##CLASS_NAME##__,LINE); \
            friend struct REF_JOIN(__constructor_##CLASS_NAME##__,LINE); \
        ACCESS_ATTR: \
            CLASS_NAME METHOD_ARGS;

}


#endif  /*__T3D_REFLECTION_MACRO_H__*/
