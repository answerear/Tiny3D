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


#ifndef __T3D_CLASS_H__
#define __T3D_CLASS_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class __callable__;
    class __property_base__;

    class T3D_PLATFORM_API Class
    {
        T3D_DISABLE_COPY(Class);

    public:
        typedef std::map<std::string, Class*> ClassMap;
        typedef std::map<std::string, Field*> FieldMap;
        typedef std::map<std::string, StaticField*> StaticFieldMap;
        typedef std::map<std::string, Method*> MethodMap;
        typedef std::map<std::string, StaticMethod*> StaticMethodMap;
        typedef std::map<std::string, Property*> PropertyMap;

        static const Class *getClass(const std::string &name);

        Class(const char *name, size_t s, const Class *base);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1, const Class *base2);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1, const Class *base2, const Class *base3);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1, const Class *base2, const Class *base3,
            const Class *base4);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1, const Class *base2, const Class *base3,
            const Class *base4, const Class *base5);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1, const Class *base2, const Class *base3,
            const Class *base4, const Class *base5, const Class *base6);

        Class(const char *name, size_t s, const Class *base0, 
            const Class *base1, const Class *base2, const Class *base3, 
            const Class *base4, const Class *base5, const Class *base6, 
            const Class *base7);

    private:
        void addField(Field *field);
        void addStaticField(StaticField *field);
        void addMethod(Method *method, __callable__ *cb);
        void addStaticMethod(StaticMethod *method, __callable__ *cb);
        void addConstructor(ConstructorMethod *method, __callable__ *cb);
        void addProperty(Property *prop, __property_base__ *handler);

    private:
        friend struct __register_field__;
        friend struct __register_static_field__;
        friend struct __register_method__;
        friend struct __register_static_method__;
        friend struct __register_constructor_method__;
        friend struct __register_property__;

        static const size_t MAX_BASE_CLASS_COUNT = 8;

        static ClassMap msClasses;  /**< The milliseconds classes */

        const char      *mName;     /**< The name */
        const size_t    mSize;      /**< The size */

        const size_t    mBaseClassCount;                        /**< Number of base class */
        const Class     *mBaseClasses[MAX_BASE_CLASS_COUNT];    /**< The base classes[ maximum base class count] */
        
        FieldMap        mFields;        /**< The fields */
        StaticFieldMap  mStaticFields;  /**< The static fields */
        MethodMap       mMethods;       /**< The methods */
        StaticMethodMap mStaticMethods; /**< The static methods */
        PropertyMap     mProperties;    /**< The properties */
    };
}


#endif  /*__T3D_CLASS_H__*/
