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


#ifndef __T3D_METHOD_H__
#define __T3D_METHOD_H__


#include "Reflection/T3DMember.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API __callable__
    {
    public:
        typedef std::vector<const Class *> arg_list_type;
        virtual ~__callable__() {}

        virtual int get_args_count() const = 0;
        virtual const arg_list_type &get_args() const = 0;
        virtual const Class *get_ret_type() const = 0;
    };

    class T3D_PLATFORM_API MethodBase : public MemberBase
    {
        T3D_DISABLE_COPY(MethodBase);

        friend class Class;

    public:
        virtual ~MethodBase();

        const char *getID() const;

        const char *getSignature() const;

        const char *getArgs() const;

        int getArgsCount() const;

        const Class *getRuturnClass() const;

    private:
        const char  *mID;
        const char  *mArgs;
        const char  *mLongID;
        bool        mOwnCB;
    };
}


#endif  /*__T3D_METHOD_H__*/
