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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Class::ClassMap Class::msClasses;

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
}
