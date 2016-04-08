

#ifndef __T3D_SUB_MESH_DATA_H__
#define __T3D_SUB_MESH_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class SubMeshData : public Object
    {
    public:
        typedef std::vector<int32_t>    Indices;
        typedef Indices::iterator       IndicesItr;
        typedef Indices::const_iterator IndicesConstItr;

        static SubMeshDataPtr create(const String &materialName, const Indices &indices, bool is16Bits);

        virtual ~SubMeshData();

    protected:
        SubMeshData();

        bool init(const String &materialName, const Indices &indices, bool is16Bits);

    private:
        SubMeshData(const SubMeshData &);
        SubMeshData &operator =(const SubMeshData &);

    protected:
        String                  mMaterialName;
        HardwareIndexBufferPtr  mIndexBuffer;
    };
}


#endif  /*__T3D_SUB_MESH_DATA_H__*/
