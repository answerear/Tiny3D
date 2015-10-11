

#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Resource
    {
    public:
        enum EType
        {
            E_TYPE_UNKNOWN = 0,
            E_TYPE_DYLIB,
            E_TYPE_MODEL,
            E_TYPE_MATERIAL,
            E_TYPE_TEXTURE,
        };

        Resource(const TString &strName);
        virtual ~Resource();

        virtual EType getType() const = 0;

        virtual bool load() = 0;
        virtual void unload() = 0;
        virtual Resource *clone() = 0;

        ulong_t getID() const
        {
            return m_ulID;
        }

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
        ulong_t     m_ulID;
        bool        m_bIsLoaded;
        TString     m_strName;
    };

    typedef std::shared_ptr<Resource>   ResourcePtr;
}


#endif  /*__T3D_RESOURCE_H__*/
