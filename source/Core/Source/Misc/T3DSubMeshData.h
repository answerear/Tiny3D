

#ifndef __T3D_SUBMESH_DATA_H__
#define __T3D_SUBMESH_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class SubMeshData : public Object
    {
    public:
        typedef std::vector<uint8_t>            Indices;
        typedef Indices::iterator               IndicesItr;
        typedef Indices::const_iterator         IndicesConstItr;

        static SubMeshDataPtr create(const String &name, const String &materialName, Renderer::PrimitiveType priType, bool is16Bits, size_t indexCount);

        String                  mName;
        String                  mMaterialName;
        Renderer::PrimitiveType mPrimitiveType;
        bool                    mIs16Bits;
        Indices                 mIndices;

    protected:
        SubMeshData(const String &name, const String &materialName, Renderer::PrimitiveType priType, bool is16Bits, size_t indexCount);

    private:
        SubMeshData(const SubMeshData &rkOther);
        SubMeshData &operator =(const SubMeshData &rkOther);
    };
}



#endif  /*__T3D_SUBMESH_DATA_H__*/
