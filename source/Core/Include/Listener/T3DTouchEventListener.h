

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
        virtual void onTouchOver(int32_t x, int32_t y) = 0;
        virtual void onTouchMove(int32_t x, int32_t y) = 0;
        virtual void onTouchUp(int32_t x, int32_t y) = 0;
    };
}


#endif  /*__T3D_TOUCH_EVENT_LISTENER_H__*/
