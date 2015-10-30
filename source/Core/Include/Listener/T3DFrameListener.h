

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
