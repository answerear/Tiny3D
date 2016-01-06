

#include "DataStruct/T3DVertexList.h"


namespace Tiny3D
{
    VertexList::VertexList(VertexFormat vertexFormat, uint32_t vertexCount, char *vertices /* = nullptr */)
        : mVertexFormat(vertexFormat)
        , mVertexSize(0)
        , mVertexCount(vertexCount)
        , mVertices(vertices)
        , mUserBuffer(vertices != nullptr)
    {
        switch (mVertexFormat)
        {
        case E_VF_POSITION:
            {
                mVertexSize = sizeof(VertexP);
            }
            break;
        case E_VF_NORMAL:
            {
                mVertexSize = sizeof(VertexN);
            }
            break;
        case E_VF_TEXCOORD:
            {
                mVertexSize = sizeof(VertexT);
            }
            break;
        case E_VF_COLOR:
            {
                mVertexSize = sizeof(VertexC);
            }
            break;
        case E_VF_NT:
            {
                mVertexSize = sizeof(VertexNT);
            }
            break;
        case E_VF_NC:
            {
                mVertexSize = sizeof(VertexNC);
            }
            break;
        case E_VF_TC:
            {
                mVertexSize = sizeof(VertexTC);
            }
            break;
        case E_VF_NTC:
            {
                mVertexSize = sizeof(VertexNTC);
            }
            break;
        default:
            break;
        }

        if (!mUserBuffer)
        {
            uint32_t bufSize = mVertexSize * mVertexCount;
            mVertices = new char[bufSize];
            memset(mVertices, 0, bufSize);
        }
    }

    VertexList::~VertexList()
    {
        if (mUserBuffer)
        {
            delete []mVertices;
        }
    }

    const Vector3 &VertexList::getPosition(uint32_t index) const
    {
        T3D_ASSERT(index < mVertexCount);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_POSITION:
                {
                    VertexP *vertices = (VertexP *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NORMAL:
                {
                    VertexN *vertices = (VertexN *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_TEXCOORD:
                {
                    VertexT *vertices = (VertexT *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_COLOR:
                {
                    VertexC *vertices = (VertexC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NT:
                {
                    VertexNT *vertices = (VertexNT *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NC:
                {
                    VertexNC *vertices = (VertexNC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_TC:
                {
                    VertexTC *vertices = (VertexTC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            default:
                break;
            }
        }

        return Vector3::ZERO;
    }

    Vector3 &VertexList::getPosition(uint32_t index)
    {
        T3D_ASSERT(index < mVertexCount);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_POSITION:
                {
                    VertexP *vertices = (VertexP *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NORMAL:
                {
                    VertexN *vertices = (VertexN *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_TEXCOORD:
                {
                    VertexT *vertices = (VertexT *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_COLOR:
                {
                    VertexC *vertices = (VertexC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NT:
                {
                    VertexNT *vertices = (VertexNT *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NC:
                {
                    VertexNC *vertices = (VertexNC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_TC:
                {
                    VertexTC *vertices = (VertexTC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mPosition;
                }
                break;
            default:
                break;
            }
        }

        static Vector3 pos;
        return pos;
    }

    const Vector2 &VertexList::getTexCoord(uint32_t index) const
    {
        T3D_ASSERT(index < mVertexCount);
        T3D_ASSERT(mVertexFormat & E_VF_TEXCOORD);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_TEXCOORD:
                {
                    VertexT *vertices = (VertexT *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            case E_VF_NT:
                {
                    VertexNT *vertices = (VertexNT *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            case E_VF_TC:
                {
                    VertexTC *vertices = (VertexTC *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            default:
                break;
            }
        }

        return Vector2::ZERO;
    }

    Vector2 &VertexList::getTexCoord(uint32_t index)
    {
        T3D_ASSERT(index < mVertexCount);
        T3D_ASSERT(mVertexFormat & E_VF_TEXCOORD);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_TEXCOORD:
                {
                    VertexT *vertices = (VertexT *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            case E_VF_NT:
                {
                    VertexNT *vertices = (VertexNT *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            case E_VF_TC:
                {
                    VertexTC *vertices = (VertexTC *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mTexCoord;
                }
                break;
            default:
                break;
            }
        }

        static Vector2 coord;
        return coord;
    }

    const Vector3 &VertexList::getNormal(uint32_t index) const
    {
        T3D_ASSERT(index < mVertexCount);
        T3D_ASSERT(mVertexFormat & E_VF_NORMAL);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_NORMAL:
                {
                    VertexN *vertices = (VertexN *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            case E_VF_NT:
                {
                    VertexNT *vertices = (VertexNT *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            case E_VF_NC:
                {
                    VertexNC *vertices = (VertexNC *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            default:
                break;
            }
        }

        return Vector3::UNIT_Z;
    }

    Vector3 &VertexList::getNormal(uint32_t index)
    {
        T3D_ASSERT(index < mVertexCount);
        T3D_ASSERT(mVertexFormat & E_VF_NORMAL);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_NORMAL:
                {
                    VertexN *vertices = (VertexN *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            case E_VF_NT:
                {
                    VertexNT *vertices = (VertexNT *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            case E_VF_NC:
                {
                    VertexNC *vertices = (VertexNC *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mNormal;
                }
                break;
            default:
                break;
            }
        }

        static Vector3 normal(0.0, 0.0, 1.0);
        return normal;
    }

    const Color4 &VertexList::getColor(uint32_t index) const
    {
        T3D_ASSERT(index < mVertexCount);
        T3D_ASSERT(mVertexFormat & E_VF_COLOR);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_COLOR:
                {
                    VertexC *vertices = (VertexC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            case E_VF_NC:
                {
                    VertexNC *vertices = (VertexNC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            case E_VF_TC:
                {
                    VertexTC *vertices = (VertexTC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            default:
                break;
            }
        }

        return Color4::WHITE;
    }

    Color4 &VertexList::getColor(uint32_t index)
    {
        T3D_ASSERT(index < mVertexCount);
        T3D_ASSERT(mVertexFormat & E_VF_COLOR);

        if (mVertices != NULL)
        {
            switch (mVertexFormat)
            {
            case E_VF_COLOR:
                {
                    VertexC *vertices = (VertexC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            case E_VF_NC:
                {
                    VertexNC *vertices = (VertexNC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            case E_VF_TC:
                {
                    VertexTC *vertices = (VertexTC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            case E_VF_NTC:
                {
                    VertexNTC *vertices = (VertexNTC *)mVertices;
                    return vertices[index].mColor;
                }
                break;
            default:
                break;
            }
        }

        static Color4 color(255, 255, 255);
        return color;
    }

    void VertexList::setVertices(uint32_t startIdx, uint32_t vertexCount, char *vertices)
    {
        T3D_ASSERT(startIdx < mVertexCount && startIdx + vertexCount <= mVertexCount);

        uint32_t offset = startIdx * mVertexSize;
        uint32_t bufSize = vertexCount * mVertexSize;
        uint32_t totalSize = mVertexSize * mVertexCount;

        T3D_ASSERT(offset + bufSize <= totalSize);

        memcpy(mVertices+offset, vertices, bufSize);
    }
}
