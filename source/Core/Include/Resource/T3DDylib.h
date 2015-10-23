
#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        Dylib(const String &name);
        virtual ~Dylib();

        virtual EType getType() const;

        virtual void *getSymbol(const String &name) const;

    protected:
        virtual bool load();
        virtual void unload();
        virtual Resource *clone() const;

    protected:
        THandle   mHandle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
