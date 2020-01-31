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


#include "Reflection/T3DClass.h"
#include "Reflection/T3DField.h"
#include "Reflection/T3DProperty.h"
#include "Reflection/T3DMethod.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Class::ClassMap Class::msClasses;

    //--------------------------------------------------------------------------

    const Class* Class::getClass(const std::string &name)
    {
        auto itr = msClasses.find(name);
        if (itr == msClasses.end())
            return nullptr;
        return itr->second;
    }

    //--------------------------------------------------------------------------

    Class::Class(const char *name, size_t s, size_t baseCount, 
        const Class *base0 /* = nullptr */, const Class *base1 /* = nullptr */, 
        const Class *base2 /* = nullptr */, const Class *base3 /* = nullptr */, 
        const Class *base4 /* = nullptr */, const Class *base5 /* = nullptr */, 
        const Class *base6 /* = nullptr */, const Class *base7 /* = nullptr */)
        : mName(name)
        , mSize(s)
        , mBaseClassCount(baseCount)
    {
        mBaseClasses[0] = base0;
        mBaseClasses[1] = base1;
        mBaseClasses[2] = base2;
        mBaseClasses[3] = base3;
        mBaseClasses[4] = base4;
        mBaseClasses[5] = base5;
        mBaseClasses[6] = base6;
        mBaseClasses[7] = base7;

        msClasses[name] = this;
    }

    //--------------------------------------------------------------------------

    bool Class::isBaseOf(const Class *cls) const
    {
        if (cls == nullptr || !cls->hasBase())
            return false;

        for (size_t i = 0; i < cls->getBaseClassCount(); ++i)
        {
            for (const Class *c = cls->getBaseClass(i); 
                c != nullptr; 
                c = c->getBaseClass(i))
            {
                if (c == this)
                    return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool Class::isSuperOf(const Class *cls) const 
    {
        if (cls == nullptr || !cls->hasBase())
            return false;

        for (size_t i = 0; i < cls->getBaseClassCount(); ++i)
        {
            if (this == cls->getBaseClass(i))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool Class::isKindOf(const Class *cls) const
    {
        bool ret = false;

        if (this == cls)
        {
            ret = true;
        }
        else
        {
            ret = cls->isBaseOf(this);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    const Field *Class::getField(const char *name, 
        bool searchBase /* = true */) const
    {
        const Field *field = nullptr;
        auto itr = mFields.find(name);

        if (itr == mFields.end())
        {
            if (searchBase && hasBase())
            {
                for (size_t i = 0; i < getBaseClassCount(); ++i)
                {
                    field = getBaseClass(i)->getField(name, true);
                    if (field != nullptr)
                        break;
                }
            }
        }
        else
        {
            field = itr->second;
        }

        return field;
    }

    //--------------------------------------------------------------------------

    const StaticField *Class::getStaticField(const char *name, 
        bool searchBase /* = true */) const
    {
        const StaticField *field = nullptr;
        auto itr = mStaticFields.find(name);

        if (itr == mStaticFields.end())
        {
            if (searchBase && hasBase())
            {
                for (size_t i = 0; i < getBaseClassCount(); ++i)
                {
                    field = getBaseClass(i)->getStaticField(name, true);
                    if (field != nullptr)
                        break;
                }
            }
        }
        else
        {
            field = itr->second;
        }

        return field;
    }

    //--------------------------------------------------------------------------

    const Method *Class::getMethod(const char *name,
        bool searchBase /* = true */) const
    {
        const Method *method = nullptr;
        auto itr = mMethods.find(name);

        if (itr == mMethods.end())
        {
            if (searchBase && hasBase())
            {
                for (size_t i = 0; i < getBaseClassCount(); ++i)
                {
                    method = getBaseClass(i)->getMethod(name, true);
                    if (method != nullptr)
                        break;
                }
            }
        }
        else
        {
            method = itr->second;
        }

        return method;
    }

    //--------------------------------------------------------------------------

    const StaticMethod *Class::getStaticMethod(const char *name, 
        bool searchBase /* = true */) const
    {
        const StaticMethod *method = nullptr;
        auto itr = mStaticMethods.find(name);

        if (itr != mStaticMethods.end())
        {
            if (searchBase && hasBase())
            {
                for (size_t i = 0; i < getBaseClassCount(); ++i)
                {
                    method = getBaseClass(i)->getStaticMethod(name, true);
                    if (method != nullptr)
                        break;
                }
            }
        }
        else
        {
            method = itr->second;
        }

        return method;
    }

    //--------------------------------------------------------------------------

    const ConstructorMethod *Class::getDefaultConstructor() const
    {
        return defaultConstructor();
    }

    //--------------------------------------------------------------------------

    const ConstructorMethod *Class::defaultConstructor() const
    {
        const ConstructorMethod *method = nullptr;

        auto itr = mConstructorMethods.begin();

        while (itr != mConstructorMethods.end())
        {
            method = itr->second;

            if (method->mCallable != nullptr 
                && method->mCallable->get_args_count() == 0) 
            {
                break;
            }

            ++itr;
        }

        return method;
    }

    //--------------------------------------------------------------------------

    const Property *Class::getProperty(const char *name, 
        bool searchBase /* = true */) const
    {
        const Property *prop = nullptr;
        auto itr = mProperties.find(name);

        if (itr != mProperties.end())
        {
            if (searchBase && hasBase())
            {
                for (size_t i = 0; i < getBaseClassCount(); ++i)
                {
                    prop = getBaseClass(i)->getProperty(name, true);
                    if (prop != nullptr)
                        break;
                }
            }
        }
        else
        {
            prop = itr->second;
        }

        return prop;
    }

    //--------------------------------------------------------------------------

    void *Class::newInstance() const
    {
        const ConstructorMethod *c = defaultConstructor();
        typedef const __static_callable0__<void*> CallableType;
        CallableType *cb = (CallableType *)(c->mCallable);
        return cb->invoke();
    }

    //--------------------------------------------------------------------------

    void Class::addField(Field *field)
    {
        mFields.insert(FieldMapValue(field->getName(), field));
    }

    //--------------------------------------------------------------------------

    void Class::addStaticField(StaticField *field)
    {
        mStaticFields.insert(StaticFieldMapValue(field->getName(), field));
    }

    //--------------------------------------------------------------------------

    void Class::addMethod(Method *method, __callable__ *cb)
    {
        method->set_callable(cb);
        mMethods.insert(MethodMapValue(method->getName(), method));
    }

    //--------------------------------------------------------------------------

    void Class::addStaticMethod(StaticMethod *method, __callable__ *cb)
    {
        method->set_callable(cb);
        mStaticMethods.insert(StaticMethodMapValue(method->getName(), method));
    }

    //--------------------------------------------------------------------------

    void Class::addConstructor(ConstructorMethod *method, __callable__ *cb)
    {
        method->set_callable(cb);
        mConstructorMethods.insert(ConstructorMapValue(method->getName(), 
            method));
    }

    //--------------------------------------------------------------------------

    void Class::addProperty(Property *prop, __property_base__ *handler)
    {
        prop->mHandler = handler;
        mProperties.insert(PropertyMapValue(prop->getName(), prop));
    }

    //--------------------------------------------------------------------------

    __register_field__::__register_field__(
        unsigned long offset, const Class *cls, const Class *owner, 
        AccessType access, const char *type, const char *name) 
    {
        Field* field = new Field(offset, cls, owner, access, type, name);
        (const_cast<Class *>(owner))->addField(field);
    }

    //--------------------------------------------------------------------------

    __register_static_field__::__register_static_field__(
        void *address, const Class *cls, const Class *owner, AccessType access, 
        const char *type, const char *name) 
    {
        StaticField*  field = new StaticField(address, cls, owner, access, type, 
            name);
        (const_cast<Class *>(owner))->addStaticField(field);
    }

    //--------------------------------------------------------------------------

    __register_method__::__register_method__(
        __callable__ *cb, const Class *owner, AccessType access, 
        const char *type, const char *name, const char *args, const char *virt) 
    {
        Method* method = new Method(owner, access, type, name, args, 
            virt[0] == 'v');
        (const_cast<Class *>(owner))->addMethod(method, cb);
    }

    //--------------------------------------------------------------------------

    __register_static_method__::__register_static_method__(
        __callable__ *cb, const Class *owner, AccessType access, 
        const char *type, const char *name, const char *args) 
    {
        StaticMethod* method = new StaticMethod(owner, access, type, name, args);
        (const_cast<Class *>(owner))->addStaticMethod(method, cb);
    }

    //--------------------------------------------------------------------------

    __register_constructor_method__::__register_constructor_method__(
        __callable__ *cb, __callable__ *placement_cb, const Class *owner, 
        AccessType access, const char *type, const char *name, const char *args) 
    {
        ConstructorMethod* method = new ConstructorMethod(owner, access, type, 
            name, args, placement_cb);
        (const_cast<Class *>(owner))->addConstructor(method, cb);
    }

    //--------------------------------------------------------------------------

    __register_property__::__register_property__(
        __property_base__ *handler, const Class *owner, const char *type, 
        const char *name, const Class *cls) 
    {
        Property* prop = new Property(owner, type, name, cls);
        (const_cast<Class *>(owner))->addProperty(prop, handler);
    }
}
