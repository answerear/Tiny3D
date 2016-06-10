

#ifndef __T3D_INPUT_SYSTEM_H__
#define __T3D_INPUT_SYSTEM_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class TouchDevice;
    class KeyboardDevice;
    class JoystickDevice;

    class T3D_ENGINE_API InputSystem : public Object
    {
    public:
        InputSystem();
        virtual ~InputSystem();

        virtual bool initialize() = 0;
        virtual void uninitialize() = 0;

        virtual String getName() const = 0;


    };
}


#endif  /*__T3D_INPUT_SYSTEM_H__*/
