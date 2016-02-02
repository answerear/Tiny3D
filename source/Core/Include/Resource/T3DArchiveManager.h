

#ifndef __T3D_ARCHIVE_MANAGER_H__
#define __T3D_ARCHIVE_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class ArchiveCreator;

    class T3D_ENGINE_API ArchiveManager
        : public Singleton<ArchiveManager>
        , public ResourceManager
    {
    public:
        ArchiveManager();
        virtual ~ArchiveManager();

        virtual ArchivePtr loadArchive(const String &name, const String &archiveType);

        void addArchiveCreator(const ArchiveCreator &creator);
        void removeArchiveCreator(const String &name);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

        typedef std::map<String, ArchiveCreator*>   Creators;
        typedef Creators::iterator                  CreatorsItr;
        typedef Creators::const_iterator            CreatorsConstItr;
        typedef Creators::value_type                CreatorsValue;

        Creators    mCreators;

    };
}


#endif  /*__T3D_ARCHIVE_MANAGER_H__*/
