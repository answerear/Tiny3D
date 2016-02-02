

#ifndef __T3D_ARCHIVE_CREATOR_H__
#define __T3D_ARCHIVE_CREATOR_H__


#include "Misc/T3DCreator.h"
#include "Resource/T3DArchive.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ArchiveCreator : public Creator<Archive>
    {
        T3D_DECLARE_INTERFACE(ArchiveCreator);
    };
}


#endif  /*__T3D_ARCHIVE_CREATOR_H__*/
