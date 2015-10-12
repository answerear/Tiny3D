
#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        Dylib(const TString &strName);
        virtual ~Dylib();

        virtual EType getType() const;

        virtual void *getSymbol(const TString &strName) const;

    protected:
        virtual bool load();
        virtual void unload();
        virtual Resource *clone() const;

    protected:
        THandle   m_Handle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
