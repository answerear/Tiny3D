
#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        static DylibPtr create(const String &name);

        virtual ~Dylib();

        virtual Type getType() const override;

        virtual void *getSymbol(const String &name) const;

    protected:
        Dylib(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

    protected:
        THandle   mHandle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
