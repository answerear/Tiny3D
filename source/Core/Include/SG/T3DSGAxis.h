

#ifndef __T3D_SG_AXIS_H__
#define __T3D_SG_AXIS_H__


#include "SG/T3DSGGeometry.h"


namespace Tiny3D
{
    enum Axis
    {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };

    class T3D_ENGINE_API SGAxis : public SGRenderable
    {
    public:
        static SGAxisPtr create(Real fAxisX, Real fAxisY, Real fAxisZ, uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGAxis();
        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        Real getAxisLength(Axis axis) const;
        void setAxisLength(Axis axis, Real fLength);

    protected:
        SGAxis(uint32_t uID = E_NID_AUTOMATIC);

        virtual bool init(Real fAxisX, Real fAxisY, Real fAxisZ);
        
        virtual void cloneProperties(const NodePtr &node) const override;

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

        bool updateVertices();

    private:
        struct Vertex
        {
            Vertex() 
                : position(Vector3::ZERO)
                , color(Color4::WHITE)
            {
            }

            Vector3 position;
            Color4  color;
        };

        Vertex  mVertices[6];
        bool    mIsDataDirty;

        VertexDataPtr   mVertexData;
    };
}

#endif  /*__T3D_SG_AXIS_H__*/

