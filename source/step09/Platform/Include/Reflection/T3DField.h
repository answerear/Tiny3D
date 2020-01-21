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


#ifndef __T3D_FIELD_H__
#define __T3D_FIELD_H__


#include "Reflection/T3DMember.h"


namespace Tiny3D
{
    /**
     * @class   Field
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API Field : public MemberBase
    {
        T3D_DISABLE_COPY(Field);

        friend class Class;
        friend struct __resiter_field__;

    public:
        /**
         * @fn  const Class Field::*getClass() const
         * @brief   Gets the class
         * @return  Null if it fails, else the class.
         */
        const Class *getClass() const { return mClass; }

        /**
         * @fn  template <typename Object, typename Value> 
         *      void Field::get(Value &result, Object *object) const
         * @brief   Gets
         * @tparam  Object  Type of the object.
         * @tparam  Value   Type of the value.
         * @param [in,out]  result  The result.
         * @param [in,out]  object  If non-null, the object.
         */
        template <typename Object, typename Value>
        void get(Value &result, Object *object) const
        {
            // 访问权限不合法
            if (getAccess() != AccessType::PUBLIC)
                return;
            // 对象类型跟 Field 所属的类类型不一致
            const Class *cls = Object::getStaticClass();
            if (cls == nullptr || !getOwner().isDerived(*cls))
                return;
            // 值类型跟 Field 类型不一致
            if (Value::getStaticClass() != mClass)
                return;
            result = *(const Value *)(((const char *)object) + mOffset);
        }

        /**
         * @fn  template <typename Object, typename Value> 
         *      void Field::set(Object *object, const Value &value) const
         * @brief   Sets
         * @tparam  Object  Type of the object.
         * @tparam  Value   Type of the value.
         * @param [in,out]  object  If non-null, the object.
         * @param           value   The value.
         */
        template <typename Object, typename Value>
        void set(Object *object, const Value &value) const
        {
            // 访问权限不合法
            if (getAccess() != AccessType::PUBLIC)
                return;
            // 对象类型跟 Field 所属的类类型不一致
            const Class *cls = Object::getStaticClass();
            if (cls == nullptr || !getOwner().isDerived(*cls))
                return;
            // 值类型跟 Field 类型不一致
            if (Value::getStaticClass() != mClass)
                return;
            *(Value *)(((char *)object) + mOffset) = value;
        }

    private:
        /**
         * @fn  Field::Field(uint32_t offset, const Class *clsField, 
         *      const Class *cls, AccessType acess, const char *type, 
         *      const char *name);
         * @brief   Constructor
         * @param   offset      The offset.
         * @param   cls         The cls of field.
         * @param   onwer       The cls of owner.
         * @param   access      The access.
         * @param   type        The type.
         * @param   name        The name.
         */
        Field(uint32_t offset, const Class *cls, const Class *onwer, 
            AccessType access, const char *type, const char *name);

    private:
        const Class *mClass;    /**< 字段的类型 */
        uint32_t    mOffset;    /**< 字段在对象中的偏移 */
    };


    /**
     * @class   StaticField
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API StaticField : public MemberBase
    {
        T3D_DISABLE_COPY(StaticField);

        friend class Class;
        friend struct __register_static_field__;

    public:
        /**
         * @fn  const Class StaticField::*getClass() const
         * @brief   Gets the class
         * @return  Null if it fails, else the class.
         */
        const Class *getClass() const { return mClass; }

        /**
         * @fn  template <typename Value> 
         *      void StaticField::get(Value &value) const
         * @brief   Gets a void using the given value
         * @tparam  Value   Type of the value.
         * @param [in,out]  value   The value.
         */
        template <typename Value>
        void get(Value &value) const
        {
            // 对象类型跟 Field 所属的类类型不一致
            if (getAccess() != AccessType::PUBLIC)
                return;
            // 值类型跟 Field 类型不一致
            if (Value::getStaticClass() != mClass)
                return;
            value = *(const Value *)(mAddress);
        }

        /**
         * @fn  template <typename Value> 
         *      void StaticField::set(const Value &value) const
         * @brief   Sets the given value
         * @tparam  Value   Type of the value.
         * @param   value   The value.
         */
        template <typename Value>
        void set(const Value &value) const
        {
            // 对象类型跟 Field 所属的类类型不一致
            if (getAccess() != AccessType::PUBLIC)
                return;
            // 值类型跟 Field 类型不一致
            if (Value::getStaticClass() != mClass)
                return;
            *(Value *)(mAddress) = value;
        }

    private:
        /**
         * @fn  StaticField::StaticField(void *address, const Class *cls, 
         *      const Class *owner, AccessType access, const char *type, 
         *      const char *name);
         * @brief   Constructor
         * @param [in,out]  address If non-null, the address.
         * @param           cls     The cls.
         * @param           owner   The owner.
         * @param           access  The access.
         * @param           type    The type.
         * @param           name    The name.
         */
        StaticField(void *address, const Class *cls, const Class *owner,
            AccessType access, const char *type, const char *name);

    private:
        const Class *mClass;    /**< 字段类型 */
        void        *mAddress;  /**< 字段地址 */
    };
}


#endif  /*__T3D_FIELD_H__*/
