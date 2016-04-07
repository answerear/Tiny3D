

#ifndef __T3D_SUB_MESH_DATA_H__
#define __T3D_SUB_MESH_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class SubMeshData : public Object
    {
    public:
        static SubMeshDataPtr create();

        virtual ~SubMeshData();

    protected:
        SubMeshData();

    private:
        SubMeshData(const SubMeshData &);
        SubMeshData &operator =(const SubMeshData &);

    public:
        typedef std::list<int32_t>      Indices;
        typedef Indices::iterator       IndicesItr;
        typedef Indices::const_iterator IndicesConstItr;

        String      mMaterialName;
        Indices     mIndices;
    };
}


#endif  /*__T3D_SUB_MESH_DATA_H__*/
