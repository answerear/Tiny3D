

#ifndef __T3D_SUB_MESH_DATA_H__
#define __T3D_SUB_MESH_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "DataStruct/T3DIndexData.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class SubMeshData : public Object
    {
    public:
        typedef std::vector<uint8_t>        Indices;
        typedef Indices::iterator           IndicesItr;
        typedef Indices::const_iterator     IndicesConstItr;

        static SubMeshDataPtr create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices &indices, bool is16bits);

        virtual ~SubMeshData();

        const String &getMaterialName() const
        {
            return mMaterialName;
        }

        Renderer::PrimitiveType getPrimitiveType() const
        {
            return mPrimitiveType;
        }

        const IndexDataPtr &getIndexData() const
        {
            return mIndexData;
        }

    protected:
        SubMeshData();

        bool init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices &indices, bool is16bits);

    private:
        SubMeshData(const SubMeshData &);
        SubMeshData &operator =(const SubMeshData &);

    protected:
        String                  mMaterialName;
        Renderer::PrimitiveType mPrimitiveType;
        IndexDataPtr            mIndexData;
    };
}


#endif  /*__T3D_SUB_MESH_DATA_H__*/
