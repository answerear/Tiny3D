

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
