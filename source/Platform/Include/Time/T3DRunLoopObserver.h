
#ifndef __T3D_RUN_LOOP_OBSERVER_H__
#define __T3D_RUN_LOOP_OBSERVER_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformMacro.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API RunLoopObserver
    {
        T3D_DECLARE_INTERFACE(RunLoopObserver);

    public:
        virtual void onExecute(uint32_t unLoopID, uint64_t dt) = 0;
    };
}


#endif  /*__T3D_RUN_LOOP_OBSERVER_H__*/
