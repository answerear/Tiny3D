

#include "T3DPlugin.h"


namespace Tiny3D
{
    Plugin::Plugin()
    {

    }

    Plugin::~Plugin()
    {

    }

    bool Plugin::load()
    {
        return true;
    }

    void Plugin::unload()
    {

    }

    void *Plugin::getSymbol(const TString &strName) const
    {
        return PLUGIN_GETSYM(m_Handle, strName.c_str());
    }
}
