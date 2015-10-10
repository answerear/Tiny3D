

#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Resource
    {
    public:
        Resource();
        virtual ~Resource();

        virtual bool load() = 0;
        virtual void unload() = 0;

        size_t getSize() const
        {
            return m_unSize;
        }

        const TString &getName() const
        {
            return m_strName;
        }

        bool isLoaded() const
        {
            return m_bIsLoaded;
        }

    protected:
        size_t      m_unSize;
        bool        m_bIsLoaded;
        TString     m_strName;
    };
}


#endif  /*__T3D_RESOURCE_H__*/
