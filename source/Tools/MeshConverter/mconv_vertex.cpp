

#include "mconv_vertex.h"


namespace mconv
{
    String VertexAttribute::toString(VertexType eVertexType) const
    {
        String s;
        switch (eVertexType)
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

    String VertexAttribute::toString(DataType eDataType) const
    {
        String s;
        switch (eDataType)
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
                s = "int16";
            }
            break;
        case E_VT_INT32:
            {
                s = "int32";
            }
            break;
        case E_VT_INT64:
            {
                s = "int64";
            }
            break;
        }

        return s;
    }
}
