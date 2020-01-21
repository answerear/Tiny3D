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


#include "Reflection/T3DMethod.h"
#include "Reflection/T3DReflectionMacro.h"
#include "Reflection/T3DClass.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MethodBase::MethodBase(const Class *owner, AccessType access, const char *type, const char *name, const char *args)
        : MemberBase(owner, access, type, name)
        , mCallable(nullptr)
        , mID(name)
        , mArgs(args)
        , mSignature(name)
        , mOwnCB(false)
    {
        mID += args;
    }

    //--------------------------------------------------------------------------

    int MethodBase::getArgsCount() const 
    {
        return mCallable->get_args_count();
    }

    //--------------------------------------------------------------------------

    const Class *MethodBase::getRuturnClass() const
    {
        return mCallable->get_ret_type();
    }

    //--------------------------------------------------------------------------

    void MethodBase::set_callable(__callable__ *cb)
    {
        typedef __callable__::arg_list_type arglist;

        if (mOwnCB)
        {
            T3D_SAFE_DELETE(mCallable);
        }

        mCallable = cb;

        if (cb != nullptr)
        {
            mID = getName();
            mSignature = getPrefix(cb->get_ret_type());
            mSignature += '(';

            if (cb->get_args_count() == 0)
            {
                mID += "__";
                mID += "void";
            }
            else
            {
                const arglist &list = cb->get_args();
                auto i = list.begin();
                bool notfirst = false;
                while (i != list.end())
                {
                    const Class *cls = *i++;
                    mID += "__";
                    mID += cls->getName();
                    if (notfirst)
                        mSignature += ", ";
                    mSignature += cls->getName();
                    notfirst = true;
                }
            }

            mSignature += ')';
        }

        mOwnCB = true;
    }

    //--------------------------------------------------------------------------

    std::string MethodBase::getPrefix(const Class *retType) const
    {
        return std::string(retType->getName()) + ' ' + getOwner().getName() + "::" + getName();
    }

    //--------------------------------------------------------------------------

    Method::Method(const Class *owner, AccessType access, const char *type,
        const char *name, const char *args, bool virt)
        : MethodBase(owner, access, type, name, args)
        , mIsVirtual(virt)
    {

    }

    //--------------------------------------------------------------------------

    Method::~Method()
    {

    }

    //--------------------------------------------------------------------------

    StaticMethod::StaticMethod(const Class *owner, AccessType access, const char *type, const char *name, const char *args)
        : MethodBase(owner, access, type, name, args)
    {

    }

    //--------------------------------------------------------------------------

    std::string StaticMethod::getPrefix(const Class *retType) const 
    {
        return std::string("static ") + MethodBase::getPrefix(retType);

    }

    //--------------------------------------------------------------------------

    ConstructorMethod::ConstructorMethod(const Class *owner, AccessType access,
        const char *type, const char *name, const char *args,
        __callable__ *callable)
        : StaticMethod(owner, access, type, name, args)
        , mPlacementCallable(callable)
    {

    }

    //--------------------------------------------------------------------------

    std::string ConstructorMethod::getPrefix(const Class *retType) const
    {
        return getOwner().getName() + std::string("::") + getName();
    }
}
