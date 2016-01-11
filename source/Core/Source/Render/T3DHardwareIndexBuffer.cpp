

#ifndef __T3D_HARDWARE_BUFFER_H__
#define __T3D_HARDWARE_BUFFER_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareBuffer : public Object
    {
    public:
        virtual ~HardwareBuffer();

    protected:
        HardwareBuffer();
    };
}


#endif  /*__T3D_HARDWARE_BUFFER_H__*/
