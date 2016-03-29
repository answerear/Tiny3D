

#include "mconv_vertex.h"


namespace mconv
{
    String VertexAttribute::vertexTypeStr() const
    {
        String s;
        switch (mVertexType)
        {
        case E_VT_POSITION:
            {
                s = "POSITION";
            }
            break;
        case E_VT_TEXCOORD:
            {
                s = "TEXCOORD";
            }
            break;
        case E_VT_NORMAL:
            {
                s = "NORMAL";
            }
            break;
        case E_VT_TANGENT:
            {
                s = "TANGENT";
            }
            break;
        case E_VT_BINORMAL:
            {
                s = "BINORMAL";
            }
            break;
        case E_VT_COLOR:
            {
                s = "COLOR";
            }
            break;
        case E_VT_BLEND_WEIGHT:
            {
                s = "BLEND_WEIGHT";
            }
            break;
        case E_VT_BLEND_INDEX:
            {
                s = "BLEND_INDEX";
            }
            break;
        }

        return s;
    }

    String VertexAttribute::dataTypeStr() const
    {
        String s;
        switch (mDataType)
        {
        case E_VT_FLOAT:
            {
                s = "float";
            }
            break;
        case E_VT_DOUBLE:
            {
                s = "double";
            }
            break;
        case E_VT_INT16:
            {
                s = "short";
            }
            break;
        case E_VT_INT32:
            {
                s = "int";
            }
            break;
        case E_VT_INT64:
            {
                s = "long";
            }
            break;
        }

        return s;
    }
}
