

#ifndef __T3D_PLUGIN_H__
#define __T3D_PLUGIN_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Plugin
    {
    public:
        T3D_DECLARE_INTERFACE(Plugin);

        virtual const TString &getName() const = 0;
        virtual bool install() = 0;
        virtual bool startup() = 0;
        virtual void shutdown();
        virtual void uninstall();
    };
}


#endif  /*__T3D_PLUGIN_H__*/
