

#ifndef __T3D_ARCHIVE_H__
#define __T3D_ARCHIVE_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Archive : public Resource
    {
    public:
        virtual ~Archive();

        virtual Type getType() const override;

        virtual String getArchiveType() const = 0;

        virtual String getLocation() const = 0;
        virtual bool exists(const String &name) const = 0;
        virtual bool read(const String &name, MemoryDataStream &stream) = 0;
        virtual bool write(const String &name, const MemoryDataStream &stream) = 0;

    protected:
        Archive(const String &name);
    };
}


#endif  /*__T3D_ARCHIVE_H__*/
