

#ifndef __T3D_CONFIG_FILE_H__
#define __T3D_CONFIG_FILE_H__


#include "T3DPrerequisites.h"
#include <T3DPlatform.h>


namespace Tiny3D
{
    class T3D_ENGINE_API ConfigFile
    {
    public:
        ConfigFile();
        virtual ~ConfigFile();

        bool load(const String &filename);
        bool save(const String &filename);

    private:
        FileDataStream  mFileStream;
    };
}


#endif  /*__T3D_CONFIG_FILE_H__*/
