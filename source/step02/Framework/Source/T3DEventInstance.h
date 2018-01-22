/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __T3D_EVENT_INSTANCE_H__
#define __T3D_EVENT_INSTANCE_H__


#include "T3DEventPrerequisites.h"

namespace Tiny3D
{
//     class T3D_FRAMEWORK_API EventInstance
//     {
//     public:
//         static const EventInstance INVALID_INSTANCE;
//         static const EventInstance BROADCAST_INSTANCE;
// 
//         long_t  ref;
//         int32_t idx;
// 
//         EventInstance()
//             : ref(0)
//             , idx(0)
//         {}
// 
//         EventInstance(long_t r, int32_t i)
//             : ref(r)
//             , idx(i)
//         {}
// 
//         bool operator ==(const EventInstance &other)
//         {
//             return (idx == other.idx && ref == other.ref);
//         }
// 
//         bool operator !=(const EventInstance &other)
//         {
//             return (idx != other.idx || ref != other.ref);
//         }
// 
//         bool operator <(const EventInstance &other)
//         {
//             return (idx < other.idx || (idx == other.idx && ref < other.ref));
//         }
//     };
    struct _TINSTANCE
    {
        _TINSTANCE()
            : obj(0)
            , idx(0)
        {}

        _TINSTANCE(long_t o, int32_t i)
            : obj(o)
            , idx(i)
        {}

        long_t  obj;
        int32_t idx;
    };
}


#endif  /*__T3D_EVENT_INSTANCE_H__*/
