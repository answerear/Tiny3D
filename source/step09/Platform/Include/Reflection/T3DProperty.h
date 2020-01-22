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


#include "Reflection/T3DMember.h"


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

    /**
     * @class   Property
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API Property : public MemberBase
    {
        friend class Class;
        friend struct __register_property__;

        T3D_DISABLE_COPY(Property);

    public:
        /**
         * @fn  virtual Property::~Property();
         * @brief   Destructor
         */
        virtual ~Property();

        /**
         * @fn  const Class Property::*getClass() const
         * @brief   Gets the class
         * @return  Null if it fails, else the class.
         */
        const Class *getClass() const { return mClass; }

        /**
         * @fn  template <typename Object, typename Value> 
         *      void Property::get(Value &result, const Object *object) const
         * @brief   Gets
         * @tparam  Object  Type of the object.
         * @tparam  Value   Type of the value.
         * @param [in,out]  result  The result.
         * @param           object  The object.
         */
        template <typename Object, typename Value>
        void get(Value &result, const Object *object) const
        {
            const Class *cls = Object::getStaticClass();
            if (cls == nullptr || !getOwner().isDerived(*cls))
                return;
            typedef const __property_handler__<Value> PropertyType;
            PropertyType *prop = dynamic_cast<PropertyType*>(mHandler);
            if (prop == nullptr)
                return;
            result = prop->get(object);
        }

        /**
         * @fn  template <typename Object, typename Value> 
         *      void Property::set(Object *object, const Value &value) const
         * @brief   Sets
         * @tparam  Object  Type of the object.
         * @tparam  Value   Type of the value.
         * @param [in,out]  object  If non-null, the object.
         * @param           value   The value.
         */
        template <typename Object, typename Value>
        void set(Object *object, const Value &value) const
        {
            const Class *cls = Object::getStaticClass();
            if (cls == nullptr || !getOwner().isDerived(*cls))
                return;
            typedef const __property_handler__<Value> PropertyType;
            PropertyType *prop = dynamic_cast<PropertyType *>(mHandler);
            if (prop == nullptr)
                return;
            prop->set(object, value);
        }

    private:
        /**
         * @fn  Property::Property(const Class *owner, const char *type, 
         *      const char *name, const Class *cls);
         * @brief   Constructor
         * @param   owner   The owner.
         * @param   type    The type.
         * @param   name    The name.
         * @param   cls     The cls.
         */
        Property(const Class *owner, const char *type, const char *name, 
            const Class *cls);

    private:
        __property_base__   *mHandler;      /**< The handler */
        const Class         *mClass;        /**< The class */
    };
}


#endif  /*__T3D_PROPERTY_H__*/
