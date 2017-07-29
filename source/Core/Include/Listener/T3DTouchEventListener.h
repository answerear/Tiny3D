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

#ifndef __T3D_TOUCH_EVENT_LISTENER_H__
#define __T3D_TOUCH_EVENT_LISTENER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TouchEventListener
    {
    public:
        virtual ~TouchEventListener();

        virtual void onTouchDown(int32_t x, int32_t y) = 0;
        virtual void onTouchMove(int32_t x, int32_t y) = 0;
        virtual void onTouchUp(int32_t x, int32_t y) = 0;
        virtual void onTouchMoveOn(int32_t x, int32_t y) = 0;
        virtual void onTouchMoveOver(int32_t x, int32_t y) = 0;
    };
}


#endif  /*__T3D_TOUCH_EVENT_LISTENER_H__*/
