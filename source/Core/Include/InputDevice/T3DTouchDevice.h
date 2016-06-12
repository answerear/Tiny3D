

#ifndef __T3D_TOUCH_DEVICE_H__
#define __T3D_TOUCH_DEVICE_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TouchDevice : public Object
    {
    public:
        virtual ~TouchDevice();

        virtual bool addListener(TouchEventListener *listener) = 0;
        virtual bool removeListener(TouchEventListener *listener) = 0;

        virtual void poll() = 0;
    };
}



#endif  /*__T3D_TOUCH_DEVICE_H__*/
