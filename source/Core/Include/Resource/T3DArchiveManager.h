

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

        bool getArchive(const String &name, ArchivePtr &archive);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

        typedef std::map<String, ArchiveCreator*>   Creators;
        typedef Creators::iterator                  CreatorsItr;
        typedef Creators::const_iterator            CreatorsConstItr;
        typedef Creators::value_type                CreatorsValue;

        typedef std::map<String, ArchivePtr>        Archives;
        typedef Archives::iterator                  ArchivesItr;
        typedef Archives::const_iterator            ArchivesConstItr;
        typedef Archives::value_type                ArchivesValue;

        Creators    mCreators;
        Archives    mArchives;
    };

    #define T3D_ARCHIVE_MGR     ArchiveManager::getInstance()
}


#endif  /*__T3D_ARCHIVE_MANAGER_H__*/
