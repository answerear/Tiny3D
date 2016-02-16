

#ifndef __T3D_D3D9_PLUGIN_H__
#define __T3D_D3D9_PLUGIN_H__


#include "T3DD3D9Prerequisites.h"
#include "Misc/T3DPlugin.h"


namespace Tiny3D
{
    class D3D9Plugin : public Plugin
    {
    public:
        D3D9Plugin();
        virtual ~D3D9Plugin();

        virtual const String &getName() const;

        virtual bool install();
        virtual bool startup();
        virtual void shutdown();
        virtual void uninstall();

    protected:
        String      mName;

        Renderer        *mRenderer;
        TextureManager  *mTextureMgr;
    };
}


#endif  /*__T3D_D3D9_PLUGIN_H__*/
