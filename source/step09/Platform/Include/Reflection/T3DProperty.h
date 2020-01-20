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


#ifndef __T3D_PROPERTY_H__
#define __T3D_PROPERTY_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    //property handler base
    class T3D_PLATFORM_API __property_base__
    {
    public:
        virtual ~__property_base__() 
        {
        }
    };


    //property handler
    template <typename T>
    class __property_handler__ : public __property_base__ 
    {
    public:
        //get
        virtual T get(const void *object) const = 0;

        //set
        virtual void set(void *object, T value) const = 0;
    };


    //property handler class
    template <typename C, typename T>
    class __property__ : public __property_handler__<T> 
    {
    public:
        //type of getters/setters
        typedef T(C::*Getter)() const;
        typedef void (C::*Setter)(T);

        //pointer to member getters/setters
        Getter getter;
        Setter setter;

        //default constructor
        __property__(Getter g, Setter s) : getter(g), setter(s) 
        {}

        //get
        virtual T get(const void *object) const 
        {
            const C *o = (const C *)(object);
            return (o->*getter)();
        }

        //set
        virtual void set(void *object, T value) const 
        {
            C *o = (C *)(object);
            (o->*setter)(value);
        }
    };

    template <typename C, typename T>
    __property_base__* __create_property__(T(C::*Getter)() const, void (C::*Setter)(T)) 
    {
        return new __property__<C, T>(Getter, Setter);
    }

    class T3D_PLATFORM_API Property
    {
        T3D_DISABLE_COPY(Property);
    };

}


#endif  /*__T3D_PROPERTY_H__*/
