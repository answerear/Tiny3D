

#ifndef __T3D_MODEL_H__
#define __T3D_MODEL_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Model : public Resource
    {
    public:
        typedef std::vector<ObjectPtr>          SubMeshDataList;
        typedef SubMeshDataList::iterator       SubMeshDataListItr;
        typedef SubMeshDataList::const_iterator SubMeshDataListConstItr;

        typedef std::vector<String>             MaterialList;
        typedef MaterialList::iterator          MaterialListItr;
        typedef MaterialList::const_iterator    MaterialListConstItr;

        static ModelPtr create(const String &name);

        virtual ~Model();

        virtual Type getType() const override;

    protected:
        Model(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

    protected:
        ObjectPtr       mMeshData;
        SubMeshDataList mSubMeshData;
    };
}

#endif  /*__T3D_MODEL_H__*/
