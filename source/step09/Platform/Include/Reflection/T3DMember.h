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


#ifndef __T3D_MEMBER_H__
#define __T3D_MEMBER_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "Reflection/T3DClass.h"


namespace Tiny3D
{
    class Class;

    /**
     * @class   MemberBase
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API MemberBase
    {
        T3D_DISABLE_COPY(MemberBase);

    public:
        /**
         * @fn  const Class MemberBase::&getClass() const
         * @brief   Gets the class
         * @return  The class.
         */
        const Class &getOwner() const { return *mOwner; }

        /**
         * @fn  const AccessType MemberBase::getAccess() const
         * @brief   Gets the access
         * @return  The access.
         */
        AccessType getAccess() const { return mAccess; }

        /**
         * @fn  const char MemberBase::*getType() const
         * @brief   Gets the type
         * @return  Null if it fails, else the type.
         */
        const char *getType() const { return mType.c_str(); }

        /**
         * @fn  const char MemberBase::*getName() const
         * @brief   Gets the name
         * @return  Null if it fails, else the name.
         */
        const char *getName() const { return mName.c_str(); }

    protected:
        /**
         * @fn  MemberBase::MemberBase(const Class *owner, AccessType access, 
         *      const char *type, const char *name);
         * @brief   Constructor
         * @param   owner   The owner.
         * @param   access  The access.
         * @param   type    The type.
         * @param   name    The name.
         */
        MemberBase(const Class *owner, AccessType access, const char *type, 
            const char *name);

    private:
        const Class *mOwner;    /**< 成员所属的类 */
        AccessType  mAccess;    /**< 访问权限 */
        std::string mType;      /**< 成员类型 */
        std::string mName;      /**< 成员名称 */
    };
}


#endif  /*__T3D_MEMBER_H__*/
