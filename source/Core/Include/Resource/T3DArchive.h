

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

        virtual bool open(const String &name) = 0;
        virtual size_t read(MemoryDataStream &stream, const String &name = "") = 0;
        virtual size_t write(const MemoryDataStream &stream, const String &name = "") = 0;
        virtual void close() = 0;

    protected:
        Archive(const String &name);
    };
}


#endif  /*__T3D_ARCHIVE_H__*/
