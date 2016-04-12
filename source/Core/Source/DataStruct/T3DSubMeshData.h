

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
        typedef std::vector<uint16_t>       Indices16;
        typedef Indices16::iterator         Indices16Itr;
        typedef Indices16::const_iterator   Indices16ConstItr;

        typedef std::vector<uint32_t>       Indices32;
        typedef Indices32::iterator         Indices32Itr;
        typedef Indices32::const_iterator   Indices32ConstItr;

        static SubMeshDataPtr create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices16 &indices);
        static SubMeshDataPtr create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices32 &indices);

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

        bool init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices16 &indices);
        bool init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices32 &indices);

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
