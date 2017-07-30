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

#ifndef __T3D_FRAME_LISTENER_H__
#define __T3D_FRAME_LISTENER_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    struct FrameEvent
    {
        /** Elapsed time in milliseconds since the last event.
            This gives you time between frame start & frame end,
            and between frame end and next frame start.
          @remarks
            This may not be the elapsed time but the average
            elapsed time between recently fired events.
         */
        uint32_t    timeSinceLastEvent;

        /** Elapsed time in milliseconds since the last event of the same type,
            i.e. time for a complete frame.
          @remarks
            This may not be the elapsed time but the average
            elapsed time between recently fired events of the same type.
        */
        uint32_t    timeSinceLastFrame;
    };

    class T3D_ENGINE_API FrameListener
    {
    public:
        FrameListener();
        virtual ~FrameListener();

        virtual bool onFrameStarted(const FrameEvent &evt);
        virtual bool onFrameEnded(const FrameEvent &evt);
    };
}


#endif  /*__T3D_FRAME_LISTENER_H__*/
