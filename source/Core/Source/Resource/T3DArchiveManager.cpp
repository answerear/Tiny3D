

#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveCreator.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(ArchiveManager);

    ArchiveManager::ArchiveManager()
    {

    }

    ArchiveManager::~ArchiveManager()
    {

    }

    ArchivePtr ArchiveManager::loadArchive(const String &name, const String &archiveType)
    {
        return smart_pointer_cast<Archive>(ResourceManager::load(name, 1, archiveType.c_str()));
    }

    ResourcePtr ArchiveManager::create(const String &name, int32_t argc, va_list args)
    {
        ArchivePtr res;
        if (argc == 1)
        {
            String archiveType = va_arg(args, char *);

            CreatorsConstItr itr = mCreators.find(archiveType);

            if (itr != mCreators.end())
            {
                ArchiveCreator *creator = itr->second;
                res = creator->createObject();
            }
        }

        return res;
    }
}

