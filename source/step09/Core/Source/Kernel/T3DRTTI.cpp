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


#include "Kernel/T3DRTTI.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name)
        : mBaseClassesCount(0)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass)
        : mBaseClassesCount(1)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1)
        : mBaseClassesCount(2)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
        const RTTI *baseClass2)
        : mBaseClassesCount(3)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
        mBaseClasses[2] = baseClass2;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
        const RTTI *baseClass2, const RTTI *baseClass3)
        : mBaseClassesCount(4)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
        mBaseClasses[2] = baseClass2;
        mBaseClasses[3] = baseClass3;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
        const RTTI *baseClass2, const RTTI *baseClass3, const RTTI *baseClass4)
        : mBaseClassesCount(5)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
        mBaseClasses[2] = baseClass2;
        mBaseClasses[3] = baseClass3;
        mBaseClasses[4] = baseClass4;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
        const RTTI *baseClass2, const RTTI *baseClass3, const RTTI *baseClass4,
        const RTTI *baseClass5)
        : mBaseClassesCount(6)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
        mBaseClasses[2] = baseClass2;
        mBaseClasses[3] = baseClass3;
        mBaseClasses[4] = baseClass4;
        mBaseClasses[5] = baseClass5;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
        const RTTI *baseClass2, const RTTI *baseClass3, const RTTI *baseClass4,
        const RTTI *baseClass5, const RTTI *baseClass6)
        : mBaseClassesCount(7)
    {
        strcpy(mName, name);
        memset(mBaseClasses, 0, sizeof(mBaseClasses));
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
        mBaseClasses[2] = baseClass2;
        mBaseClasses[3] = baseClass3;
        mBaseClasses[4] = baseClass4;
        mBaseClasses[5] = baseClass5;
        mBaseClasses[6] = baseClass6;
    }

    //--------------------------------------------------------------------------

    RTTI::RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
        const RTTI *baseClass2, const RTTI *baseClass3, const RTTI *baseClass4,
        const RTTI *baseClass5, const RTTI *baseClass6, const RTTI *baseClass7)
        : mBaseClassesCount(8)
    {
        strcpy(mName, name);
        mBaseClasses[0] = baseClass0;
        mBaseClasses[1] = baseClass1;
        mBaseClasses[2] = baseClass2;
        mBaseClasses[3] = baseClass3;
        mBaseClasses[4] = baseClass4;
        mBaseClasses[5] = baseClass5;
        mBaseClasses[6] = baseClass6;
        mBaseClasses[7] = baseClass7;
    }

    //--------------------------------------------------------------------------

    bool RTTI::isKindOf(const RTTI *rtti) const
    {
        bool ret = false;

        if (this == rtti)
        {
            ret = true;
        }
        else
        {
            for (size_t i = 0; i < mBaseClassesCount; ++i)
            {
                if (isKindOf(rtti->getBaseClassRTTI(i)))
                {
                    ret = true;
                }
            }
        }
        
        return ret;
    }
}
