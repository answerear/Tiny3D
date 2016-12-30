

#ifndef __SG_TRIANGLE_H__
#define __SG_TRIANGLE_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    class SGTriangle;

    T3D_DECLARE_SMART_PTR(SGTriangle);

    class SGTriangle : public SGGeometry
    {
    public:
        static SGTrianglePtr create();

        virtual ~SGTriangle();

    protected:
        SGTriangle(uint32_t unID = E_NID_AUTOMATIC);

        VertexDataPtr   mVertexData;
    };
}


#endif  /*__SG_TRIANGLE_H__*/
