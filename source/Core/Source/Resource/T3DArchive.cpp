

#include "Resource/T3DArchive.h"


namespace Tiny3D
{
    Archive::Archive(const String &name)
        : Resource(name)
    {

    }

    Archive::~Archive()
    {

    }

    Resource::Type Archive::getType() const
    {
        return E_TYPE_ARCHIVE;
    }
}
