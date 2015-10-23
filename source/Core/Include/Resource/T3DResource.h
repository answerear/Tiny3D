

#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Resource : public Object
    {
        friend class ResourceManager;

    public:
        enum EType
        {
            E_TYPE_UNKNOWN = 0,
            E_TYPE_DYLIB,
            E_TYPE_MESH,
            E_TYPE_MATERIAL,
            E_TYPE_TEXTURE,
        };

        Resource(const String &strName);
        virtual ~Resource();

        virtual EType getType() const = 0;

        uint32_t getID() const
        {
            return mID;
        }

        uint32_t getCloneID() const
        {
            return mCloneID;
        }

        bool isCloned() const
        {
            return (mCloneID != 0);
        }

        size_t getSize() const
        {
            return mSize;
        }

        const String &getName() const
        {
            return mName;
        }

        bool isLoaded() const
        {
            return mIsLoaded;
        }

    protected:
        virtual bool load() = 0;
        virtual void unload();
        virtual Resource *clone() const = 0;

    protected:
        size_t      mSize;      /** size of this resource */
        int32_t     mID;        /** ID of this resource */
        int32_t     mCloneID;   /** If this resource is cloned from another, this value is none zero */
        bool        mIsLoaded;  /** loaded flag */
        String     mName;      /** name of the resource */
    };
}


#endif  /*__T3D_RESOURCE_H__*/
