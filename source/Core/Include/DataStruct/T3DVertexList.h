

#ifndef __T3D_VERTEX_LIST_H__
#define __T3D_VERTEX_LIST_H__


#include "Misc/T3DObject.h"
#include "Math/T3DVector2.h"
#include "Math/T3DMatrix3.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    struct VertexP
    {
        Vector3 mPosition;
    };

    struct VertexN
    {
        Vector3 mPosition;
        Vector3 mNormal;
    };

    struct VertexT
    {
        Vector3 mPosition;
        Vector2 mTexCoord;
    };

    struct VertexC
    {
        Vector3 mPosition;
        Color4  mColor;
    };

    struct VertexNT
    {
        Vector3 mPosition;
        Vector3 mNormal;
        Vector2 mTexCoord;
    };

    struct VertexNC
    {
        Vector3 mPosition;
        Vector3 mNormal;
        Color4  mColor;
    };

    struct VertexTC
    {
        Vector3 mPosition;
        Vector2 mTexCoord;
        Color4  mColor;
    };

    struct VertexNTC
    {
        Vector3 mPosition;
        Vector3 mNormal;
        Vector2 mTexCoord;
        Color4  mColor;
    };


    class T3D_ENGINE_API VertexList : Object
    {
    public:
        enum VertexFormat
        {
            E_VF_POSITION   = 0,
            E_VF_NORMAL     = (1<<0),
            E_VF_TEXCOORD   = (1<<1),
            E_VF_COLOR      = (1<<2),
            E_VF_NT         = E_VF_NORMAL | E_VF_TEXCOORD,
            E_VF_NC         = E_VF_NORMAL | E_VF_COLOR,
            E_VF_TC         = E_VF_TEXCOORD | E_VF_COLOR,
            E_VF_NTC        = E_VF_NORMAL | E_VF_TEXCOORD | E_VF_COLOR,
        };

        VertexList(VertexFormat vertexFormat, uint32_t vertexCount, char *vertices = NULL);
        ~VertexList();

        VertexFormat getVertexFormat() const;

        uint32_t getVertexSize() const;
        uint32_t getVertexCount() const;

        const Vector3 &getPosition(uint32_t index) const;
        Vector3 &getPosition(uint32_t index);

        const Vector2 &getTexCoord(uint32_t index) const;
        Vector2 &getTexCoord(uint32_t index);

        const Vector3 &getNormal(uint32_t index) const;
        Vector3 &getNormal(uint32_t index);

        const Color4 &getColor(uint32_t index) const;
        Color4 &getColor(uint32_t index);

        char *getVertices() const;

        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexP *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexN *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexT *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexC *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexNT *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexNC *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexTC *vertices);
        void setVertices(uint32_t startIdx, uint32_t vertexCount, VertexNTC *vertices);

    private:
        void setVertices(uint32_t startIdx, uint32_t vertexCount, char *vertices);

        VertexFormat   mVertexFormat;

        uint32_t    mVertexSize;
        uint32_t    mVertexCount;

        char        *mVertices;
        bool        mUserBuffer;
    };
}


#include "T3DVertexList.inl"


#endif  /*__T3D_VERTEX_LIST_H__*/
