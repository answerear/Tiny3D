
#ifndef __T3D_SG_BOX_H__
#define __T3D_SG_BOX_H__


#include "SG/Renderable/T3DSGGeometry.h"
#include "Math/T3DAabb.h"
#include "Math/T3DVector2.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGBox : public SGGeometry
    {
    public:
        static SGBoxPtr create(const String &materialName, uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGBox();

        bool loadBox();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        void setMinX(Real x);
        void setMaxX(Real x);
        void setMinY(Real y);
        void setMaxY(Real y);
        void setMinZ(Real z);
        void setMaxZ(Real z);

        Real getMinX() const;
        Real getMaxX() const;
        Real getMinY() const;
        Real getMaxY() const;
        Real getMinZ() const;
        Real getMaxZ() const;

    protected:
        SGBox(uint32_t unID = E_NID_AUTOMATIC);
        virtual bool init(const String &materialName);

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        void updateVertices();

    protected:
        enum
        {
            MAX_FACE_NUM = 6,
            MAX_VERTICES_NUM = 23,//24,
            INDICES_PER_FACE = 6
        };

        struct Vertex
        {
            Vector3 position;
            Vector2 texcoord;
        };

        Vertex mVertices[MAX_VERTICES_NUM];

        MaterialPtr     mMaterial;
        VertexDataPtr   mVertexData;
        IndexDataPtr    mIndexData;
    };
}


#endif  /*__T3D_SG_BOX_H__*/
