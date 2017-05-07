

#ifndef __T3D_SG_SKELETON_H__
#define __T3D_SG_SKELETON_H__


#include "SG/Renderable/T3DSGGeometry.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGSkeleton : public SGGeometry
    {
    public:
        static SGSkeletonPtr create(ModelPtr model, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGSkeleton();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

    protected:
        struct BoneVertex
        {
            BoneVertex() : position(), color(Color4::WHITE) {}
            Vector3     position;
            Color4      color;
        };

        SGSkeleton(uint32_t uID = E_NID_AUTOMATIC);

        virtual bool init(ModelPtr model);
        bool buildSkeletonVertices(const ObjectPtr &skeleton, std::vector<BoneVertex> &vertices);

        void updateVertices();

        virtual void updateTransform() override;

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        bool searchSkinData(const String &name, ObjectPtr &skin);

    protected:
        ModelPtr        mModel;
        VertexDataPtr   mVertexData;
    };
}


#endif  /*__T3D_SG_SKELETON_H__*/
