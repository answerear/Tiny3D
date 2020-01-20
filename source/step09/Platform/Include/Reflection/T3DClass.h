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

    enum class AccessType : uint32_t
    {
        PUBLIC = 0,
        PROTECTED,
        PRIVATE
    };

    /**
     * @class   Class
     * @brief   类信息，用于 RTTI.
     */
    class T3D_PLATFORM_API Class
    {
        T3D_DISABLE_COPY(Class);

    public:
        typedef std::map<std::string, Class*> ClassMap;
        typedef std::map<std::string, Field*> FieldMap;
        typedef std::map<std::string, StaticField*> StaticFieldMap;
        typedef std::map<std::string, Method*> MethodMap;
        typedef std::map<std::string, StaticMethod*> StaticMethodMap;
        typedef std::map<std::string, ConstructorMethod*> ConstructorMap;
        typedef std::map<std::string, Property*> PropertyMap;

        /**
         * @fn  static const Class Class::*getClass(const std::string &name);
         * @brief   Gets the class
         * @param   name    The name.
         * @return  Null if it fails, else the class.
         */
        static const Class *getClass(const std::string &name);

        /**
         * @fn  Class::Class(const char *name, size_t s, const Class *base0, 
         *      const Class *base1, const Class *base2, const Class *base3, 
         *      const Class *base4, const Class *base5, const Class *base6, 
         *      const Class *base7);
         * @brief   Constructor
         * @param   name    The name.
         * @param   s       A size_t to process.
         * @param   base0   The base 0.
         * @param   base1   The first base.
         * @param   base2   The second base.
         * @param   base3   The third base.
         * @param   base4   The fourth base.
         * @param   base5   The fifth base.
         * @param   base6   The base 6.
         * @param   base7   The base 7.
         */
        Class(const char *name, size_t s, const Class *base0 = nullptr, 
            const Class *base1 = nullptr, const Class *base2 = nullptr, 
            const Class *base3 = nullptr, const Class *base4 = nullptr, 
            const Class *base5 = nullptr, const Class *base6 = nullptr,
            const Class *base7 = nullptr);

        const char *getName() const;

        /**
         * @fn  size_t Class::getBaseClassCount() const;
         * @brief   Gets base class count
         * @return  The base class count.
         */
        size_t getBaseClassCount() const;

        /**
         * @fn  const Class Class::*getBaseClass(size_t idx) const;
         * @brief   Gets base class
         * @param   idx Zero-based index of the.
         * @return  Null if it fails, else the base class.
         */
        const Class *getBaseClass(size_t idx) const;

        /**
         * @fn  bool Class::isBase() const;
         * @brief   Query if this object is base
         * @return  True if base, false if not.
         */
        bool isBase() const;

        /**
         * @fn  size_t Class::getSize() const;
         * @brief   Gets the size
         * @return  The size.
         */
        size_t getSize() const;

        /**
         * @fn  bool Class::isSameAs(const Class *cls) const;
         * @brief   Query if 'cls' is same as
         * @param   cls The cls.
         * @return  True if same as, false if not.
         */
        bool isSameAs(const Class *cls) const;

        /**
         * @fn  bool Class::isKindOf(const Class *cls) const;
         * @brief   Is kind of the given cls
         * @param   cls The cls.
         * @return  True if kind of, false if not.
         */
        bool isKindOf(const Class *cls) const;

        /**
         * @fn  bool Class::isDerived(const Class *cls) const;
         * @brief   Query if 'cls' is derived
         * @param   cls The cls.
         * @return  True if derived, false if not.
         */
        bool isDerived(const Class *cls) const;

        /**
         * @fn  const FieldMap Class::&getFields() const;
         * @brief   Gets the fields
         * @return  The fields.
         */
        const FieldMap &getFields() const;

        /**
         * @fn  const StaticFieldMap Class::&getStaticFields() const;
         * @brief   Gets static fields
         * @return  The static fields.
         */
        const StaticFieldMap &getStaticFields() const;

        /**
         * @fn  const MethodMap Class::&getMethods() const;
         * @brief   Gets the methods
         * @return  The methods.
         */
        const MethodMap &getMethods() const;

        /**
         * @fn  const StaticMethodMap Class::&getStaticMethods() const;
         * @brief   Gets static methods
         * @return  The static methods.
         */
        const StaticMethodMap &getStaticMethods() const;

        /**
         * @fn  const PropertyMap Class::&getProperties() const;
         * @brief   Gets the properties
         * @return  The properties.
         */
        const PropertyMap &getProperties() const;

        /**
         * @fn  const ConstructorMap Class::&getConstructors() const;
         * @brief   Gets the constructors
         * @return  The constructors.
         */
        const ConstructorMap &getConstructors() const;

        /**
         * @fn  const Field Class::&getField(const char *name, bool searchBase = true) const;
         * @brief   Gets a field
         * @param   name        The name.
         * @param   searchBase  (Optional) True to search base.
         * @return  The field.
         */
        const Field &getField(const char *name, bool searchBase = true) const;

        /**
         * @fn  const StaticField Class::&getStaticField(const char *name, bool searchBase = true) const;
         * @brief   Gets static field
         * @param   name        The name.
         * @param   searchBase  (Optional) True to search base.
         * @return  The static field.
         */
        const StaticField &getStaticField(const char *name, bool searchBase = true) const;

        /**
         * @fn  const Method Class::&getMethod(const char *name, bool searchBase = true) const;
         * @brief   Gets a method
         * @param   name        The name.
         * @param   searchBase  (Optional) True to search base.
         * @return  The method.
         */
        const Method &getMethod(const char *name, bool searchBase = true) const;

        /**
         * @fn  const StaticMethod Class::&getStaticMethod(const char *name, bool searchBase = true) const;
         * @brief   Gets static method
         * @param   name        The name.
         * @param   searchBase  (Optional) True to search base.
         * @return  The static method.
         */
        const StaticMethod &getStaticMethod(const char *name, bool searchBase = true) const;

        /**
         * @fn  const ConstructorMethod Class::&getDefaultConstructor() const;
         * @brief   Gets default constructor
         * @return  The default constructor.
         */
        const ConstructorMethod &getDefaultConstructor() const;

        /**
         * @fn  const Property Class::&getProperty(const char *name, bool searchBase = true) const;
         * @brief   Gets a property
         * @param   name        The name.
         * @param   searchBase  (Optional) True to search base.
         * @return  The property.
         */
        const Property &getProperty(const char *name, bool searchBase = true) const;

        /**
         * @fn  bool Class::hasDefaultConstructor() const;
         * @brief   Query if this object has default constructor
         * @return  True if default constructor, false if not.
         */
        bool hasDefaultConstructor() const;

    protected:
        /**
         * @fn  const ConstructorMethod Class::&defaultConstructor() const;
         * @brief   Default constructor
         * @return  A reference to a const ConstructorMethod.
         */
        const ConstructorMethod &defaultConstructor() const;

    private:
        /**
         * @fn  void Class::addField(Field *field);
         * @brief   Adds a field
         * @param [in,out]  field   If non-null, the field.
         */
        void addField(Field *field);

        /**
         * @fn  void Class::addStaticField(StaticField *field);
         * @brief   Adds a static field
         * @param [in,out]  field   If non-null, the field.
         */
        void addStaticField(StaticField *field);

        /**
         * @fn  void Class::addMethod(Method *method, __callable__ *cb);
         * @brief   Adds a method to 'cb'
         * @param [in,out]  method  If non-null, the method.
         * @param [in,out]  cb      If non-null, the cb.
         */
        void addMethod(Method *method, __callable__ *cb);

        /**
         * @fn  void Class::addStaticMethod(StaticMethod *method, __callable__ *cb);
         * @brief   Adds a static method to 'cb'
         * @param [in,out]  method  If non-null, the method.
         * @param [in,out]  cb      If non-null, the cb.
         */
        void addStaticMethod(StaticMethod *method, __callable__ *cb);

        /**
         * @fn  void Class::addConstructor(ConstructorMethod *method, __callable__ *cb);
         * @brief   Adds a constructor to 'cb'
         * @param           method  The method.
         * @param [in,out]  cb      If non-null, the cb.
         */
        void addConstructor(ConstructorMethod *method, __callable__ *cb);

        /**
         * @fn  void Class::addProperty(Property *prop, __property_base__ *handler);
         * @brief   Adds a property to 'handler'
         * @param [in,out]  prop    If non-null, the property.
         * @param [in,out]  handler If non-null, the handler.
         */
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
        
        FieldMap        mFields;                /**< The fields */
        StaticFieldMap  mStaticFields;          /**< The static fields */
        MethodMap       mMethods;               /**< The methods */
        StaticMethodMap mStaticMethods;         /**< The static methods */
        ConstructorMap  mConstructorMethods;    /**< The constructor methods */
        PropertyMap     mProperties;            /**< The properties */
    };
}


#endif  /*__T3D_CLASS_H__*/
