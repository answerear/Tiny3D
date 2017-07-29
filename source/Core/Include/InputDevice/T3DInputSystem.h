/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_INPUT_SYSTEM_H__
#define __T3D_INPUT_SYSTEM_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API InputSystem : public Object
    {
    public:
        /**
         * @brief Îö¹¹º¯Êý
         */
        virtual ~InputSystem();

        virtual bool initialize() = 0;
        virtual void uninitialize() = 0;

        virtual bool hasTouchDevice() const = 0;
        virtual bool hasKeyDevice() const = 0;
        virtual bool hasJoystickDevice() const = 0;

        virtual TouchDevicePtr getTouchDevcie() = 0;
        virtual KeyDevicePtr getKeyDevice() = 0;
        virtual JoystickDevicePtr getJoystickDevice() = 0;
    };
}


#endif  /*__T3D_INPUT_SYSTEM_H__*/
